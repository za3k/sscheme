#include "types.h"
#include <string.h>

#ifndef __HELPERS
#define __HELPERS
// car, cdr, and cons are listed under 'prims', not here

int iserror(sval *arg);
int isempty(sval *arg);
int isnil(sval *arg);
int isfalse(sval *arg);
int istrue(sval *arg);
int ispair(sval *arg);
int isnumber(sval *arg);
int issymbol(sval *arg);
int isenv(sval *arg);
int symboleq(sval *arg1, sval *arg2);
int iseq(sval *arg1, sval *arg2);

int islistoflength(sval *arg, int l);

// Print is repr-style
void print1(sval *arg1);
void print1nl(sval *arg1);
int snprint1(char* buffer, size_t n, sval *arg);

char* slurp_stdin(char *buffer);

#endif //__HELPERS
