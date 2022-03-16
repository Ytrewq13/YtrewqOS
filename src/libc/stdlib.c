#include "stdlib.h"
#include "drivers/hw/uart.h"

/* stdlib.c
 * Copyright Sam Whitehead, 2021
 * Last updated 2021-11-15
 */

int64_t errno;

void abort(void)
{
    uart0_puts("abort() called\n");
    while (1);
}
