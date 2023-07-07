#include "types.h"

#ifndef CONSTANTSH
#define CONSTANTSH

extern char *char_constant_names[140];
extern char char_constant_values[140];

// Singleton constants
extern sval* CHARS; // Length 128 array

extern sval* TRUE;
extern sval* FALSE;
extern sval* EMPTY_LIST;
extern sval* NIL;
extern sval* COND;
extern sval* DEFINE;
extern sval* DEFINE_MACRO;
extern sval* LAMBDA;
extern sval *UNQUOTE;
extern sval *UNQUOTE_SPLICING;
extern sval *QUASIQUOTE;
extern sval* QUOTE;
extern sval* SET;
extern sval* BUILTINS_ENV;
extern sval* STANDARD_ENV;
extern sval* OUT_OF_MEMORY;

void init_constants();

#endif // CONSTANTSH
