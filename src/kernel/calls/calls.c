#include <stdint.h>
#include <stdarg.h>
#include "hw/uart.h"

void syscall_hello()
{
    uart0_printf("Hello from a system call!\n");
    while (1) __asm volatile("nop");
}
