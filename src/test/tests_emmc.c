/* kernel/test/tests_emmc.c
 * Copyright Sam Whitehead, 2022
 * Last updated 2022-01-06
 */

#include "kernel/test.h"
#include "drivers/hw/eMMC.h"

// Test implementations

TEST_RESULT test_sd_init(char **msg)
{
    struct block_device *foo = NULL;
    int ret = sd_card_init(&foo);
    if (ret != 0) {
        // TODO: sprintf()
        *msg = "sd_card_init() returned a nonzero value";
        return TEST_WARN;
    }
    *msg = "";
    free(foo);
    return TEST_PASSED;
}

TEST_RESULT (*funs_tests_emmc[1])(char**) = {test_sd_init};
// The test batch
test_batch_t test_batch_emmc = {.count = 1, .test_funs = funs_tests_emmc};
