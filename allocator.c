#include "allocator.h"

#include "types.h"
#include "config.h"
#include "prims.h"
#include "errors.h"
#include "constants.h"

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

// ----------- Heap allocation ----------
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
        FREE_LIST = ret->body.list.cdr;
        ret->body.list.cdr = 0;
        ret->allocated = 1;
        return ret;
    } else {
        // Allocate a new cell
        HEAP[HEAP_END].allocated = 1;
        return &HEAP[HEAP_END++];
    }
}

void free_cell(sval* cell) {
    if (cell==0) return;
    if (cell->tag == SYMBOL || cell->tag == STRING) {
        free(cell->body.symbol);
    } else if (cell->tag == ERROR) {
        free(cell->body.error);
    }
    cell->tag = PAIR;
    cell->body.list.car = 0;
    cell->body.list.cdr = FREE_LIST;
    cell->allocated = 0;
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

    for (int i=0; i<MAX_CELLS; i++) HEAP[i].in_use=0;
    for (int i=0; i<MAX_CELLS; i++) HEAP[i].marked=0;
    for (int i=0; i<MAX_CELLS; i++) candidates[i]=0;

    // Start with the root
    candidates[0] = root;
    candidates[1] = STANDARD_ENV;
    long int finger_low = (root ? 0 : 1), finger_high = 2;
    for (int i=finger_low; i<finger_high; i++) candidates[i]->marked = 1;

    // "Mark" phase
    //printf("Garbage collection\n");
    //printf("  cells used (before) %ld\n", cells_used);
    //printf("  freelist (before) %ld\n", cells_freed);
    //printf("  unallocated (before) %ld\n", cells_free);
    while (finger_low < finger_high) {
        sval *value = candidates[finger_low++];
        value->in_use = 1;
        sval *children[3] = {0, 0, 0};

        if (value->tag == ENV) {
            children[0] = value->body.env.frame;
            children[1] = value->body.env.parent;
        } else if (value->tag == PAIR) {
            children[0] = value->body.list.car;
            children[1] = value->body.list.cdr;
        } else if (value->tag == MACRO) {
            children[0] = value->body.macro_procedure;
        } else if (value->tag == FUNCTION) {
            children[0] = value->body.closure.parameters;
            children[1] = value->body.closure.body;
            children[2] = value->body.closure.env;
        }

        for (int i=0; i<3; i++) {
            if (children[i]) {
                sval *child = children[i];
                if (inheap(child) && !child->allocated) printf("Mark phase encountered an unallocated child, this is an error.\n");
                if (child->marked) continue; // Already on the list
                child->marked = 1;
                candidates[finger_high++] = child;
            }
        }
    }
    //printf("  finger (low) %ld\n", finger_low);
    //printf("  finger (low) %ld\n", finger_high);

    // "Sweep" phase
    for (int i=0; i<MAX_CELLS; i++) {
        if (HEAP[i].allocated && !HEAP[i].in_use) {
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
