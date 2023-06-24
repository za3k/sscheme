#include "parser.h"
#include "tests.h"
#include "helpers.h"
#include "types.h"
#include "eval.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    char *EXAMPLE;
    if ( argc == 1) { run_tests(); return 0; }
    if ( argc == 2) EXAMPLE=argv[1];
    else { printf("Usage: eval [SCHEME CODE]\n"); return 2; }
    sexp* parsed = parse(EXAMPLE);
    print1nl(parsed);

    sval* result = eval(parsed, empty_env());
    print1nl(result);
}
