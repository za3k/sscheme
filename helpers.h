#include "types.h"

#ifndef __HELPERS
#define __HELPERS
// car, cdr, and cons are listed under 'prims', not here

int iserror(sval *arg);
int isempty(sval *arg);
int isnil(sval *arg);
int isfalse(sval *arg);
int istrue(sval *arg);
int islist(sval *arg);
int isnumber(sval *arg);
int issymbol(sval *arg);

void print1(sval *arg1);
void print1nl(sval *arg1);

#endif //__HELPERS
