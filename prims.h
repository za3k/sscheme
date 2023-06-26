#include "types.h"

// TODO: Add FFI so these can be defined in scheme, too.

// Primitive functions -- uniform signature
extern sval* (*primitives[])(sval *args);
extern char* primitive_names[];

// Primitive functions -- core
sval* add(sval *arg1, sval *arg2);
sval* apply(sval *arg1, sval *arg2);
sval* cons(sval *arg1, sval *arg2);
sval* car(sval *arg1);
sval* charp(sval *arg1);
sval* cdr(sval *arg1);
sval* divide(sval *arg1, sval *arg2);
sval* emptyp(sval *arg1);
sval* eqp(sval *arg1, sval *arg2);
sval* error_prim(sval *arg1);
sval* lt(sval *arg1, sval *arg2);
sval* multiply(sval *arg1, sval *arg2);
sval* nilp(sval *arg1);
sval* numberp(sval *arg1);
sval* pairp(sval *arg1);
sval* print1(sval *arg1);
sval* print1nl(sval *arg1);
sval* procedurep(sval *arg1);
sval* stringp(sval *arg1);
sval* subtract(sval *arg1, sval *arg2);
