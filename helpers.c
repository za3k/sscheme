#include "helpers.h"
#include "prims.h"
#include "constants.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// TODO: Print quotes and quasiquotes better

int ischar(sval *arg) { return arg >= CHARS_V && arg <= &CHARS_V[127]; }
int isconstant(sval *arg) { return arg->tag == CONSTANT; }
int iserror(sval *arg) { return arg->tag == ERROR; }
int isempty(sval *arg) { return arg == EMPTY_LIST; }
int isenv(sval *arg) { return arg->tag == ENV; }
int isfalse(sval *arg) { return arg == FALSE; }
int isform(sval *arg) { return arg->tag == SPECIAL_FORM; }
int isfunction(sval *arg) { return arg->tag == FUNCTION; }
int isnil(sval *arg) { return arg == NIL; }
int ismacro(sval *arg) { return arg->tag == MACRO; }
int isnumber(sval *arg) { return arg->tag == NUMBER; }
int ispair(sval *arg) { return arg->tag == PAIR; }
int isprimitive(sval *arg) { return arg->tag == PRIMITIVE; }
int isprocedure(sval *arg) { return isfunction(arg) || isprimitive(arg); }
int isstring(sval *arg) { return arg->tag == STRING; }
int issymbol(sval *arg) { return arg->tag == SYMBOL; }
int sametype(sval *arg1, sval *arg2) { return arg1->tag == arg2->tag; }

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
    if (!sametype(arg1, arg2)) return 0;
    if (iserror(arg1)) return 0;
    else if (arg1==arg2) return 1;
    else if (isprimitive(arg1)) return arg1->body.primitive == arg2->body.primitive;
    else if (issymbol(arg1)) return symboleq(arg1, arg2);
    else if (isnumber(arg1)) return arg1->body.smallnum == arg2->body.smallnum;
    else return 0;
}

int snprint1(char* buffer, size_t n, sval *arg) {
    int size = 0;
    if (isnumber(arg)) size = snprintf(buffer, n, "%d", arg->body.smallnum);
    else if (issymbol(arg)) size = snprintf(buffer, n, ":%s", arg->body.symbol);
    else if (isconstant(arg)) {
        if (arg == NIL) size = snprintf(buffer, n, "nil");
        else if (arg == EMPTY_LIST) size = snprintf(buffer, n, "()");
        else if (arg == FALSE) size = snprintf(buffer, n, "#f");
        else if (arg == TRUE) size = snprintf(buffer, n, "#t");
        else if (ischar(arg)) size = snprintf(buffer, n, "%s", char_constant_names[arg-CHARS_V]);
        else size = snprintf(buffer, n, "<Unknown constant>");
    } else if (isform(arg)) {
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
    } else if (isprimitive(arg)) {
        int i;
        for (i=0; primitives[i]!=0; i++) {
            if (primitives[i]==arg->body.primitive) {
                size = snprintf(buffer, n, "<primitive %s>", primitive_names[i]);
                break;
            }
        }
        if (primitives[i]==0) size = snprintf(buffer, n, "<builtin %lx>", (unsigned long) arg->body.primitive);
    } else if (iserror(arg)) size = snprintf(buffer, n, "<error: %s>", arg->body.error);
    else if (isenv(arg)) size = snprintf(buffer, n, "<env: 0x%lx>", (unsigned long) &arg->body.env);
    else if (isfunction(arg)) size = snprintf(buffer, n, "<function 0x%lx>", (unsigned long) &arg->body);
    else if (ismacro(arg)) {
        size = snprintf(buffer, n, "<macro ");
        size += snprint1(buffer+size, n-size, arg->body.macro_procedure);
        size += snprintf(buffer+size, n-size, ">");
    } else if (isstring(arg)) {
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
    } else if (ispair(arg)) {
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
    } else size = snprintf(buffer, n, "<Unknown value>");
    return size;
}

int listlength(sval *arg) {
    int l=0;
    while (ispair(arg)) {
        arg = cdr(arg);
        l++;
    }
    return isempty(arg) ? l : -1;
}

int islistoflength(sval *arg, int l) {
    return l == listlength(arg);
}

char *slurp_stdin(char* buffer) {
    int size = read(0, buffer, 999999);
    buffer[size] = 0;
    return buffer;
}
