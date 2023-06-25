#include "prims.h"
#include "errors.h"
#include "helpers.h"
#include "constants.h"
#include <stdio.h>

// Type and arity checker macros
#define FARITY(x, args) if (!islistoflength(args, x)) return error(ERR_WRONG_NUM);
#define TYPE(p, arg) if (!p(arg)) return error(ERR_WRONG_TYPE);
#define VARITY(args) if (!islist(args)) return error(ERR_NON_LIST);
#define VARITY_TYPE(p, args) if (!listOf(args, p)) return error(ERR_WRONG_TYPE);

// Helpers
int listOf(sval *arg, int p(sval *arg)) {
    if (!islist(arg)) return 0;
    if (isempty(arg)) return 1;
    if (!p(car(arg))) return 0;
    return listOf(cdr(arg), p);
}

inline static sval* pred(int x) {
    return x ? &TRUE_V : &FALSE_V;
}

/*  ============ Arity checkers and uniform function signatures ============= */

sval* prim_cons(sval *args)    { FARITY(2, args); return cons(car(args), car(cdr(args))); }
sval* prim_car(sval *args)     { FARITY(1, args); return car(car(args)); }
sval* prim_cdr(sval *args)     { FARITY(1, args); return cdr(car(args)); }
sval* prim_nilp(sval *args)    { FARITY(1, args); return nilp(car(args)); }
sval* prim_emptyp(sval *args)  { FARITY(1, args); return emptyp(car(args)); }
sval* prim_listp(sval *args)   { FARITY(1, args); return listp(car(args)); }
sval* prim_numberp(sval *args) { FARITY(1, args); return numberp(car(args)); }
sval* prim_procedurep(sval *args) { FARITY(1, args); return procedurep(car(args)); }
sval* prim_eqp(sval *args)     { FARITY(2, args); return eqp(car(args), car(cdr(args))); }

sval* prim_list(sval *args)    { VARITY(args);    return args; }

sval* prim_plus(sval *args) {
    VARITY(args);
    if (islistoflength(args, 0)) return error(ERR_WRONG_NUM);
    return add(args);
}

sval* prim_minus(sval *args) {
    if (islistoflength(args, 1)) return negative(car(args));
    if (islistoflength(args, 2)) return subtract(car(args), car(cdr(args)));
    else return error(ERR_WRONG_NUM);
}

sval* prim_print(sval *args) {
    VARITY(args);
    return print(args);
}

/*  ============ Definitions of primitives ============= */

sval* nilp(sval *arg1) { return pred(isnil(arg1)); }
sval* listp(sval *arg1) { return pred(islist(arg1)); }
sval* numberp(sval *arg1) { return pred(isnumber(arg1)); }
sval* emptyp(sval *arg1) { return pred(isempty(arg1)); }
sval* eqp(sval *arg1, sval *arg2) { return pred(iseq(arg1, arg2)); }
sval* procedurep(sval *arg1) { return pred(arg1->tag == FUNCTION || arg1->tag == PRIMITIVE); }

sval* cons(sval *arg1, sval *arg2) {
    // Should we allow cons-ing to make non-lists? currently yes!
    return make_cons(arg1, arg2);
}

sval* car(sval *arg1) {
    TYPE(islist, arg1);

    if (isempty(arg1)) return error(ERR_EMPTY_LIST);
    else return arg1->body.list.car;
}

sval* cdr(sval *arg1) {
    TYPE(islist, arg1);

    if (isempty(arg1)) return error(ERR_EMPTY_LIST);
    else return arg1->body.list.cdr;
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

sval* add(sval *args) {
    VARITY_TYPE(isnumber, args);
    int sum = 0;
    while (!isempty(args)) {
        sum += car(args)->body.smallnum;
        args = cdr(args);
    }
    return make_int(sum);
}

sval* print(sval *args) {
    while (!isempty(args)) {
        print1nl(car(args));
        args = cdr(args);
    }
    return &NIL_V;
}
