#include <stdint.h>
#include <stdarg.h>

#include "drivers/hw/uart.h"

uint64_t syscall_hello(uint64_t nr, uint64_t x1, uint64_t x2, uint64_t x3, uint64_t x4, uint64_t x5, uint64_t x6)
{
    uart0_printf("syscall_hello args:\n");
    uart0_printf("nr:   %#lx\n", nr);
    uart0_printf("arg1: %#lx\n", x1);
    uart0_printf("arg2: %#lx\n", x2);
    uart0_printf("arg3: %#lx\n", x3);
    uart0_printf("arg4: %#lx\n", x4);
    uart0_printf("arg5: %#lx\n", x5);
    uart0_printf("arg6: %#lx\n", x6);
    return x1 + x2 + x3 + x4 + x5 + x6;  // 0x34 + 0x56 + 0x78 + 0x9a + 0xbc + 0xde == 822
}
