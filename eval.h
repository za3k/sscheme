#include "types.h"

// TODO: Move env* to a sval*

// The core!
sval* eval1(sexp *expression, struct senv *env);
sval* eval_all(sexp *expressions, struct senv *env);
sval* apply(sval *proc, sval *args);

struct senv* empty_env();
sval* define(struct senv *env, sval* symbol, sval* val);
