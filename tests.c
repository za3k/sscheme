#include "tests.h"

#include "config.h"
#include "eval.h"
#include "helpers.h"
#include "tests.inc.h"
#include "types.h"
#include "parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int read_test(char **remaining_file, char* input, char *expected_output) {
    // Read one test from *remaining_file. Return 1 for success, 0 for failure.
    // Results are written to input/expected_output strings.
    char *f = *remaining_file;
    char *inp = input, *outp = expected_output;

    while (f[0]==' ' || f[0] == '\n') f++;
    if (f[0] == 0) return 0;
    while (!(f[0]=='=' && f[1]=='>')) *inp++ = *f++;
    *inp = 0;
    f+=2;
    while (f[0]==' ' || f[0] == '\n') f++;
    while (f[0]!='\n') *outp++ = *f++;
    *outp = 0;

    //printf("Test read:\n  Input: %s\n  Output: %s\n\n", input, expected_output);

    *remaining_file = f;
    return 1;
}

int run_test(char* input, char* expected_output) {
    static char output[MAX_TEST_OUTPUT_SIZE];
    // Return 0 on success, 1 on failure. Print to stdout.
    //printf("Toparse: %s\n", input);
    sexp* parsed = parse(input);
    if (!parsed) return 1;
    //print1nl(parsed);
    sval* result = eval_all(parsed, empty_env());

    snprint1(output, sizeof(output), result);
    int success = strcmp(output,expected_output)==0 ||
      (strcmp("proc", expected_output)==0 && isprocedure(result)) ||
      (strcmp("err", expected_output)==0 && iserror(result));
    if (success) {
        //printf("pass\n");
    } else {
        printf("\nTest: %s\n", input);
        printf("          => %s\n", output);
        printf("Expected: => %s\n", expected_output);
    }
    //free(parsed);
    //free(result);
    return 1-success;
}

int run_tests() {
    int num_tests=0, i, failed_tests=0;

    char *remaining_file = tests_txt;
    static char input[MAX_TEST_INPUT_SIZE];
    static char expected_output[MAX_TEST_OUTPUT_SIZE];
    for (i=0; read_test(&remaining_file, input, expected_output); i++) {
        num_tests++;
        //printf("Running test %d... ", i+1);
        failed_tests += run_test(input, expected_output);
    }
    printf("%d/%d tests passed: ", num_tests-failed_tests, num_tests);
    if (failed_tests > 0) printf("FAILURE\n");
    else printf("SUCCESS\n");
    return failed_tests > 0;
}
