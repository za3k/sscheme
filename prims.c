#include "prims.h"
#include "errors.h"
#include "helpers.h"
#include "constants.h"
#include <stdio.h>

// Type and arity checker macros
#define FARITY(x, args) if (!islistoflength(args, x)) return error(ERR_WRONG_NUM);
#define TYPE(p, arg) if (!arg) { return error(ERR_NULL_PTR); } else if (arg->tag == ERROR) { return arg; } else if (!p(arg)) { return error(ERR_WRONG_TYPE); }
#define VARITY(args) if (!ispair(args) && !isempty(args)) return error(ERR_NON_LIST);
#define VARITY_TYPE(p, args) if (!listOf(args, p)) return error(ERR_WRONG_TYPE);

// Helpers
int listOf(sval *arg, int p(sval *arg)) {
    if (isempty(arg)) return 1;
    if (!ispair(arg)) return 0;
    if (!p(car(arg))) return 0;
    return listOf(cdr(arg), p);
}

inline static sval* pred(int x) {
    return x ? TRUE : FALSE;
}

/*  ============ Arity checkers and uniform function signatures ============= */

sval* prim_apply(sval *args)      { FARITY(2, args); return apply(car(args), car(cdr(args))); }
sval* prim_add(sval *args)        { FARITY(2, args); return add(car(args), car(cdr(args))); }
sval* prim_cons(sval *args)       { FARITY(2, args); return cons(car(args), car(cdr(args))); }
sval* prim_car(sval *args)        { FARITY(1, args); return car(car(args)); }
sval* prim_charp(sval *args)      { FARITY(1, args); return charp(car(args)); }
sval* prim_cdr(sval *args)        { FARITY(1, args); return cdr(car(args)); }
sval* prim_divide(sval *args)     { FARITY(2, args); return divide(car(args), car(cdr(args))); }
sval* prim_nilp(sval *args)       { FARITY(1, args); return nilp(car(args)); }
sval* prim_emptyp(sval *args)     { FARITY(1, args); return emptyp(car(args)); }
sval* prim_eqp(sval *args)        { FARITY(2, args); return eqp(car(args), car(cdr(args))); }
sval* prim_error(sval *args)      { FARITY(1, args); return error_prim(car(args)); }
sval* prim_pairp(sval *args)      { FARITY(1, args); return pairp(car(args)); }
sval* prim_lt(sval *args)         { FARITY(2, args); return lt(car(args), car(cdr(args))); }
sval* prim_multiply(sval *args)   { FARITY(2, args); return multiply(car(args), car(cdr(args))); }
sval* prim_numberp(sval *args)    { FARITY(1, args); return numberp(car(args)); }
sval* prim_print(sval *args)      { FARITY(1, args); return print1(car(args)); }
sval* prim_procedurep(sval *args) { FARITY(1, args); return procedurep(car(args)); }
sval* prim_stringp(sval *args)    { FARITY(1, args); return stringp(car(args)); }
sval* prim_subtract(sval *args)   { FARITY(2, args); return subtract(car(args), car(cdr(args))); }

/*  ============ Definitions of primitives ============= */

sval* charp(sval *arg1)           { return pred(ischar(arg1)); }
sval* emptyp(sval *arg1)          { return pred(isempty(arg1)); }
sval* eqp(sval *arg1, sval *arg2) { return pred(iseq(arg1, arg2)); }
sval* pairp(sval *arg1)           { return pred(ispair(arg1)); }
sval* nilp(sval *arg1)            { return pred(isnil(arg1)); }
sval* numberp(sval *arg1)         { return pred(isnumber(arg1)); }
sval* procedurep(sval *arg1)      { return pred(arg1->tag == FUNCTION || arg1->tag == PRIMITIVE); }
sval* stringp(sval *arg1)         { return pred(isstring(arg1)); }

sval* add(sval *arg1, sval *arg2) {
    TYPE(isnumber, arg1);
    TYPE(isnumber, arg2);

    return make_int(arg1->body.smallnum+arg2->body.smallnum);
}

sval* car(sval *arg1) {
    TYPE(ispair, arg1);

    return arg1->body.list.car;
}

sval* cdr(sval *arg1) {
    TYPE(ispair, arg1);

    return arg1->body.list.cdr;
}

sval* cons(sval *arg1, sval *arg2) {
    if (arg1->tag == ERROR) return arg1;
    if (arg2->tag == ERROR) return arg2;
    return make_cons(arg1, arg2);
}

sval* divide(sval *arg1, sval *arg2) {
    TYPE(isnumber, arg1);
    TYPE(isnumber, arg2);

    if (arg2->body.smallnum == 0) return error(ERR_DIV_BY_ZERO);
    return make_int(arg1->body.smallnum/arg2->body.smallnum);
}


sval* error_prim(sval *arg1) {
    TYPE(isstring, arg1);

    return error(arg1->body.symbol);
}

sval* lt(sval *arg1, sval *arg2) {
    TYPE(isnumber, arg1);
    TYPE(isnumber, arg1);

    return pred(arg1->body.smallnum < arg2->body.smallnum);
}

sval* multiply(sval *arg1, sval *arg2) {
    TYPE(isnumber, arg1);
    TYPE(isnumber, arg2);

    return make_int(arg1->body.smallnum*arg2->body.smallnum);
}

sval* negative(sval *arg1) {
    TYPE(isnumber, arg1);

    return make_int(-arg1->body.smallnum);
}

sval* subtract(sval *arg1, sval *arg2) {
    TYPE(isnumber, arg1);
    TYPE(isnumber, arg2);

    return make_int(arg1->body.smallnum-arg2->body.smallnum);
}

static char print_buffer[1000];
sval* print1(sval *arg) {
    snprint1(print_buffer, sizeof(print_buffer), arg);
    printf("%s",print_buffer);
    return NIL;
}

sval* print1nl(sval *arg) {
    snprint1(print_buffer, sizeof(print_buffer), arg);
    printf("%s\n",print_buffer);
    return NIL;
}


sval* (*primitives[])(sval *args) = {
    prim_multiply,
    prim_lt,
    prim_add,
    prim_subtract,
    prim_apply,
    prim_car,
    prim_cdr,
    prim_charp,
    prim_cons,
    prim_print,
    prim_error,
    prim_eqp,
    prim_nilp,
    prim_emptyp,
    prim_numberp,
    prim_pairp,
    prim_procedurep,
    prim_divide,
    prim_stringp,
    0,
};
char* primitive_names[] = {
    "*",
    "<",
    "+",
    "-",
    "apply",
    "car",
    "cdr",
    "char?",
    "cons",
    "display",
    "error",
    "eq?",
    "nil?",
    "null?",
    "number?",
    "pair?",
    "procedure?",
    "quotient",
    "string?",
};
