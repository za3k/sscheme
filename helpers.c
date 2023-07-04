#include "helpers.h"
#include "prims.h"
#include "constants.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// TODO: Print quotes and quasiquotes better

int ischar(sval *arg) { return arg >= CHARS_V && arg <= &CHARS_V[127]; }
int iserror(sval *arg) { return arg->tag == ERROR; }
int isempty(sval *arg) { return arg == EMPTY_LIST; }
int isenv(sval *arg) { return arg->tag == ENV; }
int isfalse(sval *arg) { return arg == FALSE; }
int isnil(sval *arg) { return arg == NIL; }
int ismacro(sval *arg) { return arg->tag == MACRO; }
int isnumber(sval *arg) { return arg->tag == NUMBER; }
int ispair(sval *arg) { return arg->tag == PAIR; }
int isprocedure(sval *arg) { return arg->tag == FUNCTION || arg->tag == PRIMITIVE; }
int isstring(sval *arg) { return arg->tag == STRING; }
int issymbol(sval *arg) { return arg->tag == SYMBOL; }
int istrue(sval *arg) { return arg == TRUE; }

sexp* reverse(sval *arg) {
    sval *ret = EMPTY_LIST;
    while (ispair(arg)) {
        ret = make_cons(car(arg), ret);
        arg = cdr(arg);
    }
    return ret;
}

int symboleq(sval *arg1, sval *arg2) {
    return strcmp(arg1->body.symbol,arg2->body.symbol) == 0;
}

int iseqv(sval *arg1, sval *arg2) {
    if (arg1->tag != arg2->tag) return 0;
    switch (arg1->tag) {
        case ERROR: return 0; break;
        case PRIMITIVE: return arg1->body.primitive == arg2->body.primitive;
        case SYMBOL: return symboleq(arg1, arg2);
        case NUMBER: return arg1->body.smallnum == arg2->body.smallnum; break;
        case CONSTANT: return arg1==arg2;
        case SPECIAL_FORM: return arg1==arg2;
        case PAIR: return arg1==arg2;
        case FUNCTION: return arg1==arg2;
        default: return 0;
    }
}

int snprint1(char* buffer, size_t n, sval *arg) {
    int size = 0;
    if (arg->tag == NUMBER) {
        size = snprintf(buffer, n, "%d", arg->body.smallnum);
    } else if (arg->tag == SYMBOL) {
        size = snprintf(buffer, n, ":%s", arg->body.symbol);
    } else if (arg->tag == CONSTANT) {
        if (arg == NIL) size = snprintf(buffer, n, "nil");
        else if (arg == EMPTY_LIST) size = snprintf(buffer, n, "()");
        else if (arg == FALSE) size = snprintf(buffer, n, "#f");
        else if (arg == TRUE) size = snprintf(buffer, n, "#t");
        else if (ischar(arg)) size = snprintf(buffer, n, "%s", char_constant_names[arg-CHARS_V]);
        else size = snprintf(buffer, n, "<Unknown constant>");
    } else if (arg->tag == SPECIAL_FORM) {
        if (arg == QUOTE) size = snprintf(buffer, n, "quote");
        else if (arg == COND) size = snprintf(buffer, n, "cond");
        else if (arg == LAMBDA) size = snprintf(buffer, n, "lambda");
        else if (arg == DEFINE) size = snprintf(buffer, n, "define");
        else if (arg == DEFINE_MACRO) size = snprintf(buffer, n, "define-macro");
        else if (arg == QUASIQUOTE) size = snprintf(buffer, n, "quasiquote");
        else if (arg == UNQUOTE) size = snprintf(buffer, n, "unquote");
        else if (arg == UNQUOTE_SPLICING) size = snprintf(buffer, n, "unquote-splicing");
        else if (arg == SET) size = snprintf(buffer, n, "set!");
        else size = snprintf(buffer, n, "<special form %lx>", (unsigned long) arg);
    } else if (arg->tag == PRIMITIVE) {
        int i;
        for (i=0; primitives[i]!=0; i++) {
            if (primitives[i]==arg->body.primitive) {
                size = snprintf(buffer, n, "<primitive %s>", primitive_names[i]);
                break;
            }
        }
        if (primitives[i]==0) size = snprintf(buffer, n, "<builtin %lx>", (unsigned long) arg->body.primitive);
    } else if (arg->tag == ERROR) {
        size = snprintf(buffer, n, "<error: %s>", arg->body.error);
    } else if (arg->tag == ENV) {
        size = snprintf(buffer, n, "<env: 0x%lx>", (unsigned long) &arg->body.env);
    } else if (arg->tag == FUNCTION) {
        size = snprintf(buffer, n, "<function 0x%lx>", (unsigned long) &arg->body);
    } else if (arg->tag == MACRO) {
        size = snprintf(buffer, n, "<macro ");
        size += snprint1(buffer+size, n-size, arg->body.macro_procedure);
        size += snprintf(buffer+size, n-size, ">");
    } else if (arg->tag == STRING) {
        char *s = arg->body.symbol;
        char c;
        int size=snprintf(buffer, n, "\"");
        while ((c=*(s++))) {
            switch (c) {
                case '\a': size += snprintf(buffer+size, n-size-1, "\\a"); break;
                case '\b': size += snprintf(buffer+size, n-size-1, "\\b"); break;
                case '\f': size += snprintf(buffer+size, n-size-1, "\\f"); break;
                case '\n': size += snprintf(buffer+size, n-size-1, "\\n"); break;
                case '\r': size += snprintf(buffer+size, n-size-1, "\\r"); break;
                case '\t': size += snprintf(buffer+size, n-size-1, "\\t"); break;
                case '\v': size += snprintf(buffer+size, n-size-1, "\\v"); break;
                case '"':  size += snprintf(buffer+size, n-size-1, "\\\""); break;
                case '\\': size += snprintf(buffer+size, n-size-1, "\\\\"); break;
                default:   size += snprintf(buffer+size, n-size-1, "%c", c);
            }
        }
        size+=snprintf(buffer+size, n-size, "\"");
    } else if (arg->tag == PAIR) {
        size = snprintf(buffer, n, "(");
        sval *lst = arg;
        int first=1;
        while (ispair(lst)) {
            if (first) first=0;
            else size += snprintf(buffer+size,n-size," ");
            size += snprint1(buffer+size,n-size,lst->body.list.car);
            lst = lst->body.list.cdr;
        }
        if (isempty(lst)) {} // We finished a normal-style list. Done. (); (1 2 3); (1)
        else { // This is a 'cons', not a list. (1 . 2); (1 2 3 . 4)
            size += snprintf(buffer+size,n-size," . ");
            size += snprint1(buffer+size,n-size,lst);
        }
        size += snprintf(buffer+size,n-size,")");
    } else {
        size = snprintf(buffer, n, "<Unknown value>");
    }
    return size;
}

int islistoflength(sval *arg, int l) {
    while (ispair(arg)) {
        arg = cdr(arg);
        l--;
    }
    return isempty(arg) && l==0;
}

char *slurp_stdin(char* buffer) {
    int size = read(0, buffer, 999999);
    buffer[size] = 0;
    return buffer;
}
