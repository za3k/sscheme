#include "types.h"

#ifndef CONSTANTSH
#define CONSTANTSH

extern char *char_constant_names[140];
extern char char_constant_values[140];

// Singleton constants
extern struct sval CHARS_V[128];

extern struct sval* TRUE;
extern struct sval* FALSE;
extern struct sval* EMPTY_LIST;
extern struct sval* NIL;
extern sexp* COND;
extern sexp* DEFINE;
extern sexp* DEFINE_MACRO;
extern sexp* LAMBDA;
extern sexp *UNQUOTE;
extern sexp *UNQUOTE_SPLICING;
extern sexp *QUASIQUOTE;
extern sexp* QUOTE;
extern sexp* SET;
extern sval* BUILTINS_ENV;
extern sval* STANDARD_ENV;
extern sval* OUT_OF_MEMORY;

#endif // CONSTANTSH
