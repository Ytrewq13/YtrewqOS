/* kernel/test/tests_examples.c
 * Copyright Sam Whitehead, 2022
 * Last updated 2022-01-02
 */

#include "kernel/test.h"

// Test implementations

TEST_RESULT test_pass(char** msg)
{
    *msg = "Hello from a passing test!";
    return TEST_PASSED;
}

TEST_RESULT test_warn(char** msg)
{
    *msg = "Hello from a warning test!";
    return TEST_WARN;
}

TEST_RESULT test_fail_continue(char** msg)
{
    *msg = "Hello from a (recoverably) failing test!";
    return TEST_FAIL_CONTINUE;
}

TEST_RESULT test_fail_abort(char** msg)
{
    *msg = "Hello from a failing test that cannot be recovered from!";
    return TEST_FAIL_ABORT;
}

TEST_RESULT (*funs[4])(char**) = {test_pass, test_warn, test_fail_continue, test_fail_abort};
// The test batch
test_batch_t test_batch_examples = {.count = 4, .test_funs = funs};
