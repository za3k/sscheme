#include "types.h"
#include "prims.h"
#include "errors.h"
#include "constants.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*  Make the base types  */

sval* make_cell() {
    return malloc(sizeof(sval));
}

sval* error(char *msg) {
    sval *err = make_cell();
    err->tag = ERROR;
    err->body.error = msg;
    //printf("%s\n", msg);
    //msg[1000000]=0; // Generate a crash
    return err;
}

sval* make_cons(sval *car, sval *cdr) {
    if (car->tag == ERROR) return car;
    if (cdr->tag == ERROR) return cdr;
    sval *v = make_cell();
    v->tag = CONS;
    v->body.list.car = car;
    v->body.list.cdr = cdr;
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
    int l = strlen(name);
    char *s = malloc(l+1);
    memcpy(s, name, l+1);
    v->tag = SYMBOL;
    v->body.symbol = s;
    return v;
}

sval* make_prim(sval* (*primitive)(sval*)) {
    sval *v = make_cell();
    v->tag = PRIMITIVE;
    v->body.primitive = primitive;
    return v;
}

sexp* make_function(sexp *parameters, sexp *body, struct senv *env) {
    sval *v = make_cell();

    v->tag = FUNCTION;
    v->body.closure.parameters = parameters;
    v->body.closure.body = body;
    v->body.closure.env = env;
    return v;
}

sval* make_character_constant (char c) { 
    if (((unsigned int)c) >= 128) return 0;
    return &CHARS_V[(int)c];
};

sval* make_true()  { return &TRUE_V; }
sval* make_false() { return &FALSE_V; }
sval* make_nil()   { return &NIL_V; }
sval* make_empty() { return &EMPTY_V; }

sexp* make_cond()  { return &COND_V; }
sexp* make_quote() { return &QUOTE_V; }
sexp* make_lambda(){ return &LAMBDA_V; }
sexp* make_define(){ return &DEFINE_V; }
