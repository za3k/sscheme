#include "types.h"

#include "config.h"
#include "constants.h"
#include "errors.h"
#include "prims.h"
#include "stdarg.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*  Make the base types  */

sval* make_cell() {
    return malloc(sizeof(sval));
}

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
    if (car->tag == ERROR) return car;
    if (cdr->tag == ERROR) return cdr;
    sval *v = make_cell();
    v->tag = PAIR;
    v->body.list.car = car;
    v->body.list.cdr = cdr;
    return v;
}

sval* make_env(sval* env) {
    // Caller is expected to mutate to set frame
    if (env->tag == ERROR) return env;
    if (env->tag != ENV) return 0;
    sval *v = make_cell();
    v->tag = ENV;
    v->body.env.parent = env;
    v->body.env.frame.names = make_empty();
    v->body.env.frame.values = make_empty();
    return v;
}

sval* make_int(int i) {
    sval *v = make_cell();
    v->tag = NUMBER;
    v->body.smallnum = i;
    return v;
}

sval* make_symbol(char* name) {
    sval *v = make_cell();
    v->tag = SYMBOL;
    v->body.symbol = strdup(name);
    return v;
}
sval* make_string(char* str) {
    sval *v = make_cell();
    v->tag = STRING;
    v->body.symbol = strdup(str);
    return v;
}

sval* make_prim(sval* (*primitive)(sval*)) {
    sval *v = make_cell();
    v->tag = PRIMITIVE;
    v->body.primitive = primitive;
    return v;
}

sexp* make_function(sexp *parameters, sexp *body, sval *env) {
    sval *v = make_cell();
    if (env->tag == ERROR) return env;
    if (env->tag != ENV) return 0;

    v->tag = FUNCTION;
    v->body.closure.parameters = parameters;
    v->body.closure.body = body;
    v->body.closure.env = env;
    return v;
}

sexp* make_macro(sexp *function) {
    sval *v = make_cell();
    v->tag = MACRO;
    v->body.macro_procedure = function;
    return v;
}

sval* make_character_constant (char c) { 
    if (((unsigned int)c) >= 128) return 0;
    return &CHARS_V[(int)c];
};

sval* make_empty() { return EMPTY_LIST; }
