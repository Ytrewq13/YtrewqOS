/* kernel.c
 * Copyright Sam Whitehead, 2021
 * Last updated 2021-05-07
 */
//#include <stdint.h>

#include "uart.h"

extern void PUT32(unsigned int addr, unsigned int x);
extern unsigned int GET32(unsigned int addr);

void kernel_main() {
    // Set up serial console
    uart_init();
    // Say hello
    uart_puts("Hello World!\n");
    // echo everything back
    while (1)
        uart_send(uart_getc());
}

/*
 * 115200 = SYSTEM_CLK_FREQ / (8 * (BAUD_REG + 1))
 * 115200 = 1200000000 / (8 * (BAUD_REG + 1))
 * 8 * (BAUD_REG + 1) = 1200000000 / 115200
 * BAUD_REG + 1 = 1200000000 / 115200 / 8
 * BAUD_REG = 1200000000 / 115200 / 8 - 1 = ~1301
 */
