#ifndef KERN_TEST_H
#define KERN_TEST_H 1

#include <stdbool.h>
#include <stddef.h>

#include "printf.h"

typedef enum {
    TEST_PASSED,
    TEST_WARN,
    TEST_FAIL,
} TEST_RESULT;

struct test_counts {
    size_t passed;
    size_t warned;
    size_t failed;
};

unsigned int test_all();

#endif /* kern_test_h */
