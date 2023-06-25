#include "types.h"

#ifndef CONSTANTSH
#define CONSTANTSH

extern char *char_constant_names[140];
extern char char_constant_values[140];

// Singleton constants
extern struct sval CHARS_V[256]; // Initialized after we start, sneaky sneaky.

extern struct sval TRUE_V;
extern struct sval FALSE_V;
extern struct sval EMPTY_V;
extern struct sval NIL_V;
extern sexp LAMBDA_V;
extern sexp COND_V;
extern sexp QUOTE_V;
extern struct senv BASE_ENV;

#endif // CONSTANTSH
