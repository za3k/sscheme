#ifndef __ALLOC
#define __ALLOC

#include "types.h"

extern long cells_used;
extern long cells_free;

sval* make_cell();
sval* gc(sval* root);
void gc_force(sval* root);

extern sval* STACK;
void init_stack();
void push(sval* a);
sval* pop();

#endif //__ALLOC
