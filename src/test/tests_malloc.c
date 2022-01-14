/* tests_malloc.c
 * Copyright Sam Whitehead, 2022
 * Last updated 2022-01-07
 */

#include "kernel/test.h"
#include "stdlib.h"

#define MALLOC_SIZE_TINY 1
#define MALLOC_SIZE_SMALL 1024
#define MALLOC_SIZE_LARGE 4097  // Larger than one page

static TEST_RESULT test_malloc_nonnull_tiny(char **msg)
{
    void *bytes;
    bytes = malloc(MALLOC_SIZE_TINY);
    if (bytes == NULL) {
        *msg = "malloc(tiny) returned NULL";
        return TEST_FAIL_CONTINUE;
    }
    free(bytes);
    *msg = "";
    return TEST_PASSED;
}

static TEST_RESULT test_malloc_nonnull_small(char **msg)
{
    void *bytes;
    bytes = malloc(MALLOC_SIZE_SMALL);
    if (bytes == NULL) {
        *msg = "malloc(small) returned NULL";
        return TEST_FAIL_CONTINUE;
    }
    free(bytes);
    *msg = "";
    return TEST_PASSED;
}

static TEST_RESULT test_malloc_nonnull_large(char **msg)
{
    void *bytes;
    bytes = malloc(MALLOC_SIZE_LARGE);
    if (bytes == NULL) {
        *msg = "malloc(large) returned NULL";
        return TEST_FAIL_CONTINUE;
    }
    free(bytes);
    *msg = "";
    return TEST_PASSED;
}

extern void *sbrk(ptrdiff_t);

static TEST_RESULT test_malloc_free_brk_tiny(char **msg)
{
    void *brk_a, *brk_b, *bytes;
    brk_a = sbrk(0);
    bytes = malloc(MALLOC_SIZE_TINY);
    free(bytes);
    brk_b = sbrk(0);
    if (brk_a != brk_b) {
        *msg = "malloc(tiny) then free(tiny) results in a change to the program break";
        return TEST_FAIL_FATAL;
    }
    *msg = "";
    return TEST_PASSED;
}

static TEST_RESULT test_malloc_free_brk_small(char **msg)
{
    void *brk_a, *brk_b, *bytes;
    brk_a = sbrk(0);
    bytes = malloc(MALLOC_SIZE_SMALL);
    free(bytes);
    brk_b = sbrk(0);
    if (brk_a != brk_b) {
        *msg = "malloc(small) then free(small) results in a change to the program break";
        return TEST_FAIL_FATAL;
    }
    *msg = "";
    return TEST_PASSED;
}

static TEST_RESULT test_malloc_free_brk_large(char **msg)
{
    void *brk_a, *brk_b, *bytes;
    brk_a = sbrk(0);
    bytes = malloc(MALLOC_SIZE_LARGE);
    free(bytes);
    brk_b = sbrk(0);
    if (brk_a != brk_b) {
        *msg = "malloc(large) then free(large) results in a change to the program break";
        return TEST_FAIL_FATAL;
    }
    *msg = "";
    return TEST_PASSED;
}

TEST_RESULT (*funs_tests_malloc[6])(char**) = {test_malloc_nonnull_tiny, test_malloc_nonnull_small, test_malloc_nonnull_large, test_malloc_free_brk_tiny, test_malloc_free_brk_small, test_malloc_free_brk_large};
// The test batch
test_batch_t test_batch_malloc = {.count = 3, .test_funs = funs_tests_malloc};
