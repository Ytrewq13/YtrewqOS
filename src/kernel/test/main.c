/* kernel/test/main.c
 * Copyright Sam Whitehead, 2021
 * Last updated 2022-01-01
 */

#include "kernel/test.h"

void run_test(TEST_RESULT (*test_func)(char**), struct test_counts *cnts);

// Test declarations
// (test definitions are in external *.c files)

extern TEST_RESULT test_pass(char** msg);
extern TEST_RESULT test_warn(char** msg);
extern TEST_RESULT test_fail(char** msg);

// Run all the tests which are enabled
unsigned int test_all()
{
    struct test_counts cnts = {.passed = 0, .warned = 0, .failed = 0};
    run_test(test_pass, &cnts);
    run_test(test_warn, &cnts);
    run_test(test_fail, &cnts);
    return cnts.failed;
}

// Run an individual test, modifying an external counter of pass/warn/failed
// tests
void run_test(TEST_RESULT (*test_func)(char**), struct test_counts *cnts)
{
    char* msg;
    switch (test_func(&msg)) {
        case TEST_PASSED:
            cnts->passed++;
#if defined (TEST_VERBOSE) && TEST_VERBOSE >= 2
            printf("PASS: %s\n", msg);
#endif
            break;
        case TEST_WARN:
            cnts->warned++;
#if defined (TEST_VERBOSE) && TEST_VERBOSE >= 1
            printf("WARN: %s\n", msg);
#endif
            break;
        case TEST_FAIL:
            cnts->failed++;
            printf("FAIL: %s\n", msg);
            break;
    }
}
