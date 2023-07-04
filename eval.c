#include "eval.h"

#include "config.h"
#include "constants.h"
#include "errors.h"
#include "helpers.h"
#include "parser.h"
#include "prims.h"
#include "standard.inc.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>

sval* evcond(sexp *conditions, sval *env);
sval* evlist(sexp *args, sval *env);
sval* quasiquote(sexp *template, sval *env);

sval* bind(sexp *names, sval *values, sval *env);
sval* lookup(sexp *symbol, sval *env);


// The following functions call each other recursively:
// - apply (calls eval_all)
// - eval1 (calls apply, evcond, evlist, quasiquote)
// - eval_all (calls eval1)
// - evcond (calls eval1, eval_all)
// - evlist (calls eval1)
// - quasiquote (calls eval1, quasiquote)
// To deal with tail recursion, limited C stack depth, and garbage collection
// we turn these 6 functions into an explicit state machine with a stack,
// instead.

static sval* STACK=0;

void push(sval* a) {
    STACK = make_cons(a, STACK);
}
sval* pop() {
    sval* a = car(STACK);
    STACK = cdr(STACK);
    return a;
}

enum gotos {
    goto_apply_start,
    goto_eval1_start,
    goto_evalall_start,
    goto_evalall_midlist,
    goto_evcond_start,
    goto_evlist_start,
    goto_quasiquote_start,
    goto_executestack_done,
    goto_executestack_error,
};
enum gotos _apply(enum gotos resume);
enum gotos _eval1(enum gotos resume);
enum gotos _evalall(enum gotos resume);
enum gotos _evcond(enum gotos resume);
enum gotos _evlist(enum gotos resume);
enum gotos _quasiquote(enum gotos resume);

#define ERROR(x) do { push(x); return goto_executestack_error; } while(0)
#define SAVECONT(x) push(make_int(x))
#define RESTORECONT() pop()->body.smallnum
#define IMM(x) do { enum gotos returned_resume = RESTORECONT(); push(x); return returned_resume; } while(0)
#define TAILCALL(x, y, z) do { push(z); push(y); return goto_##x##_start; } while (0)
#define RESUME(x) do { if(resume == goto_##x) goto goto_##x; } while(0)
#define CALL(x, arg1, arg2, resume_name) do { SAVECONT(goto_##resume_name); push(arg2); push(arg1); return goto_##x##_start; goto_##resume_name : } while (0)
#define RECEIVE(x) x = pop()
#define SAVE(x) push(x)
#define RESTORE(x) x = pop()

sval* executestack(enum gotos andthen, sexp *arg1, sexp *arg2) {
    if (STACK==0) STACK=EMPTY_LIST;
    printf("Entering 'executestack'.\n");
    SAVECONT(goto_executestack_done);
    push(arg2);
    push(arg1);

    while (1) {
        printf("Stack size: %d, ", listlength(STACK));
        switch (andthen) {
            #define printif(x) case goto_##x: printf("\tfunction: %s\n", #x); break;
            printif(evalall_start)
            printif(apply_start)
            printif(eval1_start)
            printif(evalall_midlist)
            printif(evcond_start)
            printif(evlist_start)
            printif(quasiquote_start)
            printif(executestack_done)
            printif(executestack_error)
        }
        // In general, the form of the stack is:
        // << arg1 arg2 (resume-address saved-register1 ... saved-registerN)* return-address >>

        // Current state is that the stack contains:
        // <<
        // arg1
        // arg2
        // <Nth resume address>
        // ...more... >>
        // nextstep.next_call contains the function to call
        //
        //                      OR
        //
        // <<
        // saved-register1 (for Nth resume-address)
        // ...
        // saved-registerN
        // <(N-1)st resume address>
        // ...more... >>
        // nextstep.next_call contains the Nth resume-address, to resume

        // After calling or resuming the function:
        //  - We execute a GOTO to get to the right part of the function
        //  - All arguments or saved registers are popped.
        //  - The top of the stack is now the resume-address to return a value to.
        //  - Execution resumes in normal C code. Note that /all variables have been reset/



        // There are a few possible return modes: CALL, TAILCALL, IMM, and ERROR

        // IMM, a regular "return" results in:
        // <<
        // result
        // ...more... >>
        // nextstep.next_call contains <Nth resume address>

        // CALL, a subprocedure call which must return, results in:
        // <<
        // subarg1
        // subarg2
        // <(N+1)st resume-address>
        // saved-register1 (for (N+1)st resume-address)
        // ...
        // saved-registerN
        // <Nth resume-address>
        // ...more... >>
        // nextstep.next_call contains the function to make a subprocedure call to

        // TAILCALL, a subprocedure call which doesn't need to return, results in:
        // <<
        // subarg1
        // subarg2
        // <Nth resume address>
        // ...more... >>
        // nextstep.next_call contains the function to make a subprocedure call to

        // ERROR, a fatal error which exits execution, results in:
        // <<
        // an-error
        // ...doesn't matter... >>
        // nextstep.next_call contains 'goto_executestack_error'

        switch(andthen) {
            case goto_executestack_error: return error(ERR_LOGIC);
            case goto_executestack_done:
                return pop();
            case goto_apply_start:
                andthen = _apply(andthen);
                break;
            case goto_eval1_start:
                andthen = _eval1(andthen);
                break;
            case goto_evalall_start:
            case goto_evalall_midlist:
                andthen = _evalall(andthen);
                break;
            case goto_evcond_start:
                andthen = _evcond(andthen);
                break;
            case goto_evlist_start:
                andthen = _evlist(andthen);
                break;
            case goto_quasiquote_start:
                andthen = _quasiquote(andthen);
                break;
        }
        if (andthen == goto_executestack_error) {
            sexp *err = pop();
            while (STACK != EMPTY_LIST) pop();
            return err;
        }
    }
}

enum gotos _evalall(enum gotos resume) {
    sexp *expressions, *env, *ret=0;
    //RESUME(evalall_midlist);
    expressions = pop();
    env = pop();
    if (isempty(expressions)) IMM(NIL);
    else if (!ispair(expressions)) ERROR(error(ERR_LOGIC));
    else if (isempty(cdr(expressions))) {
        TAILCALL(evalall, car(expressions), env);
    } else {
        //SAVE(expressions);
        //SAVE(env);
        //CALL(eval1, car(expressions), env, evalall_midlist);
        //RECEIVE(ret);
        //RESTORE(env);
        //RESTORE(expressions);
        ret = eval1(car(expressions), env);
        if (iserror(ret)) ERROR(ret);
        TAILCALL(evalall, cdr(expressions), env);
    }
}

enum gotos _eval1(enum gotos resume) {
    sexp *expression = pop();
    sexp *env = pop();
    if (expression->tag == NUMBER) IMM(expression);
    else if (expression->tag == SYMBOL) IMM(cdr(lookup(expression, env)));
    else if (expression->tag == CONSTANT) IMM(expression);
    else if (expression->tag == STRING) IMM(expression);
    else if (expression->tag == SPECIAL_FORM) IMM(expression);
    else if (expression->tag == FUNCTION) ERROR(error(ERR_EVAL_CLOSURE));
    else if (expression->tag == PRIMITIVE) ERROR(error(ERR_EVAL_PRIMITIVE));
    else if (expression->tag == MACRO) ERROR(error(ERR_EVAL_MACRO));
    else if (expression->tag == ERROR) ERROR(expression);
    else if (expression->tag == PAIR) { // An application
        sval *proc = eval1(car(expression), env);
        sval *rest = cdr(expression);
        if (proc == COND) {
            // (cond (<cond1> <val1>) (<cond2> <val2>) (else <val3>))
            TAILCALL(evcond, rest, env);
        } else if (proc == DEFINE || proc == DEFINE_MACRO) {
            if (isempty(rest) || isempty(cdr(rest))) ERROR(error(ERR_WRONG_NUM_FORM, "define/define-macro"));
            sval *name;
            sval *value;
            if (ispair(car(rest))) {
                // (define (<func-name> <param1> <param2>) <body>)
                // (define (<func-name> . <params>) <body>)
                // (define (<func-name> <param1> <param2> . <params>) <body>)
                sval *params;
                sval *body;
                name = car(car(rest));
                params = cdr(car(rest));
                body = cdr(rest);
                value = make_function(params, body, env);
            } else {
                // (define x <value>)
                name = car(rest);
                value = eval1(car(cdr(rest)), env);
            }
            if (proc == DEFINE_MACRO) value = make_macro(value);
            IMM(define(env, name, value));
        } else if (proc == LAMBDA) {
            // (lambda (<param1> <param2>) <body>)
            // (lambda (<param1> <param2> . <params>) <body>)
            // (lambda <params> <body>)
            if (isempty(rest) || isempty(cdr(rest))) ERROR(error(ERR_WRONG_NUM_FORM, "lambda"));
            else IMM(make_function(car(rest), cdr(rest), env));
        } else if (proc == QUASIQUOTE) {
            if (!islistoflength(rest, 1)) ERROR(error(ERR_WRONG_NUM_FORM, "quasiquote"));
            TAILCALL(quasiquote, car(rest), env);
        } else if (proc == QUOTE) {
            if (!islistoflength(rest, 1)) ERROR(error(ERR_WRONG_NUM_FORM, "quote"));
            IMM(car(rest));
        } else if (proc == SET) {
            if (!islistoflength(rest, 2)) ERROR(error(ERR_WRONG_NUM_FORM, "set!"));
            IMM(set(env, car(rest), eval1(car(cdr(rest)), env)));
        } else if (proc->tag == SPECIAL_FORM && (proc == UNQUOTE || proc == UNQUOTE_SPLICING)) {
            ERROR(error(ERR_UNQUOTE_NOWHERE));
        } else if (proc->tag == PRIMITIVE || proc->tag == FUNCTION) {
            sval *ret = evlist(rest, env);
            TAILCALL(apply, proc, ret);
        } else if (proc->tag == MACRO) {
            sval *ret = apply(proc->body.macro_procedure, rest);
            TAILCALL(eval1, ret, env);
        } else if (proc->tag == ERROR) ERROR(proc);
        else ERROR(error(ERR_APPLY_NON_FUNCTION));
    } else ERROR(error(ERR_EVAL_UNKNOWN));
}

enum gotos _apply(enum gotos resume) {
    sval *proc = pop();
    sval *args = pop();
    if (iserror(proc)) ERROR(proc);
    else if (iserror(args)) ERROR(args);

    if (proc->tag == PRIMITIVE) IMM(proc->body.primitive(args));
    else TAILCALL(
        evalall,
        proc->body.closure.body,
        bind(proc->body.closure.parameters, args, proc->body.closure.env));
}

enum gotos _evlist(enum gotos resume) {
    sexp *args = pop();
    sexp *env = pop();
    if (isempty(args)) IMM(args);
    args = reverse(args);
    sexp *ret = EMPTY_LIST;
    while (ispair(args)) {
        if (ismacro(car(args))) ERROR(error(ERR_PASSED_MACRO));
        sexp *first = eval1(car(args), env);
        ret = make_cons(first, ret);
        args = cdr(args);
    }
    if (!isempty(args)) ERROR(error(ERR_EVLIST_NON_LIST));
    IMM(ret);
}

enum gotos _evcond(enum gotos resume) {
    sexp *conditions = pop();
    sexp *env = pop();
    while (ispair(conditions)) {
        sexp *clause = car(conditions);
        conditions = cdr(conditions);
        if (isempty(clause)||isempty(cdr(clause))) ERROR(error(ERR_EVCOND_INVALID));
        sexp *condition = eval1(car(clause), env);
        if (iserror(condition)) ERROR(condition);
        if (!isfalse(condition)) TAILCALL(evalall, cdr(clause), env);
    }
    if (isempty(conditions)) IMM(NIL);
    ERROR(error(ERR_EVCOND_INVALID));
}

enum gotos _quasiquote(enum gotos resume) {
    sexp *template = pop();
    sexp *env = pop();
    // (quasiquote (a b (unquote c) d)) => (a b c d)
    // (quasiquote (a b (unquote-splicing (list c d)) e)) => (a b c d e)
    if (iserror(template)) ERROR(template);
    if (iserror(env)) ERROR(env);

    if (!ispair(template)) IMM(template);
    sexp *first=car(template), *rest=cdr(template);
    if (iseqv(first, UNQUOTE)) TAILCALL(eval1, car(cdr(template)), env);
    else if (iseqv(car(first), UNQUOTE_SPLICING)) {
        sval *ret1 = eval1(car(cdr(first)), env);
        sval *ret2 = quasiquote(rest, env);
        IMM(append(ret1, ret2));
    } else {
        sval *ret1 = quasiquote(first, env);
        sval *ret2 = quasiquote(rest, env);
        IMM(cons(ret1, ret2));
    }
}

sval* bind(sexp *names, sval *values, sexp *env) {
    if (iserror(env)) return env;
    if (iserror(names)) return names;
    if (iserror(values)) return values;
    if (!isenv(env)) return error(ERR_BIND_UNKNOWN);

    sexp *e, *newenv = make_env(env);
    while (ispair(names) && ispair(values)) { // Normal definitions
        // Technically this defines things in reverse order, but that should be fine.
        if (iserror(e=define(newenv, car(names), car(values)))) return e;
        names = cdr(names);
        values = cdr(values);
    }
    if (issymbol(names)) { // varargs forms
        if (iserror(e=define(newenv, names, values))) return e;
        names = values = EMPTY_LIST;
    } 
    if (isempty(names) && isempty(values)) return newenv;
    if (ispair(names)) return error(ERR_TOO_FEW_PARAM);
    if (ispair(values)) return error(ERR_TOO_MANY_PARAM);
    return error(ERR_BIND_UNKNOWN);
}


sval* lookup(sexp *symbol, sval *env) {
    if (iserror(env)) return env;
    if (env && !isenv(env)) return error(ERR_EXPECTED_ENV);
    while (env) {
        sexp *frame = env->body.env.frame;
        while (!isempty(frame)) {
            if (symboleq(symbol, car(car(frame)))) return car(frame);
            frame = cdr(frame);
        }
        env = env->body.env.parent;
    }
    return error(ERR_SYMBOL_NOT_FOUND, symbol->body.symbol);
}

sval* define(sval *env, sval* symbol, sval* thing) {
    // Allows redefinitions in the same frame, which will shadow. This is a bug I don't plan to fix.
    if (!issymbol(symbol)) return error(ERR_DEFINE_NONSYMBOL);
    if (iserror(env)) return env;
    if (iserror(thing)) return thing;
    if (!isenv(env)) return error(ERR_EXPECTED_ENV);

    sval *newframe = make_cons(
        make_cons(symbol, thing),
        env->body.env.frame);
    if (iserror(newframe)) return newframe;
    env->body.env.frame = newframe;
    return NIL;
}

sval* set(sval *env, sval* symbol, sval* thing) {
    if (!issymbol(symbol)) return error(ERR_DEFINE_NONSYMBOL);
    if (iserror(env)) return env;
    if (!isenv(env)) return error(ERR_EXPECTED_ENV);

    sexp *binding = lookup(symbol, env);
    if (iserror(binding)) return binding;
    setcdr(binding, thing);

    return NIL;
}

sval* empty_env() {
    if (0&&isempty(BUILTINS_ENV->body.env.frame)) {
        // Set up character constants
        for (int i=0; i<128; i++) CHARS_V[i].tag = CONSTANT;

        // Set up builtins
        define(BUILTINS_ENV, make_symbol("cond"), COND);
        define(BUILTINS_ENV, make_symbol("define"), DEFINE);
        define(BUILTINS_ENV, make_symbol("define-macro"), DEFINE_MACRO);
        define(BUILTINS_ENV, make_symbol("lambda"), LAMBDA);
        define(BUILTINS_ENV, make_symbol("nil"), NIL);
        define(BUILTINS_ENV, make_symbol("unquote"), UNQUOTE);
        define(BUILTINS_ENV, make_symbol("unquote-splicing"), UNQUOTE_SPLICING);
        define(BUILTINS_ENV, make_symbol("quasiquote"), QUASIQUOTE);
        define(BUILTINS_ENV, make_symbol("quote"), QUOTE);
        define(BUILTINS_ENV, make_symbol("set!"), SET);
        for (int i=0; primitives[i]!=0; i++)
            define(BUILTINS_ENV, make_symbol(primitive_names[i]), make_prim(primitives[i]));

        // Run the standard library
        eval_all(parse(standard_txt), STANDARD_ENV);
    }

    return bind(EMPTY_LIST, EMPTY_LIST, STANDARD_ENV); // Return an empty frame so we can 'define' and modify it.
}


inline sval* apply   (sval* proc,        sval* args) { return executestack(goto_apply_start, proc, args); }
inline sval* eval1   (sexp* expression,  sval* env)  { return executestack(goto_eval1_start, expression, env); }
inline sval* eval_all(sexp* expressions, sval* env)  { return executestack(goto_evalall_start, expressions, env); }
inline sval* evlist  (sexp* args,        sval* env)  { return executestack(goto_evlist_start, args, env); }
inline sval* evcond  (sexp* conditions,  sval* env)  { return executestack(goto_evcond_start, conditions, env); }
inline sval* quasiquote(sexp* template,  sval* env)  { return executestack(goto_quasiquote_start, template, env); }
