#include "tests.h"
#include "tests.inc.h"
#include "types.h"
#include "eval.h"
#include "parser.h"
#include "helpers.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//int snprint1(char* buffer, size_t n, sval *arg);
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

int read_tests(char *path, char** inputs, char **expected_outputs) {
    // Return number of tests read
    int i;
    char *remaining_file = tests_txt;
    //printf("Reading test... ");
    for(i=0;;i++) {
        inputs[i] = malloc(1000);
        expected_outputs[i] = malloc(1000);
        if (!read_test(&remaining_file, inputs[i], expected_outputs[i])) break;
        //printf("%d,",i+1);
    }
    //printf("finished\n");
    return i;
}

static char output[1000];
int run_test(char* input, char* expected_output) {
    // Return 0 on success, 1 on failure. Print to stdout.
    //printf("Toparse: %s\n", input);
    sexp* parsed = parse(input);
    //print1nl(parsed);
    sval* result = eval_all(parsed, empty_env());

    snprint1(output, sizeof(output), result);
    int success = strcmp(output,expected_output)==0 ||
      (strcmp("proc", expected_output)==0 && result->tag == FUNCTION) ||
      (strcmp("proc", expected_output)==0 && result->tag == PRIMITIVE) ||
      (strcmp("err", expected_output)==0 && result->tag == ERROR);
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
    char * inputs[1000];
    char * expected_outputs[1000];
    int num_tests, i, failed_tests=0;

    num_tests = read_tests(TESTS_FILE, inputs, expected_outputs);
    for (i=0; i<num_tests;i++) {
        //printf("Running test %d... ", i+1);
        failed_tests += run_test(inputs[i], expected_outputs[i]);
    }
    printf("%d/%d tests passed: ", num_tests-failed_tests, num_tests);
    if (failed_tests > 0) printf("FAILURE\n");
    else printf("SUCCESS\n");
    return failed_tests > 0;
}
