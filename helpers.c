#include "helpers.h"
#include "prims.h"
#include <stdio.h>

int iserror(sval *arg) { return arg->tag == ERROR; }
int isempty(sval *arg) { return arg->tag == CONSTANT && arg->body.constant == EMPTY_LIST; }
int isnil(sval *arg) { return arg->tag == CONSTANT && arg->body.constant == NIL; }
int isfalse(sval *arg) { return arg->tag == CONSTANT && arg->body.constant == FALSE; }
int istrue(sval *arg) { return arg->tag == CONSTANT && arg->body.constant == TRUE; }
int islist(sval *arg) { return arg->tag == CONS || isempty(arg); }
int isnumber(sval *arg) { return arg->tag == NUMBER; }
int issymbol(sval *arg) { return arg->tag == SYMBOL; }

void print1(sval *arg) {
    if (arg->tag == NUMBER) {
        printf("%d", arg->body.smallnum);
    } else if (arg->tag == SYMBOL) {
        printf(":%s", arg->body.symbol);
    } else if (arg->tag == CONSTANT) {
        switch (arg->body.constant) {
            case NIL: printf("NIL"); break;
            case EMPTY_LIST: printf("()"); break;
            case FALSE: printf("false"); break;
            case TRUE: printf("true"); break;
            default: printf("<Unknown constant>");
        }
    } else if (arg->tag == SPECIAL_FORM) {
        switch(arg->body.form) {
            case quote: printf("quote"); break;
            case cond: printf("cond"); break;
            case lambda: printf("lambda"); break;
            default: printf("<special form %d>", arg->body.form); break;
        }
    } else if (arg->tag == PRIMITIVE) {
        sval* (*p)(sval*) = arg->body.primitive;
        if (p==prim_cons) printf("cons");
        else if (p==prim_car)  printf("car");
        else if (p==prim_cdr)  printf("cdr");
        else if (p==prim_plus) printf("+");
        else if (p==prim_minus) printf("-");
        else if (p==prim_nilp) printf("nil?");
        else if (p==prim_falsep) printf("false?");
        else if (p==prim_truep) printf("true?");
        else if (p==prim_listp) printf("list?");
        else if (p==prim_numberp) printf("number?");
        else if (p==prim_emptyp) printf("empty?");
        else if (p==prim_list) printf("list");
        else if (p==prim_print) printf("print");
        else printf("<builtin %lx>", (unsigned long int) p);
    } else if (arg->tag == ERROR) {
        printf("<error: %s>", arg->body.error);
    } else if (arg->tag == FUNCTION) {
        printf("<function %lx>", (unsigned long) &arg->body);
    } else if (arg->tag == CONS) {
        printf("(");
        sval *lst = arg;
        while (islist(lst) && !isempty(lst)) {
            print1(lst->body.list.car);
            printf(" ");
            lst = lst->body.list.cdr;
        }
        if (islist(lst)) { // We finished a normal-style list. Done.
            // ()
            // (1 2 3)
            // (1)
        } else { // This is a 'cons', not a list.
            // (1 . 2)
            // (1 2 3 . 4)
            printf(". ");
            print1(lst);
        }
        printf(")");
    } else {
        printf("<Unknown value>");
    }
}

void print1nl(sval *arg1) {
    print1(arg1); printf("\n");
}
