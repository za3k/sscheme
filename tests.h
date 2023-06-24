#define TESTS_FILE "tests.txt"

int read_tests(char *path, char** inputs, char **expected_outputs);
int run_test(char* input, char* expected_output);
int run_tests();
