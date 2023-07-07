#include "types.h"
#include <string.h>

#ifndef __HELPERS
#define __HELPERS

int ischar(sval *arg);
int isconstant(sval *arg);
int iserror(sval *arg);
int isempty(sval *arg);
int isnil(sval *arg);
int isfalse(sval *arg);
int isform(sval *arg);
int isfunction(sval *arg);
int ispair(sval *arg);
int isprimitive(sval *arg);
int isprocedure(sval *arg);
int ismacro(sval *arg);
int isnumber(sval *arg);
int issymbol(sval *arg);
int isstring(sval *arg);
int isenv(sval *arg);
int symboleq(sval *arg1, sval *arg2);
int iseqv(sval *arg1, sval *arg2);

int listlength(sval *arg);
int islistoflength(sval *arg, int l);
sexp* reverse(sval *arg);

// Print is repr-style
int snprint1(char* buffer, size_t n, sval *arg);

char* slurp_stdin(char *buffer);

#endif //__HELPERS
