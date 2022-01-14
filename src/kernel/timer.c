#include "kernel/timer.h"

/* timer.c
 * Copyright Sam Whitehead, 2021
 * Last updated 2021-11-14
 */

int usleep(useconds_t usec)
{
    // FIXME: implement this function
    return 0;
}

struct timer_wait register_timer(useconds_t usec)
{
    // FIXME: implement this function
    struct timer_wait ret = {0};
    return ret;
}

int compare_timer(struct timer_wait tw)
{
    // FIXME: implement this function
    return 0;
}
