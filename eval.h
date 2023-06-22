#include "types.h"

// The core!
sval* eval(sexp *expression, struct senv *env);
sval* apply(sval *proc, sval *args);
struct senv* empty_env();

int main(int argc, char *argv[]);
