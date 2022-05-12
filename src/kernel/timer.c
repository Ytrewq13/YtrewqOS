#include "kernel/timer.h"

/* timer.c
 * Copyright Sam Whitehead, 2021
 * Last updated 2021-11-14
 */

#define TIMER_BASE 0x00003000
#define TIMER_CLO  0x4

static uint32_t timer_base = TIMER_BASE;

void timer_set_base(uint32_t base)
{
    timer_base = base;
}

int usleep(useconds_t usec)
{
    struct timer_wait tw = register_timer(usec);
    while (!compare_timer(tw));
    return 0;
}

struct timer_wait register_timer(useconds_t usec)
{
    struct timer_wait tw;
    tw.rollover = 0;
    tw.trigger_value = 0;

    if (usec < 0) {
        errno = EINVAL;
        return tw;
    }
    uint32_t cur_timer = mmio_read(timer_base + TIMER_CLO);
    uint32_t trig = cur_timer + (uint32_t)usec;

    if (cur_timer == 0)
        trig = 0;

    tw.trigger_value = trig;
    if (trig > cur_timer)
        tw.rollover = 0;
    else
        tw.rollover = 1;

    return tw;
}

int compare_timer(struct timer_wait tw)
{
    uint32_t cur_timer = mmio_read(timer_base + TIMER_CLO);

    if (tw.trigger_value == 0)
        return 1;
    if (cur_timer < tw.trigger_value)
    {
        if (tw.rollover)
            tw.rollover = 0;
    } else if (!tw.rollover)
        return 1;

    return 0;
}
