#include "config.h"
#include "eval.h"
#include "helpers.h"
#include "parser.h"
#include "prims.h"
#include "tests.h"
#include "types.h"
#include <stdio.h>

char buffer[INPUT_BUFFER_SIZE]={0};
char *EXAMPLE = buffer;

void debug_size() {
    printf("Space available: %lu bytes per cell x %lu cells = %lu KB total\n", sizeof(sval), MAX_CELLS, sizeof(sval)*MAX_CELLS/1000);
    printf("  sval:                      %lu\n", sizeof(sval));
    printf("  sval.tag:                  %lu\n", sizeof(((sval *)0)->tag));
    printf("  sval.body:                 %lu\n", sizeof(((sval *)0)->body));
    printf("  sval.body.list:            %lu\n", sizeof(((sval *)0)->body.list));
    printf("  sval.body.symbol:          %lu\n", sizeof(((sval *)0)->body.symbol));
    printf("  sval.body.smallnum:        %lu\n", sizeof(((sval *)0)->body.smallnum));
    printf("  sval.body.constant:        %lu\n", sizeof(((sval *)0)->body.constant));
    printf("  sval.body.form:            %lu\n", sizeof(((sval *)0)->body.form));
    printf("  sval.body.macro_procedure: %lu\n", sizeof(((sval *)0)->body.macro_procedure));
    printf("  sval.body.primitive:       %lu\n", sizeof(((sval *)0)->body.primitive));
    printf("  sval.body.closure:         %lu\n", sizeof(((sval *)0)->body.closure));
    printf("  sval.body.env:             %lu\n", sizeof(((sval *)0)->body.env));
    printf("  sval.body.error:           %lu\n", sizeof(((sval *)0)->body.error));
}

int main(int argc, char *argv[]) {
    if ( argc == 1) {
        EXAMPLE=slurp_stdin(EXAMPLE);
    } else if ( argc == 2 && strcmp(argv[1],"--test")==0) {
        run_tests(); return 0;
    } else if ( argc == 2 && strcmp(argv[1],"--size")==0) {
        debug_size(); return 0;
    } else if (argc == 2) {
        EXAMPLE=argv[1];
    } else {
        printf("Usage: eval [SCHEME CODE]\n");
        return 2;
    }
    sexp *env = empty_env();

    #ifdef LOGGING_ON
    printf("------------------Start Execution--------------\n");
    #endif

    sexp* parsed = parse(EXAMPLE);
    printf("Input: "); print1nl(parsed);

    sval* result = eval_all(parsed, env);
    print1nl(result);
}
