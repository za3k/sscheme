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
    err->body.symbol = strdup(ERR_BUF);
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
    if (iserror(env)) return env;
    if (env != EMPTY_LIST && !isenv(env)) return error(ERR_WRONG_TYPE);
    sval *v = make_cell();
    if (iserror(v)) return v;
    v->tag = ENV;
    _setcar(v, EMPTY_LIST); // empty list
    _setcdr(v, env); // parent
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
    if (iserror(env)) return env;
    if (!isenv(env)) return 0;

    sval *v1 = make_cell();
    if (iserror(v1)) return v1;
    sval *v2 = make_cell();
    if (iserror(v2)) return v2;

    v1->tag = FUNCTION;
    v2->tag = FUNCTION;

    _setcar(v1, parameters);
    _setcdr(v1, v2);
    _setcar(v2, body);
    _setcdr(v2, env);
    return v1;
}

sexp* make_macro(sexp *function) {
    sval *v = make_cell();
    if (iserror(v)) return v;
    v->tag = MACRO;
    _setcar(v, function);
    _setcdr(v, EMPTY_LIST);
    return v;
}

sval* make_character_constant (char c) { 
    if (((unsigned int)c) >= 128) return error(ERR_INVALID_CHAR);
    return &CHARS[(int)c];
};

// PAIR
sval* _car(sval *pair) { return &HEAP[pair->body.list.car]; }
sval* _cdr(sval *pair) { return &HEAP[pair->body.list.cdr]; }
int _index(sval* v) { return v ? v-HEAP : -1; }
void _setcar(sval *pair, sval *v) { pair->body.list.car = _index(v); }
void _setcdr(sval *pair, sval *v) { pair->body.list.cdr = _index(v); }
// ENV
sval* _env_frame(sval *env) { return _car(env); }
sval* _env_parent(sval *env) { return _cdr(env); }
void _set_env_frame(sval *env, sval *v) { _setcar(env, v); }
// MACRO
sval* _macro_procedure(sval *macro) { return _car(macro); }
// FUNCTION
sval* _function_args(sval *function) { return _car(function); }
sval* _function_body(sval *function) { return _car(_cdr(function)); }
sval* _function_env(sval *function) { return _cdr(_cdr(function)); }
