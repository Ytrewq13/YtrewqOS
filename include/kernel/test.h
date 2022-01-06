#ifndef KERN_TEST_H
#define KERN_TEST_H 1

#define TEST_VERBOSE 2

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "stdlib.h"
#include "printf.h"

typedef enum {
    TEST_PASSED,
    TEST_WARN,
    // A failure we can recover from (continue to the end of this batch of
    // tests)
    TEST_FAIL_CONTINUE,
    // A failure we cannot recover from
    TEST_FAIL_FATAL,
} TEST_RESULT;

typedef struct tests_status {
    size_t passed;
    size_t warned;
    size_t failed;
    // Do we cancel testing after the current batch?
    bool cancel_batch;
    // Do we cancel testing and not attempt the next test in the batch?
    bool cancel_now;
} tests_stat_t;

typedef struct {
    size_t count;
    TEST_RESULT (**test_funs)(char**);
} test_batch_t;

bool test_all(tests_stat_t*);

#endif /* kern_test_h */
