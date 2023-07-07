#ifndef __ALLOC
#define __ALLOC

#include "types.h"

extern long cells_used;
extern long cells_free;

sval* make_cell();
int isallocated(sval* v);
sval* gc(sval* root);
void gc_force(sval* root);

extern sval* STACK;
void init_stack();
void push(sval* a);
sval* pop();

extern long HEAP_END;
extern sval HEAP[];
extern long CONSTANTS_END;

#endif //__ALLOC
