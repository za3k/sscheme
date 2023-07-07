#include "types.h"

// The core!
sval* eval1(sexp *expression, sval *env);
sval* eval_all(sexp *expressions, sval *env);
sval* apply(sval *proc, sval *args);

void init_constants();
sval* empty_env();
sval* define(sval *env, sval* symbol, sval* val);
sval* set(sval *env, sval *symbol, sval* val);
