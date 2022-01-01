/* kernel/test/tests_examples.c
 * Copyright Sam Whitehead, 2022
 * Last updated 2022-01-01
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

TEST_RESULT test_fail(char** msg)
{
    *msg = "Hello from a failing test!";
    return TEST_FAIL;
}
