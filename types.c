#include "types.h"
#include "prims.h"
#include "errors.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
// TODO: Move prims to function pointers, not an enum

// Constant singletons
static struct sval TRUE_V = { CONSTANT, .body.constant = TRUE };
static struct sval FALSE_V = { CONSTANT, .body.constant = FALSE };
static struct sval EMPTY_V = { CONSTANT, .body.constant = EMPTY_LIST };
static struct sval NIL_V = { CONSTANT, .body.constant = NIL };

/*  Make the base types  */

sval* make_cell() {
    return malloc(sizeof(sval));
}

sval* error(char *msg) {
    sval *err = make_cell();
    err->tag = ERROR;
    err->body.error = msg;
    printf("%s\n", msg);
    msg[1000000]=0; // Generate a crash
    return err;
}

sval* make_cons(sval *car, sval *cdr) {
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
    // TODO: Check for special symbols: cond, if, quote
    sval *v = make_cell();
    int l = strlen(name);
    char *s = malloc(l+1);
    memcpy(s, name, l+1);
    v->tag = SYMBOL;
    v->body.symbol = s;
    return v;
}

sval* make_prim(int prim) {
    sval *v = make_cell();
    v->tag = BUILTIN_PROCEDURE;
    v->body.builtin_procedure = prim;
    return v;
}

sval* make_true()  { return &TRUE_V; }
sval* make_false() { return &FALSE_V; }
sval* make_nil()   { return &NIL_V; }
sval* make_empty() { return &EMPTY_V; }
