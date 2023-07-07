#include "allocator.h"

#include "config.h"
#include "constants.h"
#include "errors.h"
#include "helpers.h"
#include "prims.h"
#include "types.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

long cells_used = 0;
long cells_free = MAX_CELLS;
long cells_freed = 0;
sval HEAP[MAX_CELLS];

long HEAP_END = 0;
sval* FREE_LIST = 0;
sval* STACK = 0;

void free_cell(sval* cell);
int isallocated(sval* v);
int ismarked(sval *v);
void mark(sval *v, int set);
int isinuse(sval *v);
void inuse(sval *v, int set);

// ----------- Heap allocation ----------
int isallocated(sval* v) { return v->tag != UNALLOCATED; }
int ismarked(sval *v) { return v->marked; }
void mark(sval *v, int set) { v->marked = set; }
int isinuse(sval *v) { return v->in_use; }
void inuse(sval *v, int set) { v->in_use = set; }
sval* make_cell() {
    if (--cells_free <= 0) {
        if (cells_free < 0) {
            // Oops! error didn't propogate
            printf(ERR_OUT_OF_MEMORY_TWICE);
            exit(5);
        }
        return OUT_OF_MEMORY;
    }
    cells_used++;
    if (FREE_LIST) {
        // Use a previously freed cell
        cells_freed--;
        sval *ret = FREE_LIST;
        FREE_LIST = _cdr(ret);
        _setcdr(ret, 0);
        return ret;
    } else {
        // Allocate a new cell
        return &HEAP[HEAP_END++];
    }
}

void free_cell(sval* cell) {
    if (cell==0) return;
    if (issymbol(cell) || isstring(cell)) {
        free(cell->body.symbol);
    } else if (iserror(cell)) {
        free(cell->body.error);
    }
    cell->tag = UNALLOCATED;
    _setcar(cell, 0);
    _setcdr(cell, FREE_LIST);
    FREE_LIST = cell;

    cells_used--;
    cells_freed++;
    cells_free++;
}

sval* gc(sval *root) {
    if (cells_free < 1000) {
        gc_force(root);
        if (cells_free < 1000) return OUT_OF_MEMORY;
    }
    return NIL;
}

int inheap(sval* x) {
    return x >= HEAP && x < HEAP + MAX_CELLS;
}

void gc_force(sval *root) {
    // This is a queue, candidates[finger_low:finger_high]
    // Queue is a temporary hack queue for proof-of-concept, we'd like to reduce this to zero memory usage.
    // Represents nodes reachable from the root, but which we haven't yet scanned the children of
    static sval *candidates[MAX_CELLS+5000];

    for (int i=0; i<MAX_CELLS; i++) inuse(&HEAP[i], 0);
    for (int i=0; i<MAX_CELLS; i++) mark(&HEAP[i], 0);
    for (int i=0; i<MAX_CELLS; i++) candidates[i]=0;

    // Start with the root
    candidates[0] = root;
    candidates[1] = STANDARD_ENV;
    long int finger_low = (root ? 0 : 1), finger_high = 2;
    for (int i=finger_low; i<finger_high; i++) mark(candidates[i],1);

    // "Mark" phase
    //printf("Garbage collection\n");
    //printf("  cells used (before) %ld\n", cells_used);
    //printf("  freelist (before) %ld\n", cells_freed);
    //printf("  unallocated (before) %ld\n", cells_free);
    while (finger_low < finger_high) {
        sval *value = candidates[finger_low++];
        inuse(value,1);
        sval *children[3] = {0, 0, 0};

        if (isenv(value)) {
            children[0] = _env_frame(value);
            children[1] = _env_parent(value);
        } else if (ispair(value)) {
            children[0] = car(value);
            children[1] = cdr(value);
        } else if (ismacro(value)) {
            children[0] = _macro_procedure(value);
        } else if (isfunction(value)) {
            inuse(_cdr(value),1);
            children[0] = _function_args(value);
            children[1] = _function_body(value);
            children[2] = _function_env(value);
        }

        for (int i=0; i<3; i++) {
            if (children[i]) {
                sval *child = children[i];
                if (inheap(child) && !isallocated(child)) printf("Mark phase encountered an unallocated child, this is an error.\n");
                if (ismarked(child)) continue; // Already on the list
                mark(child, 1);
                candidates[finger_high++] = child;
            }
        }
    }
    //printf("  finger (low) %ld\n", finger_low);
    //printf("  finger (low) %ld\n", finger_high);

    // "Sweep" phase
    for (int i=0; i<MAX_CELLS; i++) {
        if (isallocated(&HEAP[i]) && !isinuse(&HEAP[i])) {
            free_cell(&HEAP[i]);
        }
    }
    //printf("  cells used (after) %ld\n", cells_used);
    //printf("  freelist (after) %ld\n", cells_freed);
    //printf("  unallocated (after) %ld\n", cells_free-cells_freed);
}


// ----------- Stack allocation ----------

void init_stack() {
    if (STACK==0) STACK = EMPTY_LIST;
}
void push(sval* a) {
    STACK = make_cons(a, STACK);
}
sval* pop() {
    sval* a = car(STACK);
    STACK = cdr(STACK);
    return a;
}

