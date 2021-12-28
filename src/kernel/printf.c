/* printf.c
 * Copyright Sam Whitehead, 2021
 * Last updated 2021-12-28
 */

#include "printf.h"

// Which printf methods we should try to use
#define PRINTF_USE_UART0    1
#define PRINTF_USE_UART1    0
#define PRINTF_USE_GRAPHICS 1

// printf should run on uart0, uart1, and console if they are enabled
int printf(const char *fmt, ...)
{
    int count = 0, c = 0;
    // va_list can be reused if it is disposed of with va_end and then
    // reinitialised with va_start
    va_list ap;
#if HAVE_UART0_PUTC && PRINTF_USE_UART0
    if (uart0_putc_enabled) {
        va_start(ap, fmt);
        c = generic_printf(uart0_putc, fmt, ap);
        va_end(ap);
        count = (count == 0) ? c : ((count == c) ? c : -1);
    }
#endif
#if HAVE_UART1_PUTC && PRINTF_USE_UART1
    if (uart1_putc_enabled) {
        va_start(ap, fmt);
        c = generic_printf(uart0_putc, fmt, ap);
        va_end(ap);
        count = (count == 0) ? c : ((count == c) ? c : -1);
    }
#endif
#if HAVE_CONSOLE_PUTC && PRINTF_USE_GRAPHICS
    if (console_putc_enabled) {
        va_start(ap, fmt);
        c = generic_printf(console_putc, fmt, ap);
        va_end(ap);
        count = (count == 0) ? c : ((count == c) ? c : -1);
    }
#endif
    return count;
}
