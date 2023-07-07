#include "prims.h"

#include "config.h"
#include "constants.h"
#include "errors.h"
#include "helpers.h"
#include <stdio.h>

// Type and arity checker macros
#define FARITY(f, x, args) if (!islistoflength(args, x)) return error(ERR_WRONG_NUM, f);
#define TYPE(f, p, arg) if (!arg) { return error(ERR_NULL_PTR, f); } else if (iserror(arg)) { return arg; } else if (!p(arg)) { return error(ERR_WRONG_TYPE, f); }

// Helpers
int listOf(sval *arg, int p(sval *arg)) {
    while (ispair(arg)) {
        if (!p(car(arg))) return 0;
        arg = cdr(arg);
    }
    return isempty(arg);
}

inline static sval* pred(int x) {
    return x ? TRUE : FALSE;
}

/*  ============ Arity checkers and uniform function signatures ============= */

sval* prim_add(sval *args)           { FARITY(__func__, 2, args); return add(car(args), car(cdr(args))); }
sval* prim_append(sval *args)        { FARITY(__func__, 2, args); return append(car(args), car(cdr(args))); }
sval* prim_apply(sval *args)         { FARITY(__func__, 2, args); return apply(car(args), car(cdr(args))); }
sval* prim_car(sval *args)           { FARITY(__func__, 1, args); return car(car(args)); }
sval* prim_cdr(sval *args)           { FARITY(__func__, 1, args); return cdr(car(args)); }
sval* prim_char2integer(sval *args)  { FARITY(__func__, 1, args); return char2integer(car(args)); }
sval* prim_charp(sval *args)         { FARITY(__func__, 1, args); return charp(car(args)); }
sval* prim_cons(sval *args)          { FARITY(__func__, 2, args); return cons(car(args), car(cdr(args))); }
sval* prim_divide(sval *args)        { FARITY(__func__, 2, args); return divide(car(args), car(cdr(args))); }
sval* prim_emptyp(sval *args)        { FARITY(__func__, 1, args); return emptyp(car(args)); }
sval* prim_eqvp(sval *args)          { FARITY(__func__, 2, args); return eqvp(car(args), car(cdr(args))); }
sval* prim_error(sval *args)         { FARITY(__func__, 1, args); return error_prim(car(args)); }
sval* prim_integer2char(sval *args)  { FARITY(__func__, 1, args); return integer2char(car(args)); }
sval* prim_list2string(sval *args)   { FARITY(__func__, 1, args); return list2string(car(args)); }
sval* prim_lt(sval *args)            { FARITY(__func__, 2, args); return lt(car(args), car(cdr(args))); }
sval* prim_multiply(sval *args)      { FARITY(__func__, 2, args); return multiply(car(args), car(cdr(args))); }
sval* prim_nilp(sval *args)          { FARITY(__func__, 1, args); return nilp(car(args)); }
sval* prim_numberp(sval *args)       { FARITY(__func__, 1, args); return numberp(car(args)); }
sval* prim_pairp(sval *args)         { FARITY(__func__, 1, args); return pairp(car(args)); }
sval* prim_print(sval *args)         { FARITY(__func__, 1, args); return print1(car(args)); }
sval* prim_procedurep(sval *args)    { FARITY(__func__, 1, args); return procedurep(car(args)); }
sval* prim_setcar(sval *args)        { FARITY(__func__, 2, args); return setcar(car(args), car(cdr(args))); }
sval* prim_setcdr(sval *args)        { FARITY(__func__, 2, args); return setcdr(car(args), car(cdr(args))); }
sval* prim_string2list(sval *args)   { FARITY(__func__, 1, args); return string2list(car(args)); }
sval* prim_string2symbol(sval *args) { FARITY(__func__, 1, args); return string2symbol(car(args)); }
sval* prim_stringp(sval *args)       { FARITY(__func__, 1, args); return stringp(car(args)); }
sval* prim_subtract(sval *args)      { FARITY(__func__, 2, args); return subtract(car(args), car(cdr(args))); }
sval* prim_symbol2string(sval *args) { FARITY(__func__, 1, args); return symbol2string(car(args)); }
sval* prim_symbolp(sval *args)       { FARITY(__func__, 1, args); return symbolp(car(args)); }

/*  ============ Definitions of primitives ============= */

sval* charp(sval *arg1)           { return pred(ischar(arg1)); }
sval* emptyp(sval *arg1)          { return pred(isempty(arg1)); }
sval* eqvp(sval *arg1, sval *arg2){ return pred(iseqv(arg1, arg2)); }
sval* pairp(sval *arg1)           { return pred(ispair(arg1)); }
sval* nilp(sval *arg1)            { return pred(isnil(arg1)); }
sval* numberp(sval *arg1)         { return pred(isnumber(arg1)); }
sval* procedurep(sval *arg1)      { return pred(isprocedure(arg1)); }
sval* stringp(sval *arg1)         { return pred(isstring(arg1)); }
sval* symbolp(sval *arg1)         { return pred(issymbol(arg1)); }

sval* add(sval *arg1, sval *arg2) {
    TYPE(__func__, isnumber, arg1);
    TYPE(__func__, isnumber, arg2);

    return make_int(arg1->body.smallnum+arg2->body.smallnum);
}

sval* car(sval *arg1) {
    TYPE(__func__, ispair, arg1);

    return _car(arg1);
}

sval* cdr(sval *arg1) {
    TYPE(__func__, ispair, arg1);

    return _cdr(arg1);
}

sval* cons(sval *arg1, sval *arg2) {
    if (iserror(arg1)) return arg1;
    if (iserror(arg2)) return arg2;
    return make_cons(arg1, arg2);
}

sval* divide(sval *arg1, sval *arg2) {
    TYPE(__func__, isnumber, arg1);
    TYPE(__func__, isnumber, arg2);

    if (arg2->body.smallnum == 0) return error(ERR_DIV_BY_ZERO);
    return make_int(arg1->body.smallnum/arg2->body.smallnum);
}


sval* error_prim(sval *arg1) {
    TYPE(__func__, isstring, arg1);

    return error(arg1->body.symbol);
}

sval* lt(sval *arg1, sval *arg2) {
    TYPE(__func__, isnumber, arg1);
    TYPE(__func__, isnumber, arg1);

    return pred(arg1->body.smallnum < arg2->body.smallnum);
}

sval* multiply(sval *arg1, sval *arg2) {
    TYPE(__func__, isnumber, arg1);
    TYPE(__func__, isnumber, arg2);

    return make_int(arg1->body.smallnum*arg2->body.smallnum);
}

sval* negative(sval *arg1) {
    TYPE(__func__, isnumber, arg1);

    return make_int(-arg1->body.smallnum);
}

sval* subtract(sval *arg1, sval *arg2) {
    TYPE(__func__, isnumber, arg1);
    TYPE(__func__, isnumber, arg2);

    return make_int(arg1->body.smallnum-arg2->body.smallnum);
}

static char print_buffer[MAX_PRINT_SIZE];
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

sval* char2integer(sval *arg1) {
    TYPE(__func__, ischar, arg1);
    return make_int(arg1-CHARS);
}

sval* integer2char(sval *arg1) {
    TYPE(__func__, isnumber, arg1);
    int i = arg1->body.smallnum;
    if (0 <= i && i < 128) return &CHARS[i];
    else return error(ERR_CHAR_OUT_OF_RANGE);
}

char LTS_BUFFER[MAX_STRING_SIZE+1];
sval* list2string(sval *arg1) {
    int i=0;
    if (!arg1) return error(ERR_NULL_PTR, __func__);
    else if (iserror(arg1)) return arg1;

    for (i=0; i<=MAX_STRING_SIZE && ispair(arg1); i++, arg1=cdr(arg1)) {
        TYPE(__func__, ischar, car(arg1));
        LTS_BUFFER[i] = car(arg1)-CHARS;
    }
    if (i>=MAX_STRING_SIZE) return error(ERR_STRING_TOO_BIG);
    if (!isempty(arg1)) return error(ERR_WRONG_TYPE, __func__);
    LTS_BUFFER[i]=0;
    return make_string(LTS_BUFFER);
}

sval* string2list(sval *arg1) {
    TYPE(__func__, isstring, arg1);

    char *string = arg1->body.symbol;
    int l=strlen(string);
    sval *ret = EMPTY_LIST;
    for (int i=l-1; i>=0&&!iserror(ret); i--) ret = make_cons(&CHARS[(int) string[i]], ret);
    return ret;
}

sval* string2symbol(sval *arg1) {
    TYPE(__func__, isstring, arg1)

    return make_symbol(arg1->body.symbol);
}

sval* symbol2string(sval *arg1) {
    TYPE(__func__, issymbol, arg1)

    return make_string(arg1->body.symbol);
}

sval* append(sexp *arg1, sexp *arg2) {
    if (iserror(arg1)) return arg1;
    if (iserror(arg2)) return arg2;
    if (!isempty(arg1)&&!ispair(arg1)) return error(ERR_WRONG_TYPE, __func__);
    if (!isempty(arg2)&&!ispair(arg2)) return error(ERR_WRONG_TYPE, __func__);
    sval *rarg1 = EMPTY_LIST;
    sval *ret = arg2;
    while (ispair(arg1)) {
        rarg1 = make_cons(car(arg1), rarg1);
        arg1 = cdr(arg1);
    }
    if (iserror(rarg1)) return rarg1;
    TYPE(__func__, isempty, arg1)
    while (!isempty(rarg1)&&!iserror(ret)) {
        ret = make_cons(car(rarg1), ret);
        rarg1 = cdr(rarg1);
    }
    return ret;
}

sval* setcar(sexp *arg1, sexp *arg2) {
    TYPE(__func__, ispair, arg1)
    if (iserror(arg2)) return arg2;
    _setcar(arg1, arg2);
    return NIL;
}
sval* setcdr(sexp *arg1, sexp *arg2) {
    TYPE(__func__, ispair, arg1)
    if (iserror(arg2)) return arg2;
    _setcdr(arg1, arg2);
    return NIL;
}

int lookup_primitive(sval* (*primitive)(sval *args)) {
    int i;
    sval*(**p)(sval*);

    for (i=0, p = primitives; p!=0; i++,p++) {
        if (*p==primitive) return i;
    }
    return i;
}

sval* (*primitives[])(sval *args) = {
    prim_multiply,
    prim_lt,
    prim_add,
    prim_subtract,
    prim_append,
    prim_apply,
    prim_car,
    prim_cdr,
    prim_charp,
    prim_char2integer,
    prim_cons,
    prim_print,
    prim_error,
    prim_eqvp,
    prim_integer2char,
    prim_list2string,
    prim_nilp,
    prim_emptyp,
    prim_numberp,
    prim_pairp,
    prim_procedurep,
    prim_divide,
    prim_stringp,
    prim_setcar,
    prim_setcdr,
    prim_string2list,
    prim_string2symbol,
    prim_symbol2string,
    prim_symbolp,
    0,
};
char* primitive_names[] = {
    "*",
    "<",
    "+",
    "-",
    "append",
    "apply",
    "car",
    "cdr",
    "char?",
    "char->integer",
    "cons",
    "display",
    "error",
    "eqv?",
    "integer->char",
    "list->string",
    "nil?",
    "null?",
    "number?",
    "pair?",
    "procedure?",
    "quotient",
    "string?",
    "set-car!",
    "set-cdr!",
    "string->list",
    "string->symbol",
    "symbol->string",
    "symbol?",
};
