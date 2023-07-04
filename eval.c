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

sval* evlist(sexp *args, sval *env);
sval* evcond(sexp *conditions, sval *env);
sval* lookup(sexp *symbol, sval *env);
sval* bind(sexp *names, sval *values, sval *env);
sval* quasiquote(sexp *template, sval *env);
static int call_depth = 0;

sval* eval_all(sexp *expressions, sval *env) {
    sval *ret=0;
    if (call_depth++ > MAX_CALL_DEPTH) return error(ERR_CALL_DEPTH);
    while (!isempty(expressions)) {
        ret = eval1(car(expressions), env);
        if (iserror(ret)) return ret;
        expressions = cdr(expressions);
        #ifdef LOGGING_ON
        printf("\n");
        #endif
    }
    call_depth--;
    return ret;
}

sval* _eval1(sexp* expression, sval* env) {
    if (expression->tag == NUMBER) return expression;
    else if (expression->tag == SYMBOL) return cdr(lookup(expression, env));
    else if (expression->tag == CONSTANT) return expression;
    else if (expression->tag == STRING) return expression;
    else if (expression->tag == SPECIAL_FORM) return expression;
    else if (expression->tag == FUNCTION) return error(ERR_EVAL_CLOSURE);
    else if (expression->tag == PRIMITIVE) return error(ERR_EVAL_PRIMITIVE);
    else if (expression->tag == MACRO) return error(ERR_EVAL_MACRO);
    else if (expression->tag == ERROR) return expression;
    else if (expression->tag == PAIR) { // An application
        sval *proc = eval1(car(expression), env);
        sval *rest = cdr(expression);
        if (proc == COND) {
            // (cond (<cond1> <val1>) (<cond2> <val2>) (else <val3>))
            return evcond(rest, env);
        } else if (proc == DEFINE || proc == DEFINE_MACRO) {
            if (isempty(rest) || isempty(cdr(rest))) return error(ERR_WRONG_NUM_FORM, "define/define-macro");
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
            return define(env, name, value);
        } else if (proc == LAMBDA) {
            // (lambda (<param1> <param2>) <body>)
            // (lambda (<param1> <param2> . <params>) <body>)
            // (lambda <params> <body>)
            if (isempty(rest) || isempty(cdr(rest))) return error(ERR_WRONG_NUM_FORM, "lambda");
            else return make_function(car(rest), cdr(rest), env);
        } else if (proc == QUASIQUOTE) {
            if (!islistoflength(rest, 1)) return error(ERR_WRONG_NUM_FORM, "quasiquote");
            return quasiquote(car(rest), env);
        } else if (proc == QUOTE) {
            if (!islistoflength(rest, 1)) return error(ERR_WRONG_NUM_FORM, "quote");
            return car(rest);
        } else if (proc == SET) {
            if (!islistoflength(rest, 2)) return error(ERR_WRONG_NUM_FORM, "set!");
            return set(env, car(rest), eval1(car(cdr(rest)), env));
        } else if (proc->tag == SPECIAL_FORM && (proc == UNQUOTE || proc == UNQUOTE_SPLICING)) {
            return error(ERR_UNQUOTE_NOWHERE);
        } else if (proc->tag == PRIMITIVE || proc->tag == FUNCTION) return apply(proc, evlist(rest, env));
        else if (proc->tag == MACRO) return eval1(apply(proc->body.macro_procedure, rest), env);
        else if (proc->tag == ERROR) return proc;
        else return error(ERR_APPLY_NON_FUNCTION);
    } else return error(ERR_EVAL_UNKNOWN);
}


sval* _apply(sval* proc, sval* args) {
    if (iserror(proc)) return proc;
    else if (iserror(args)) return args;

    if (proc->tag == PRIMITIVE) return proc->body.primitive(args);
    else return eval_all(
        proc->body.closure.body,
        bind(proc->body.closure.parameters, args, proc->body.closure.env));
}

sval* evlist(sexp *args, sval *env) {
    // TODO: complain if we pass a macro, since it will not work right
    if (isempty(args)) return args;
    else if(ispair(args)) {
        return make_cons(
            eval1(car(args), env),
            evlist(cdr(args), env)
        );
    } else return error(ERR_EVLIST_NON_LIST);
}

sval* evcond(sexp *conditions, sval *env) {
    if (isempty(conditions)) return NIL;
    else {
        sexp *condition = eval1(car(car(conditions)), env);
        if (iserror(condition)) return condition;
        sexp *body = cdr(car(conditions));
        if (!isfalse(condition)) return eval_all(body, env);
        else return evcond(cdr(conditions), env);
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

sval* quasiquote(sexp *template, sval *env) {
    // (quasiquote (a b (unquote c) d)) => (a b c d)
    // (quasiquote (a b (unquote-splicing (list c d)) e)) => (a b c d e)
    if (iserror(template)) return template;
    if (iserror(env)) return env;

    if (!ispair(template)) return template;
    sexp *first=car(template), *rest=cdr(template);
    if (iseqv(first, UNQUOTE)) return eval1(car(cdr(template)), env);
    else if (iseqv(car(first), UNQUOTE_SPLICING)) return append(
        eval1(car(cdr(first)), env),
        quasiquote(rest, env));
    else return cons(quasiquote(first, env), quasiquote(rest, env));
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
    if (isempty(BUILTINS_ENV->body.env.frame)) {
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


#ifdef LOGGING_ON
static int indent = 0;
#endif
inline sval* apply(sval* proc, sval* args) {
    #ifdef LOGGING_ON
        for (int i=0; i<indent; i++) printf(" ");
        printf("Apply: "); print1(proc); printf(" TO "); print1nl(args);
        indent += 2;
    #endif 
    if (call_depth++ > MAX_CALL_DEPTH) return error(ERR_CALL_DEPTH);
    sval *ret = _apply(proc, args);
    call_depth--;
    #ifdef LOGGING_ON
        indent -= 2;
        for (int i=0; i<indent; i++) printf(" ");
        printf("=> "); print1nl(ret);
    #endif
    return ret;
}

inline sval* eval1(sexp* expression, sval* env) {
    #ifdef LOGGING_ON
        for (int i=0; i<indent; i++) printf(" ");
        printf("Eval: "); print1(expression); printf(" IN "); print1(env); printf("\n");
        indent += 2;
    #endif
    if (call_depth++ > MAX_CALL_DEPTH) return error(ERR_CALL_DEPTH);
    sval *ret = _eval1(expression, env);
    call_depth--;
    #ifdef LOGGING_ON
        indent -= 2;
        for (int i=0; i<indent; i++) printf(" ");
        printf("=> "); print1nl(ret);
    #endif
    return ret;
}
