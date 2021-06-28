/* uart.c
 * Copyright Sam Whitehead, 2021
 * Last updated 2021-06-16
 */

#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>

#include "gpio.h"
#include "uart.h"
#include "io/printf.h"

extern void PUT32(uint64_t addr, uint32_t x);
extern uint32_t GET32(uint64_t addr);

/* The following is taken from example code found at
 * https://github.com/bztsrc/raspi3-tutorial/blob/master/03_uart1/uart.c
 * Modified slightly to use our enums instead of '#define's and the GET32()
 * and PUT32() functions defined in our boot.S file.
 */

void uart_init() {
    register uint32_t r;

    /* initialize UART */
    PUT32(AUX_ENABLE, GET32(AUX_ENABLE)|1); // Enable UART1, AUX mini uart
    PUT32(AUX_MU_CNTL, 0);
    PUT32(AUX_MU_LCR, 3);       // 8 bits
    PUT32(AUX_MU_MCR, 0);
    PUT32(AUX_MU_IER, 0);
    PUT32(AUX_MU_IIR, 0xC6);    // Disable interrupts
    PUT32(AUX_MU_BAUD, 270);    // 115200 baud
    /* map UART1 to GPIO pins */
    r = GET32(GPFSEL1);
    r &= ~((7<<12)|(7<<15));    // GPIO14, GPIO15
    r |= (2<<12)|(2<<15);       // alt5
    PUT32(GPFSEL1, r);
    PUT32(GPPUD, 0);            // Enable pins 14 and 15
    r = 150; while(r--) __asm volatile("nop"); // Delay 150
    PUT32(GPPUDCLK0, (1<<14)|(1<<15));
    r = 150; while(r--) __asm volatile("nop"); // Delay 150
    PUT32(GPPUDCLK0, 0);        // flush GPIO setup
    PUT32(AUX_MU_CNTL, 3);      // eable Tx, Rx
}

void uart_send(uint32_t c) {
    // Wait until we can send
    while (!(GET32(AUX_MU_LSR) & 0x20)) __asm volatile("nop");
    // Write the character to the buffer
    PUT32(AUX_MU_IO, c);
}

// TODO: uart_putc - a wrapper around uart_send that includes the check for '\n'
void uart_putc(uint32_t c) {
    if (c == '\n') uart_send('\r');
    uart_send(c);
}

char uart_getc() {
    char r;
    // Wait until something is in the buffer
    while(!(GET32(AUX_MU_LSR) & 0x01)) __asm volatile("nop");
    // Read it and return
    r = GET32(AUX_MU_IO);
    // Convert carriage return to newline
    return (r == '\r' ? '\n' : r);
}

void uart_puts(char* s) {
    if (!s) return;
    while (*s) {
        if (*s == '\n')
            uart_send('\r');
        uart_send(*s++);
    }
}

// Put a wide character string
void uart_putls(wchar_t* s) {
    // TODO
}

// Print a number as decimal on the UART
void uart_dec(uint64_t d) {
    // Put previous digits
    if (d > 9) uart_dec(d/10);
    // Put the final digit
    uart_send('0' + (d % 10));
}

void uart_oct(uintmax_t u) {
    // TODO
}

void uart_hex4(uint8_t d) {
    register uint32_t n = d & 0xF;
    n += (n > 9) ? ('A' - 10) : '0';
    uart_send(n);
}

void uart_hex8(uint8_t d) {
    uart_hex4(d >> 4);
    uart_hex4(d);
}

void uart_hex16(uint16_t d) {
    uart_hex8(d >> 8);
    uart_hex8(d);
}

void uart_hex32(uint32_t d) {
    uart_hex16(d >> 16);
    uart_hex16(d);
}

void uart_hex64(uint64_t d) {
    uart_hex32(d >> 32);
    uart_hex32(d);
}

void uart_nhex(void *src, size_t n, const char *sep) {
    size_t i;
    for (i = 0; i < n; i++) {
        uart_hex8(*((uint8_t*)src+i));
        if (i != n-1) uart_puts((char*)sep);
    }
}

void uart_rnhex(void *src, size_t n, const char *sep) {
    size_t i;
    for (i = n; i > 0; i--) {
        uart_hex8(*((uint8_t*)src+i-1));
        if (i != 1) uart_puts((char*)sep);
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
int uart_printf(const char* format, ...) {
    int count;
    va_list ap;

    va_start(ap, format);
    // Call the generic printf function, passing it the argument list and the
    // UART putc function
    count = generic_printf(uart_putc, format, ap);
    va_end(ap); // Tidy up the argument list
    return count;
}


