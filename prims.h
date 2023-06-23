#include "types.h"

// TODO: * / < ~ ^ | apply display error eq? eval procedure? quotient random
// TODO: Add macros to define the below?
// TODO special forms: define let and or do

// TODO: add definitions in scheme
// Can be done in scheme: <= = > >= abs append boolean? cadr,caar... empty? eof-object? equal? even? expt filter first,second,rest,last,third... for-each length map max member? min newline not null? odd? repeated square write
// Could be added if we add floats: expt cos sin tan sqrt log

// Primitive functions -- uniform signature
sval* prim_car(sval *args);
sval* prim_cdr(sval *args);
sval* prim_cons(sval *args);

sval* prim_plus(sval *args);
sval* prim_minus(sval *args);

sval* prim_nilp(sval *args);
sval* prim_listp(sval *args);
sval* prim_numberp(sval *args);
sval* prim_emptyp(sval *args);
sval* prim_procedurep(sval *args);

sval* prim_eqp(sval *args);
sval* prim_list(sval *args);
sval* prim_print(sval *args);

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
