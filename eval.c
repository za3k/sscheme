#include "eval.h"
#include "errors.h"
#include "helpers.h"
#include "types.h"
#include "prims.h"
#include <stdlib.h>
#include <stdio.h>

// TODO: Propogate errors

sval* evlist(sexp *args, struct senv *env);
sval* evcond(sexp *conditions, struct senv *env);
sval* lookup(sexp *symbol, struct senv *env);
sval* apply_primitive(sval* (*primitive)(sval*), sexp *args);
struct senv* bind(sexp *parameters, sval *values, struct senv *env);

sval* eval(sexp* expression, struct senv* env) {
    if (expression->tag == NUMBER) return expression;
    else if (expression->tag == SYMBOL) return lookup(expression, env);
    else if (expression->tag == CONSTANT) return expression;
    else if (expression->tag == PRIMITIVE) return expression;
    else if (expression->tag == SPECIAL_FORM) return expression;
    else if (expression->tag == FUNCTION) return error(ERR_EVAL_CLOSURE);
    else if (expression->tag == ERROR) return expression;
    else if (expression->tag == CONS) { // An application
        sval *proc = eval(car(expression), env);
        sval *rest = cdr(expression);
        if (proc->tag == SPECIAL_FORM && proc->body.form == quote) {
            if (!islistoflength(rest, 1)) return error(ERR_WRONG_NUM);
            return car(rest);
        } else if (proc->tag == SPECIAL_FORM && proc->body.form == lambda) {
            if (!islistoflength(rest, 2)) return error(ERR_WRONG_NUM);
            else return make_function(car(rest), car(cdr(rest)), env);
        } else if (proc->tag == SPECIAL_FORM && proc->body.form == cond) {
            if (!islistoflength(rest, 1)) return error(ERR_WRONG_NUM);
            return evcond(car(expression), env);
        } else if (proc->tag == PRIMITIVE) return apply_primitive(proc->body.primitive, evlist(rest, env));
        else if (proc->tag == FUNCTION) return apply(proc, evlist(rest, env));
        else if (proc->tag == ERROR) return proc;
        else return error(ERR_APPLY_NON_FUNCTION);
    } else return error(ERR_EVAL_UNKNOWN);
}

sval* apply(sval* proc, sval* args) {
    if (args->tag == ERROR) return args;
    return eval(
        proc->body.closure.body,
        bind(proc->body.closure.parameters, args, proc->body.closure.env));
}

sval* evlist(sexp *args, struct senv *env) {
    if (args->tag == CONSTANT && args->body.constant == EMPTY_LIST) return args;
    else if(args->tag == CONS) {
        return make_cons(
            eval(car(args), env),
            evlist(cdr(args), env)
        );
    } else return error(ERR_EVLIST_NON_LIST);
}

sval* evcond(sexp *conditions, struct senv *env) {
    if (isempty(conditions)) return make_nil();
    else {
        sexp *condition = eval(car(car(conditions)), env);
        if (condition->tag == ERROR) return condition;
        sexp *body = car(cdr(conditions));
        if (!isfalse(condition)) return eval(body, env);
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

static struct senv *BASE_ENV;
void add_thing(struct senv *env, char* symbol, sval* thing) {
    env->frame.names = make_cons(make_symbol(symbol), env->frame.names);
    env->frame.values = make_cons(thing, env->frame.values);
}
void add_prim(struct senv *env, char* symbol, sval* (*prim)(sval*)) {
    add_thing(env, symbol, make_prim(prim));
}
struct senv* empty_env() {
    if (!BASE_ENV) {
        BASE_ENV = malloc(sizeof(struct senv));
        BASE_ENV->parent = 0;
        BASE_ENV->frame.names = make_empty();
        BASE_ENV->frame.values = make_empty();

        add_thing(BASE_ENV, "lambda", make_lambda());
        add_thing(BASE_ENV, "cond", make_cond());
        add_thing(BASE_ENV, "quote", make_quote());
        add_thing(BASE_ENV, "nil", make_nil());
        add_thing(BASE_ENV, "else", make_true());
        add_thing(BASE_ENV, "#t", make_true());
        add_thing(BASE_ENV, "#f", make_false());
        add_prim(BASE_ENV, "eq?", prim_eqp);
        add_prim(BASE_ENV, "+", prim_plus);
        add_prim(BASE_ENV, "-", prim_minus);
        add_prim(BASE_ENV, "cons", prim_cons);
        add_prim(BASE_ENV, "car", prim_car);
        add_prim(BASE_ENV, "cdr", prim_cdr);
        add_prim(BASE_ENV, "nil?", prim_nilp);
        add_prim(BASE_ENV, "empty?", prim_emptyp);
        add_prim(BASE_ENV, "list?", prim_listp);
        add_prim(BASE_ENV, "number?", prim_numberp);
        add_prim(BASE_ENV, "procedure?", prim_procedurep);
        add_prim(BASE_ENV, "list", prim_list);
        add_prim(BASE_ENV, "print", prim_print);
    }
    return BASE_ENV;
}
