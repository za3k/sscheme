#include "eval.h"
#include "helpers.h"
#include "parser.h"
#include "prims.h"
#include "tests.h"
#include "types.h"
#include <stdio.h>

char buffer[10000]={0};
char *EXAMPLE = buffer;

int main(int argc, char *argv[]) {
    if ( argc == 1) {
        EXAMPLE=slurp_stdin(EXAMPLE);
    } else if ( argc == 2 && strcmp(argv[1],"--test")==0) {
        run_tests(); return 0;
    } else if (argc == 2) {
        EXAMPLE=argv[1];
    } else {
        printf("Usage: eval [SCHEME CODE]\n");
        return 2;
    }
    sexp* parsed = parse(EXAMPLE);
    print1nl(parsed);

    sval* result = eval_all(parsed, empty_env());
    print1nl(result);
}
