#include "eval.h"
#include "errors.h"
#include "helpers.h"
#include "types.h"
#include "prims.h"

#include <stdlib.h>
#include <string.h>

// TODO: Propogate errors
// TODO: lambda, cond

sval* evlist(sexp *args, struct senv *env);
sval* lookup(char *symbol, struct senv *env);
sval* apply_primitive(int which, sexp *args);
struct senv* bind(sexp *parameters, sval *values, struct senv *env);

sval* eval(sexp* expression, struct senv* env) {
    if (expression->tag == NUMBER) return expression;
    else if (expression->tag == SYMBOL) return lookup(expression->body.symbol, env);
    else if (expression->tag == CONSTANT) return expression;
    else if (expression->tag == BUILTIN_PROCEDURE) return expression;
    else if (expression->tag == FUNCTION) return error(ERR_EVAL_CLOSURE);
    else if (expression->tag == CONS) { // An application
        return apply(
            eval(expression->body.list.car, env),
            evlist(expression->body.list.cdr, env)
        );
    } else return error(ERR_EVAL_UNKNOWN);
}

sval* apply(sval* proc, sval* args) {
    if (proc->tag == BUILTIN_PROCEDURE && proc->body.builtin_procedure == quote) return args;
    else if (proc->tag == BUILTIN_PROCEDURE) return apply_primitive(proc->body.builtin_procedure, args);
    else if (proc->tag == FUNCTION) { // Apply a closure
        return eval(
            proc->body.closure.body,
            bind(proc->body.closure.parameters, args, proc->body.closure.env));
    } else return error(ERR_APPLY_NON_FUNCTION);
}

sval* evlist(sexp *args, struct senv *env) {
    if (args->tag == CONSTANT && args->body.constant == EMPTY_LIST) return args;
    else if(args->tag == CONS) {
        return make_cons(
            eval(args->body.list.car, env),
            evlist(args->body.list.cdr, env)
        );
    } else return error(ERR_EVLIST_NON_LIST);
}


sval* apply_primitive(int which, sexp *args) {
    return error("Primitives do not yet exist");
}

struct senv* bind(sexp *parameters, sval *values, struct senv *env) {
    struct senv *new_env = malloc(sizeof(struct senv));
    new_env->parent = env;
    new_env->frame.names = parameters;
    new_env->frame.values = values;
    return new_env;
}

sval* lookup_frame(char *symbol, sexp *parameters, sval *values) {
    if (!islist(parameters)) error(ERR_FRAME_NON_LIST);
    if (!islist(values)) error(ERR_FRAME_NON_LIST);
    
    if (isempty(parameters) && isempty(values)) return 0;
    else if (isempty(parameters)) return error(ERR_TOO_MANY_PARAM);
    else if (isempty(values)) return error(ERR_TOO_FEW_PARAM);
    else {
        sval *sym=car(parameters);
        sval *val=car(values);
        
        if (!issymbol(sym)) error(ERR_FRAME_NON_SYMBOL);
        if (strcmp(symbol, sym->body.symbol)==0) return val;
        else return lookup_frame(
            symbol,
            parameters->body.list.cdr,
            values->body.list.cdr
        );
    }
}

sval* lookup(char *symbol, struct senv *env) {
    if (env == 0) return error(ERR_SYMBOL_NOT_FOUND);
    sval* res = lookup_frame(symbol, env->frame.names, env->frame.values);
    if (res != 0) return res;
    else return lookup(symbol, env->parent);
}

static struct senv *BASE_ENV;
void add_prim(struct senv *env, char* symbol, int prim) {
    env->frame.names = make_cons(make_symbol(symbol), env->frame.names);
    env->frame.values = make_cons(make_prim(prim), env->frame.values);
}
struct senv* empty_env() {
    if (!BASE_ENV) {
        BASE_ENV = malloc(sizeof(struct senv));
        BASE_ENV->parent = 0;
        BASE_ENV->frame.names = make_empty();
        BASE_ENV->frame.values = make_empty();
        
        add_prim(BASE_ENV, "quote", quote);
        add_prim(BASE_ENV, "cond", cond);
        add_prim(BASE_ENV, "lambda", lambda);

        add_prim(BASE_ENV, "+", fplus);
        add_prim(BASE_ENV, "-", fsubtract);
        add_prim(BASE_ENV, "cons", fcons);
        add_prim(BASE_ENV, "car", fcar);
        add_prim(BASE_ENV, "cdr", fcdr);
        add_prim(BASE_ENV, "nil?", fnilp);
        add_prim(BASE_ENV, "false?", ffalsep);
        add_prim(BASE_ENV, "true?", ftruep);
        add_prim(BASE_ENV, "empty?", femptyp);
        add_prim(BASE_ENV, "list?", femptyp);
        add_prim(BASE_ENV, "number?", femptyp);
        add_prim(BASE_ENV, "list", femptyp);
        add_prim(BASE_ENV, "print", femptyp);
    }
    return BASE_ENV;
}

#include "parser.h"
int main(int argc, char *argv[]) {
    char *EXAMPLE = "(+ 4 10)";
    if ( argc == 2) EXAMPLE=argv[1];
    sexp* parsed = parse(EXAMPLE);
    print1nl(parsed);

    sval* result = eval(parsed, empty_env());
    print1nl(result);
}
