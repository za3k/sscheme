#include "types.h"

// TODO: Add FFI so these can be defined in scheme, too.

// Primitive functions -- uniform signature
extern sval* (*primitives[])(sval *args);
extern char* primitive_names[];

// Primitive functions -- core
sval* cons(sval *arg1, sval *arg2);
sval* car(sval *arg1);
sval* cdr(sval *arg1);
sval* add(sval *arg1, sval *arg2);
sval* divide(sval *arg1, sval *arg2);
sval* multiply(sval *arg1, sval *arg2);
sval* subtract(sval *arg1, sval *arg2);
sval* nilp(sval *arg1);
sval* listp(sval *arg1);
sval* lt(sval *arg1, sval *arg2);
sval* numberp(sval *arg1);
sval* emptyp(sval *arg1);
sval* procedurep(sval *arg1);
sval* error_prim(sval *arg1);
sval* eqp(sval *arg1, sval *arg2);
