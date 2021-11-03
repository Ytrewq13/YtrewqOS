/* uart_PL011.c
 * Copyright Sam Whitehead, 2021
 * Last updated 2021-08-10
 */

#include "uart.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

#include "gpio.h"
#include "io/printf.h"
#include "mbox.h"

extern void PUT32(uint64_t addr, uint32_t x);
extern uint32_t GET32(uint64_t addr);

/* The following is taken from example code found at
 * https://github.com/bztsrc/raspi3-tutorial/blob/master/05_uart0/uart.c
 * Modified slightly to use our enums instead of '#define's and the GET32()
 * and PUT32() functions defined in our boot.S file.
 */

void uart0_init()
{
    register uint32_t r;

    uint32_t __attribute__((aligned(16))) mbox[9];
//    uint32_t mbox_input[3];

    // Disable the UART
    PUT32(UART0_CR, 0);

    // Setup the mbox
    mbox[0] = 9*4;
    mbox[1] = MBOX_REQUEST;
    mbox[2] = MBOX_TAG_SET_CLK_RATE;
    mbox[3] = 12;
    mbox[4] = 8; // TODO: determine why this can't be 0
    mbox[5] = 2; // UART clock
    mbox[6] = 4000000; // 4MHz
    mbox[7] = 0; // Clear turbo
    mbox[8] = MBOX_TAG_LAST;

    mbox_call_raw(mbox);
//    mbox_command_start(MBOX_CH_PROP_W, mbox);
//    mbox_command_wait(MBOX_CH_PROP_W, mbox);

    // Setup the mbox input
//    mbox_input[0] = 2; // UART clock id (TODO: put this in a #define in uart.h or mbox.h)
//    mbox_input[1] = 4000000; // 4MHz
//    mbox_input[2] = 0; // Clear turbo

    // Mailbox call to set the clock rate
//    mbox_prop_call_internal(mbox, MBOX_TAG_SET_CLK_RATE, 12, ~0, mbox_input, NULL);

    // Map UART to GPIO pins
    r = GET32(GPFSEL1);
    r &= ~((7 << 12) | (7 << 15));  // GPIO14, GPIO15
    r |=   (4 << 12) | (4 << 15);   // alt0
    PUT32(GPFSEL1, r);
    PUT32(GPPUD, 0);  // Enable pins 14 and 15
    r = 150;
    while (r--) __asm volatile("nop");  // Delay 150
    PUT32(GPPUDCLK0, (1 << 14) | (1 << 15));
    r = 150;
    while (r--) __asm volatile("nop");  // Delay 150
    PUT32(GPPUDCLK0, 0);                // flush GPIO setup

    PUT32(UART0_ICR, 0x7FF); // Clear Interrupts
    PUT32(UART0_IBRD, 2); // 115200 baud
    PUT32(UART0_FBRD, 0xB);
    PUT32(UART0_LCRH, 0x7 << 4); // 8n1, enable FIFOs
    PUT32(UART0_CR, 0x301); // Enable Tx, Rx, UART
}

void uart0_send(uint32_t c)
{
    // Wait until we can send
    while ((GET32(UART0_FR) & 0x20)) __asm volatile("nop");
    // Write the character to the buffer
    PUT32(UART0_DR, c);
}

void uart0_putc(uint32_t c)
{
    if (c == '\n') uart0_send('\r');
    uart0_send(c);
}

char uart0_getc()
{
    char r;
    // Wait until something is in the buffer
    while ((GET32(UART0_FR) & 0x10)) __asm volatile("nop");
    // Read it and return
    r = GET32(UART0_DR);
    // Convert carriage return to newline
    return (r == '\r' ? '\n' : r);
}

void uart0_puts(char* s)
{
    if (!s) return;
    while (*s) {
        if (*s == '\n') {
            while ((GET32(UART0_FR) & 0x20)) __asm volatile("nop");
            PUT32(UART0_DR, '\r');
        }
//        if (*s == '\n') uart0_send('\r');
        while ((GET32(UART0_FR) & 0x20)) __asm volatile("nop");
        PUT32(UART0_DR, *s++);
//        uart0_send(*s++);
    }
}

void uart0_hex4(uint8_t d)
{
    register uint32_t n = d & 0xF;
    n += (n > 9) ? ('A' - 10) : '0';
    uart0_send(n);
}

void uart0_hex8(uint8_t d)
{
    uart0_hex4(d >> 4);
    uart0_hex4(d);
}

void uart0_nhex(void* src, size_t n, const char* sep)
{
    size_t i;
    for (i = 0; i < n; i++) {
        uart0_hex8(*((uint8_t*)src + i));
        if (i != n - 1) uart0_puts((char*)sep);
    }
}

void uart0_rnhex(void* src, size_t n, const char* sep)
{
    size_t i;
    for (i = n; i > 0; i--) {
        uart0_hex8(*((uint8_t*)src + i - 1));
        if (i != 1) uart0_puts((char*)sep);
    }
}

/*
 * Print formatted. NOT YET A COMPLETE IMPLEMENTATION OF PRINTF.
 * TODO:
 * - width sub-specifier
 * - precision sub-specifier
 * - flags related to width & precision (left-justify, left-pad with zeroes)
 * - testing
 *   - is the number returned accurate?
 *   - %n specifier
 */
int uart0_printf(const char* format, ...)
{
    int count;
    va_list ap;

    va_start(ap, format);
    // Call the generic printf function, passing it the argument list and the
    // UART putc function
    count = generic_printf(uart0_putc, format, ap);
    va_end(ap);  // Tidy up the argument list
    return count;
}
