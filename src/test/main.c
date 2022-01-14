/* kernel/test/main.c
 * Copyright Sam Whitehead, 2021
 * Last updated 2022-01-06
 */

#include "drivers/graphics/console.h"
#include "kernel/test.h"
#include "string.h"

static void run_test(TEST_RESULT (*test_func)(char**), tests_stat_t *status);
static void run_test_batch(test_batch_t batch, tests_stat_t *status);

// Test batch declarations
// (test definitions are in external *.c files)
extern test_batch_t test_batch_examples;
extern test_batch_t test_batch_malloc;
extern test_batch_t test_batch_emmc;

// Run all the tests which are enabled
bool test_all(tests_stat_t *stats_return)
{
    bool failed, allocd = false;
    tests_stat_t *tests_status;
#if defined (TEST_VERBOSE) && TEST_VERBOSE >= 1
    printf("Kernel tests\n============\n");
#endif
    // Prepare the status structure
    if (!stats_return) {
        allocd = true;
        tests_status = malloc(sizeof(tests_stat_t));
    } else {
        tests_status = stats_return;
    }
    memset(tests_status, 0, sizeof(tests_stat_t));
    // Run the tests
    run_test_batch(test_batch_examples, tests_status);
    run_test_batch(test_batch_malloc, tests_status);
    run_test_batch(test_batch_emmc, tests_status);
    // Get the failure value from the status struct
    failed = tests_status->failed > 0 || tests_status->cancel_batch ||
        tests_status->cancel_now;
    if (allocd) free(tests_status);
    return !failed;
}

// Run a batch of tests
static void run_test_batch(test_batch_t batch, tests_stat_t *status)
{
    size_t i;
    size_t skip_cnt;
    status->total += batch.count;
    for (i = 0; i < batch.count; i++) {
        run_test(batch.test_funs[i], status);
        if (status->cancel_now) {
            skip_cnt = batch.count - i - 1;
            status->skipped += skip_cnt;
#if defined (TEST_VERBOSE) && TEST_VERBOSE >= 2
            printf("  ");
            console_set_tmp_fg_color(CONFIG_COLOR_TEST_SKIP);
            printf("SKIP:");
            console_reset_colors();
            printf(" %lu tests skipped (aborting batch)\n", skip_cnt);
#endif
            break;
        }
    }
}

// Run an individual test, modifying an external counter of pass/warn/failed
// tests
static void run_test(TEST_RESULT (*test_func)(char**), tests_stat_t *status)
{
    char* msg;
    char* msg_prefix = "  ";
    switch (test_func(&msg)) {
        case TEST_PASSED:
            status->passed++;
#if defined (TEST_VERBOSE) && TEST_VERBOSE >= 2
            printf("%s", msg_prefix);
            console_set_tmp_fg_color(CONFIG_COLOR_TEST_PASS);
            printf("PASS");
            if (strlen(msg) > 0) printf(":");
            console_reset_colors();
            printf(" %s\n", msg);
#endif
            break;
        case TEST_WARN:
            status->passed++;  // Warning tests count as passed
            status->warned++;
#if defined (TEST_VERBOSE) && TEST_VERBOSE >= 1
            printf("%s", msg_prefix);
            console_set_tmp_fg_color(CONFIG_COLOR_TEST_WARN);
            printf("WARN");
            if (strlen(msg) > 0) printf(":");
            console_reset_colors();
            printf(" %s\n", msg);
#endif
            break;
        case TEST_FAIL_CONTINUE:
            status->failed++;
            status->cancel_batch = true;
            printf("%s", msg_prefix);
            console_set_tmp_fg_color(CONFIG_COLOR_TEST_FAIL);
            printf("FAIL");
            if (strlen(msg) > 0) printf(":");
            console_reset_colors();
            printf(" %s\n", msg);
            break;
        case TEST_FAIL_FATAL:
            status->failed++;
            status->cancel_now = true;
            status->cancel_batch = true;
            printf("%s", msg_prefix);
            console_set_tmp_fg_color(CONFIG_COLOR_TEST_FAIL);
            printf("FAIL");
            if (strlen(msg) > 0) printf(":");
            console_reset_colors();
            printf(" %s\n", msg);
            break;
    }
}
