#include "types.h"

// TODO: add definitions in scheme

// Primitive functions -- uniform signature
extern sval* (*primitives[])(sval *args);
extern char* primitive_names[];

// Primitive functions -- core
sval* cons(sval *arg1, sval *arg2);
sval* car(sval *arg1);
sval* cdr(sval *arg1);
sval* add(sval *args);
sval* negative(sval *arg1);
sval* subtract(sval *arg1, sval *arg2);
sval* nilp(sval *arg1);
sval* listp(sval *arg1);
sval* numberp(sval *arg1);
sval* emptyp(sval *arg1);
sval* procedurep(sval *arg1);
sval* print(sval *args);
sval* eqp(sval *arg1, sval *arg2);
