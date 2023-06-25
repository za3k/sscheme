#include "constants.h"
#include "eval.h"
#include "errors.h"
#include "helpers.h"
#include "types.h"
#include "prims.h"
#include <stdlib.h>
#include <stdio.h>

sval* evlist(sexp *args, struct senv *env);
sval* evcond(sexp *conditions, struct senv *env);
sval* lookup(sexp *symbol, struct senv *env);
sval* apply_primitive(sval* (*primitive)(sval*), sexp *args);
struct senv* bind(sexp *parameters, sval *values, struct senv *env);

sval* eval_all(sexp *expressions, struct senv *env) {
    sval *ret=0;
    while (!isempty(expressions)) {
        ret = eval1(car(expressions), env);
        expressions = cdr(expressions);
    }
    return ret;
}

sval* eval1(sexp* expression, struct senv* env) {
    if (expression->tag == NUMBER) return expression;
    else if (expression->tag == SYMBOL) return lookup(expression, env);
    else if (expression->tag == CONSTANT) return expression;
    else if (expression->tag == PRIMITIVE) return expression;
    else if (expression->tag == SPECIAL_FORM) return expression;
    else if (expression->tag == FUNCTION) return error(ERR_EVAL_CLOSURE);
    else if (expression->tag == ERROR) return expression;
    else if (expression->tag == CONS) { // An application
        sval *proc = eval1(car(expression), env);
        sval *rest = cdr(expression);
        if (proc->tag == SPECIAL_FORM && proc->body.form == form_quote) {
            if (!islistoflength(rest, 1)) return error(ERR_WRONG_NUM);
            return car(rest);
        } else if (proc->tag == SPECIAL_FORM && proc->body.form == form_lambda) {
            if (!islistoflength(rest, 2)) return error(ERR_WRONG_NUM);
            else return make_function(car(rest), car(cdr(rest)), env);
        } else if (proc->tag == SPECIAL_FORM && proc->body.form == form_cond) {
            if (!islistoflength(rest, 1)) return error(ERR_WRONG_NUM);
            return evcond(car(rest), env);
        } else if (proc->tag == SPECIAL_FORM && proc->body.form == form_define) {
            if (!islistoflength(rest, 2)) return error(ERR_WRONG_NUM);
            struct sval *arg1= car(rest), *arg2 = car(cdr(rest));
            if (!issymbol(arg1)) return error(ERR_DEFINE_NONSYMBOL);
            define(env, arg1->body.symbol, arg2);
            return &NIL_V;
        } else if (proc->tag == PRIMITIVE) return apply_primitive(proc->body.primitive, evlist(rest, env));
        else if (proc->tag == FUNCTION) return apply(proc, evlist(rest, env));
        else if (proc->tag == ERROR) return proc;
        else return error(ERR_APPLY_NON_FUNCTION);
    } else return error(ERR_EVAL_UNKNOWN);
}

sval* apply(sval* proc, sval* args) {
    if (args->tag == ERROR) return args;
    return eval1(
        proc->body.closure.body,
        bind(proc->body.closure.parameters, args, proc->body.closure.env));
}

sval* evlist(sexp *args, struct senv *env) {
    if (args->tag == CONSTANT && args->body.constant == EMPTY_LIST) return args;
    else if(args->tag == CONS) {
        return make_cons(
            eval1(car(args), env),
            evlist(cdr(args), env)
        );
    } else return error(ERR_EVLIST_NON_LIST);
}

sval* evcond(sexp *conditions, struct senv *env) {
    if (isempty(conditions)) return &NIL_V;
    else {
        sexp *condition = eval1(car(car(conditions)), env);
        if (condition->tag == ERROR) return condition;
        sexp *body = car(cdr(car(conditions)));
        if (!isfalse(condition)) return eval1(body, env);
        else return evcond(cdr(conditions), env);
    }
}

sval* apply_primitive(sval* (*primitive)(sval*), sexp *args) {
    if (args->tag == ERROR) return args;
    else return primitive(args);
}

struct senv* bind(sexp *parameters, sval *values, struct senv *env) {
    // TODO: Check if 'parameters' and 'values' are lists of the same length, complain if not
    struct senv *new_env = malloc(sizeof(struct senv));
    new_env->parent = env;
    new_env->frame.names = parameters;
    new_env->frame.values = values;
    return new_env;
}

sval* lookup_frame(sexp *symbol, sexp *parameters, sval *values) {
    if (!islist(parameters)) error(ERR_FRAME_NON_LIST);
    if (!islist(values)) error(ERR_FRAME_NON_LIST);
    
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

sval* lookup(sexp *symbol, struct senv *env) {
    if (env == 0) return error(ERR_SYMBOL_NOT_FOUND);
    sval* res = lookup_frame(symbol, env->frame.names, env->frame.values);
    if (res != 0) return res;
    else return lookup(symbol, env->parent);
}

void define(struct senv *env, char* symbol, sval* thing) {
    env->frame.names = make_cons(make_symbol(symbol), env->frame.names);
    env->frame.values = make_cons(thing, env->frame.values);
}
struct senv* empty_env() {
    if (isempty(BASE_ENV.frame.names)) {
        // Set up initial bindings
        define(&BASE_ENV, "lambda", &LAMBDA_V);
        define(&BASE_ENV, "cond", &COND_V);
        define(&BASE_ENV, "quote", &QUOTE_V);
        define(&BASE_ENV, "define", &DEFINE_V);
        define(&BASE_ENV, "nil", &NIL_V);
        define(&BASE_ENV, "else", &TRUE_V);
        define(&BASE_ENV, "eq?", make_prim(prim_eqp));
        define(&BASE_ENV, "+", make_prim(prim_plus));
        define(&BASE_ENV, "-", make_prim(prim_minus));
        define(&BASE_ENV, "cons", make_prim(prim_cons));
        define(&BASE_ENV, "car", make_prim(prim_car));
        define(&BASE_ENV, "cdr", make_prim(prim_cdr));
        define(&BASE_ENV, "nil?", make_prim(prim_nilp));
        define(&BASE_ENV, "null?", make_prim(prim_emptyp));
        define(&BASE_ENV, "pair?", make_prim(prim_listp));
        define(&BASE_ENV, "number?", make_prim(prim_numberp));
        define(&BASE_ENV, "procedure?", make_prim(prim_procedurep));
        define(&BASE_ENV, "list", make_prim(prim_list));
        define(&BASE_ENV, "display", make_prim(prim_print));
    }
    return bind(make_empty(), make_empty(), &BASE_ENV); // Return an empty frame so we can 'define' and modify it.
}
