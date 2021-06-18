/* kernel.c
 * Copyright Sam Whitehead, 2021
 * Last updated 2021-06-18
 */
#include <stdint.h>

#include "uart.h"
#include "mbox.h"

extern void PUT32(uint64_t addr, uint32_t x);
extern uint32_t GET32(uint64_t addr);

void kernel_main() {
    uint32_t err;
    uint64_t serial_num;

    // Set up serial console
    uart_init();
    // Say hello
    uart_puts("Hello World!\n");

    serial_num = get_serial_number(&err);
    if (err)
        uart_puts("Unable to query serial!\n");
    else {
        uart_puts("My serial number is: ");
        uart_hex64(serial_num);
        uart_puts("\n");
    }

    // echo everything back
    while (1)
        uart_send(uart_getc());
}

/* TODO:
 * - Load important values from the GPU using mailbox calls and store them in memory
 * - Memory
 * - System calls?
 */
