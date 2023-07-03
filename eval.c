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
sval* bind(sexp *parameters, sval *values, sval *env);
sval* quasiquote(sexp *template, sval *env);

sval* eval_all(sexp *expressions, sval *env) {
    sval *ret=0;
    while (!isempty(expressions)) {
        ret = eval1(car(expressions), env);
        if (ret->tag == ERROR) return ret;
        expressions = cdr(expressions);
        #ifdef LOGGING_ON
        printf("\n");
        #endif
    }
    return ret;
}

sval* _eval1(sexp* expression, sval* env) {
    if (expression->tag == NUMBER) return expression;
    else if (expression->tag == SYMBOL) return lookup(expression, env);
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
        if (proc->tag == SPECIAL_FORM && proc->body.form == form_cond) {
            // (cond (<cond1> <val1>) (<cond2> <val2>) (else <val3>))
            return evcond(rest, env);
        } else if (proc->tag == SPECIAL_FORM && (proc->body.form == form_define || proc->body.form == form_define_macro)) {
            if (!islistoflength(rest, 2)) return error(ERR_WRONG_NUM_FORM, "define/define-macro");
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
                body = car(cdr(rest));
                value = make_function(params, body, env);
            } else {
                // (define x <value>)
                name = car(rest);
                value = eval1(car(cdr(rest)), env);
            }
            if (proc->body.form == form_define_macro) value = make_macro(value);
            return define(env, name, value);
        } else if (proc->tag == SPECIAL_FORM && proc->body.form == form_lambda) {
            // (lambda (<param1> <param2>) <body>)
            // (lambda (<param1> <param2> . <params>) <body>)
            // (lambda <params> <body>)
            if (!islistoflength(rest, 2)) return error(ERR_WRONG_NUM_FORM, "lambda");
            else return make_function(car(rest), car(cdr(rest)), env);
        } else if (proc->tag == SPECIAL_FORM && proc->body.form == form_quote) {
            if (!islistoflength(rest, 1)) return error(ERR_WRONG_NUM_FORM, "quote");
            return car(rest);
        } else if (proc->tag == SPECIAL_FORM && (proc->body.form == form_unquote ||
                                                 proc->body.form == form_unquote_splicing)) {
            return error(ERR_UNQUOTE_NOWHERE);
        } else if (proc->tag == SPECIAL_FORM && proc->body.form == form_quasiquote) {
            if (!islistoflength(rest, 1)) return error(ERR_WRONG_NUM_FORM, "quasiquote");
            return quasiquote(car(rest), env);
        } else if (proc->tag == PRIMITIVE || proc->tag == FUNCTION) return apply(proc, evlist(rest, env));
        else if (proc->tag == MACRO) return eval1(apply(proc->body.macro_procedure, rest), env);
        else if (proc->tag == ERROR) return proc;
        else return error(ERR_APPLY_NON_FUNCTION);
    } else return error(ERR_EVAL_UNKNOWN);
}


sval* _apply(sval* proc, sval* args) {
    if (proc->tag == ERROR) return proc;
    else if (args->tag == ERROR) return args;

    if (proc->tag == PRIMITIVE) return proc->body.primitive(args);
    else return eval1(
        proc->body.closure.body,
        bind(proc->body.closure.parameters, args, proc->body.closure.env));
}

sval* evlist(sexp *args, sval *env) {
    // TODO: complain if we pass a macro, since it will not work right
    if (isempty(args)) return args;
    else if(args->tag == PAIR) {
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
        if (condition->tag == ERROR) return condition;
        sexp *body = car(cdr(car(conditions)));
        if (!isfalse(condition)) return eval1(body, env);
        else return evcond(cdr(conditions), env);
    }
}

sval* bind(sexp *parameters, sval *values, sexp *env) {
    sval* newenv;
    if (env->tag == ERROR) return env;
    if (parameters->tag == ERROR) return parameters;
    if (values->tag == ERROR) return values;

    if (isempty(parameters) && isempty(values)) { // Base case
        return make_env(env);
    } else if (ispair(parameters) && ispair(values)) { // Recursion
        newenv = bind(cdr(parameters), cdr(values), env);
        define(newenv, car(parameters), car(values));
        return newenv;
    } else if (issymbol(parameters)) { // varargs
        newenv = make_env(env);
        define(newenv, parameters, values);
        return newenv;
    } else if (isempty(parameters)) return error(ERR_TOO_MANY_PARAM);
    else if (isempty(values)) return error(ERR_TOO_FEW_PARAM);
    else return error(ERR_BIND_UNKNOWN);
}

sval* lookup_frame(sexp *symbol, sexp *parameters, sval *values) {
    if (!ispair(parameters)) error(ERR_FRAME_NON_LIST);
    if (!ispair(values)) error(ERR_FRAME_NON_LIST);
    
    if (isempty(parameters) && isempty(values)) return 0;
    else if (isempty(parameters)) return error(ERR_TOO_MANY_PARAM);
    else if (isempty(values)) return error(ERR_TOO_FEW_PARAM);
    else {
        if (!issymbol(car(parameters))) error(ERR_FRAME_NON_SYMBOL);
        if (symboleq(symbol, car(parameters))) return car(values);
        else return lookup_frame(
            symbol,
            cdr(parameters),
            cdr(values)
        );
    }
}

sval* quasiquote(sexp *template, sval *env) {
    // (quasiquote (a b (unquote c) d)) => (a b c d)
    // (quasiquote (a b (unquote-splicing (list c d)) e)) => (a b c d e)
    if (iserror(template)) return template;
    if (iserror(env)) return env;

    if (!ispair(template)) return template;
    sexp *first=car(template), *rest=cdr(template);
    if (iseq(first, UNQUOTE)) return eval1(car(cdr(template)), env);
    else if (iseq(car(first), UNQUOTE_SPLICING)) return append(
        eval1(car(cdr(first)), env),
        quasiquote(rest, env));
    else return cons(quasiquote(first, env), quasiquote(rest, env));
}

sval* lookup(sexp *symbol, sval *env) {
    if (env == 0) return error(ERR_SYMBOL_NOT_FOUND, symbol->body.symbol);
    if (iserror(env)) return env;
    if (!isenv(env)) return error(ERR_EXPECTED_ENV);
    sval* res = lookup_frame(symbol, env->body.env.frame.names, env->body.env.frame.values);
    if (res != 0) return res;
    else return lookup(symbol, env->body.env.parent);
}

sval* define(sval *env, sval* symbol, sval* thing) {
    if (!issymbol(symbol)) return error(ERR_DEFINE_NONSYMBOL);
    if (iserror(env)) return env;
    if (!isenv(env)) return error(ERR_EXPECTED_ENV);
    env->body.env.frame.names = make_cons(symbol, env->body.env.frame.names);
    env->body.env.frame.values = make_cons(thing, env->body.env.frame.values);
    return NIL;
}
sval* empty_env() {
    if (isempty(BUILTINS_ENV->body.env.frame.names)) {
        // Set up character constants
        for (int i=0; i<128; i++) {
            CHARS_V[i].tag = CONSTANT;
            CHARS_V[i].body.constant = C000+i;
        }

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
        for (int i=0; primitives[i]!=0; i++)
            define(BUILTINS_ENV, make_symbol(primitive_names[i]), make_prim(primitives[i]));

        // Run the standard library
        eval_all(parse(standard_txt), STANDARD_ENV);
    }

    return bind(make_empty(), make_empty(), STANDARD_ENV); // Return an empty frame so we can 'define' and modify it.
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
    sval *ret = _apply(proc, args);
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
    sval *ret = _eval1(expression, env);
    #ifdef LOGGING_ON
        indent -= 2;
        for (int i=0; i<indent; i++) printf(" ");
        printf("=> "); print1nl(ret);
    #endif
    return ret;
}
