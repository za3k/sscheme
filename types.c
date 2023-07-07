#include "types.h"

#include "allocator.h"
#include "config.h"
#include "constants.h"
#include "errors.h"
#include "helpers.h"
#include "prims.h"
#include "stdarg.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*  Make the base types  */

char ERR_BUF[MAX_STRING_SIZE];
sval* error(char *msg, ...) {
    va_list va;
    va_start(va, msg);
    vsprintf(ERR_BUF, msg, va);
    va_end(va);
    sval *err = make_cell();
    err->tag = ERROR;
    err->body.error = strdup(ERR_BUF);
    return err;
}

sval* make_cons(sval *car, sval *cdr) {
    if (iserror(car)) return car;
    if (iserror(cdr)) return cdr;
    sval *v = make_cell();
    if (iserror(v)) return v;
    v->tag = PAIR;
    _setcar(v, car);
    _setcdr(v, cdr);
    return v;
}

sval* make_env(sval* env) {
    // Caller is expected to mutate to set frame
    if (env) {
        if (iserror(env)) return env;
        if (!isenv(env)) return error(ERR_WRONG_TYPE);
    }
    sval *v = make_cell();
    if (iserror(v)) return v;
    v->tag = ENV;
    v->body.env.parent = env;
    v->body.env.frame = EMPTY_LIST;
    return v;
}

sval* make_int(int i) {
    sval *v = make_cell();
    if (iserror(v)) return v;
    v->tag = NUMBER;
    v->body.smallnum = i;
    return v;
}

sval* make_symbol(char* name) {
    sval *v = make_cell();
    if (iserror(v)) return v;
    v->tag = SYMBOL;
    v->body.symbol = strdup(name);
    return v;
}
sval* make_string(char* str) {
    sval *v = make_cell();
    if (iserror(v)) return v;
    v->tag = STRING;
    v->body.symbol = strdup(str);
    return v;
}

sval* make_prim(sval* (*primitive)(sval*)) {
    sval *v = make_cell();
    if (iserror(v)) return v;
    v->tag = PRIMITIVE;
    v->body.primitive = primitive;
    return v;
}

sexp* make_function(sexp *parameters, sexp *body, sval *env) {
    sval *v = make_cell();
    if (iserror(v)) return v;
    if (iserror(env)) return env;
    if (!isenv(env)) return 0;

    v->tag = FUNCTION;
    v->body.closure.parameters = parameters;
    v->body.closure.body = body;
    v->body.closure.env = env;
    return v;
}

sexp* make_macro(sexp *function) {
    sval *v = make_cell();
    if (iserror(v)) return v;
    v->tag = MACRO;
    v->body.macro_procedure = function;
    return v;
}

sval* make_character_constant (char c) { 
    if (((unsigned int)c) >= 128) return error(ERR_INVALID_CHAR);
    return &CHARS_V[(int)c];
};

// PAIR
sval* _car(sval *pair) { return pair->body.list.car; }
sval* _cdr(sval *pair) { return pair->body.list.cdr; }
void _setcar(sval *pair, sval *v) { pair->body.list.car = v; }
void _setcdr(sval *pair, sval *v) { pair->body.list.cdr = v; }
// ENV
sval* _env_frame(sval *env) { return env->body.env.frame; }
sval* _env_parent(sval *env) { return env->body.env.parent; }
void _set_env_frame(sval *env, sval *v) { env->body.env.frame = v; }
// MACRO
sval* _macro_procedure(sval *macro) { return macro->body.macro_procedure; }
// FUNCTION
sval* _function_args(sval *function) { return function->body.closure.parameters; }
sval* _function_body(sval *function) { return function->body.closure.body; }
sval* _function_env(sval *function) { return function->body.closure.env; }
