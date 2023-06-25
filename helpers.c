#include "helpers.h"
#include "prims.h"
#include "constants.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// TODO: Include tests.txt and standard.txt as string constants instead of slurping them.

int iserror(sval *arg) { return arg->tag == ERROR; }
int isempty(sval *arg) { return arg == EMPTY_LIST; }
int isnil(sval *arg) { return arg == NIL; }
int isfalse(sval *arg) { return arg == FALSE; }
int istrue(sval *arg) { return arg == TRUE; }
// TODO: Rename to ispair, name CONS->PAIR
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

static char print_buffer[1000];
void print1(sval *arg) {
    snprint1(print_buffer, sizeof(print_buffer), arg);
    printf("%s",print_buffer);
}

void print1nl(sval *arg) {
    snprint1(print_buffer, sizeof(print_buffer), arg);
    printf("%s\n",print_buffer);
}

int snprint1(char* buffer, size_t n, sval *arg) {
    int size;
    if (arg->tag == NUMBER) {
        size = snprintf(buffer, n, "%d", arg->body.smallnum);
    } else if (arg->tag == SYMBOL) {
        size = snprintf(buffer, n, ":%s", arg->body.symbol);
    } else if (arg->tag == CONSTANT) {
        switch (arg->body.constant) {
            case constant_nil: size = snprintf(buffer, n, "nil"); break;
            case constant_empty_list: size = snprintf(buffer, n, "()"); break;
            case constant_false: size = snprintf(buffer, n, "#f"); break;
            case constant_true: size = snprintf(buffer, n, "#t"); break;
            case C000...C127: size = snprintf(buffer, n, "%s", char_constant_names[arg->body.constant-C000]); break;
            //default: size = snprintf(buffer, n, "<Unknown constant>");
        }
    } else if (arg->tag == SPECIAL_FORM) {
        switch(arg->body.form) {
            case form_quote: size = snprintf(buffer, n, "quote"); break;
            case form_cond: size = snprintf(buffer, n, "cond"); break;
            case form_lambda: size = snprintf(buffer, n, "lambda"); break;
            case form_define: size = snprintf(buffer, n, "define"); break;
            case form_define_macro: size = snprintf(buffer, n, "define-macro"); break;
            //default: size = snprintf(buffer, n, "<special form %d>", arg->body.form); break;
        }
    } else if (arg->tag == PRIMITIVE) {
        int i;
        for (i=0; primitives[i]!=0; i++) {
            if (primitives[i]==arg->body.primitive) {
                size = snprintf(buffer, n, "%s", primitive_names[i]);
                break;
            }
        }
        if (primitives[i]==0) size = snprintf(buffer, n, "<builtin %lx>", (unsigned long int) arg->body.primitive);
    } else if (arg->tag == ERROR) {
        size = snprintf(buffer, n, "<error: %s>", arg->body.error);
    } else if (arg->tag == FUNCTION) {
        size = snprintf(buffer, n, "<function 0x%lx>", (unsigned long) &arg->body);
    } else if (arg->tag == MACRO) {
        size = snprintf(buffer, n, "<macro ");
        size += snprint1(buffer+size, n-size, arg->body.macro_procedure);
        size += snprintf(buffer+size, n-size, ">");
    } else if (arg->tag == CONS) {
        size = snprintf(buffer, n, "(");
        sval *lst = arg;
        int first=1;
        while (islist(lst) && !isempty(lst)) {
            if (first) first=0;
            else size += snprintf(buffer+size,n-size," ");
            size += snprint1(buffer+size,n-size,lst->body.list.car);
            lst = lst->body.list.cdr;
        }
        if (islist(lst)) { // We finished a normal-style list. Done.
            // ()
            // (1 2 3)
            // (1)
        } else { // This is a 'cons', not a list.
            // (1 . 2)
            // (1 2 3 . 4)
            size += snprintf(buffer+size,n-size," . ");
            size += snprint1(buffer+size,n-size,lst);
        }
        size += snprintf(buffer+size,n-size,")");
    } else {
        size = printf(buffer, n, "<Unknown value>");
    }
    return size;
}

int islistoflength(sval *arg, int l) {
    if (l == 0) return isempty(arg);
    else return islist(arg) && !isempty(arg) && islistoflength(arg->body.list.cdr, l-1);
}

char* slurp_file(char *path, char* buffer) {
    FILE *infile = fopen(path, "r");
    fseek(infile, 0L, SEEK_END);
    int numbytes = ftell(infile);
    fseek(infile, 0L, SEEK_SET);
    fread(buffer, sizeof(char), numbytes, infile);
    fclose(infile);
    buffer[numbytes]='\0';
    return buffer;
}

char *slurp_stdin(char* buffer) {
    int size = read(0, buffer, 999999);
    buffer[size] = 0;
    return buffer;
}
