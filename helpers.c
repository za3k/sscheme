#include "helpers.h"
#include "prims.h"
#include <stdio.h>
#include <string.h>

int iserror(sval *arg) { return arg->tag == ERROR; }
int isempty(sval *arg) { return arg->tag == CONSTANT && arg->body.constant == EMPTY_LIST; }
int isnil(sval *arg) { return arg->tag == CONSTANT && arg->body.constant == NIL; }
int isfalse(sval *arg) { return arg->tag == CONSTANT && arg->body.constant == FALSE; }
int istrue(sval *arg) { return arg->tag == CONSTANT && arg->body.constant == TRUE; }
int islist(sval *arg) { return arg->tag == CONS || isempty(arg); }
int isnumber(sval *arg) { return arg->tag == NUMBER; }
int issymbol(sval *arg) { return arg->tag == SYMBOL; }

int symboleq(sval *arg1, sval *arg2) {
    int ret = strcmp(arg1->body.symbol,arg2->body.symbol) == 0;
    return ret;
}

int iseq(sval *arg1, sval *arg2) {
    if (arg1->tag != arg2->tag) return 0;
    switch (arg1->tag) {
        case ERROR: return 0; break;
        case SPECIAL_FORM: return arg1->body.form == arg2->body.form; break;
        case PRIMITIVE: return arg1->body.primitive == arg2->body.primitive; break;
        case SYMBOL: return symboleq(arg1, arg2); break;
        case CONSTANT: return arg1->body.constant == arg2->body.constant; break;
        case NUMBER: return arg1->body.smallnum == arg2->body.smallnum; break;
        case CONS:
        case FUNCTION:
            return arg1==arg2;
            break;
        default: return 0;
    }
}

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
        else if (p==prim_listp) printf("list?");
        else if (p==prim_numberp) printf("number?");
        else if (p==prim_emptyp) printf("empty?");
        else if (p==prim_procedurep) printf("procedure?");
        else if (p==prim_list) printf("list");
        else if (p==prim_print) printf("print");
        else if (p==prim_eqp) printf("eq?");
        else printf("<builtin %lx>", (unsigned long int) p);
    } else if (arg->tag == ERROR) {
        printf("<error: %s>", arg->body.error);
    } else if (arg->tag == FUNCTION) {
        printf("<function %lx>", (unsigned long) &arg->body);
    } else if (arg->tag == CONS) {
        printf("(");
        sval *lst = arg;
        int first=1;
        while (islist(lst) && !isempty(lst)) {
            if (first) first=0;
            else printf(" ");
            print1(lst->body.list.car);
            lst = lst->body.list.cdr;
        }
        if (islist(lst)) { // We finished a normal-style list. Done.
            // ()
            // (1 2 3)
            // (1)
        } else { // This is a 'cons', not a list.
            // (1 . 2)
            // (1 2 3 . 4)
            printf(" . ");
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

int islistoflength(sval *arg, int l) {
    if (l == 0) return isempty(arg);
    else return islist(arg) && !isempty(arg) && islistoflength(arg->body.list.cdr, l-1);
}
