#include "types.h"

// TODO: eq? < * / ~ ^ | eval
// Could be done in scheme with macros: if and or not
// Can be done in scheme: equal? = != > <= >=

// Primitive functions -- uniform signature
sval* prim_cons(sval *args);
sval* prim_car(sval *args);
sval* prim_cdr(sval *args);
sval* prim_plus(sval *args);
sval* prim_minus(sval *args);
sval* prim_nilp(sval *args);
sval* prim_falsep(sval *args);
sval* prim_truep(sval *args);
sval* prim_listp(sval *args);
sval* prim_numberp(sval *args);
sval* prim_emptyp(sval *args);
sval* prim_list(sval *args);
sval* prim_print(sval *args);
sval* prim_else(sval *args);

// Primitive functions -- core
sval* cons(sval *arg1, sval *arg2);
sval* car(sval *arg1);
sval* cdr(sval *arg1);
sval* add(sval *args);
sval* negative(sval *arg1);
sval* subtract(sval *arg1, sval *arg2);
sval* nilp(sval *arg1);
sval* falsep(sval *arg1);
sval* truep(sval *arg1);
sval* listp(sval *arg1);
sval* numberp(sval *arg1);
sval* emptyp(sval *arg1);
sval* print(sval *args);
sval* else_(sval *args);
