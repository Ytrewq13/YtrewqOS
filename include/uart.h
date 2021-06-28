#ifndef UART_H
#define UART_H 1

#include <stdint.h>
#include <stddef.h>

#include "mmio.h"

enum UART1_REGS {
    UART1_BASE      = MMIO_BASE | 0x00215000,

    AUX_ENABLE      = UART1_BASE | 0x04,
    // Mini UART (UART1)
    AUX_MU_IO       = UART1_BASE | 0x40,    // IO Register
    AUX_MU_IER      = UART1_BASE | 0x44,    // Interrupt Enable Register
    AUX_MU_IIR      = UART1_BASE | 0x48,    // Interrupt Identify Register
    AUX_MU_LCR      = UART1_BASE | 0x4C,    // Line Control Register
    AUX_MU_MCR      = UART1_BASE | 0x50,    // Modem Control Register
    AUX_MU_LSR      = UART1_BASE | 0x54,    // Line Status Register
    AUX_MU_MSR      = UART1_BASE | 0x58,    // Modem Status Register
    AUX_MU_SCRATCH  = UART1_BASE | 0x5C,    // Single Byte of storage (LSB of this address)
    AUX_MU_CNTL     = UART1_BASE | 0x60,    // Control for extra features not found on 16650 UART
    AUX_MU_STAT     = UART1_BASE | 0x64,    // Internal status of the UART (extra information not found on 16650 UART)
    AUX_MU_BAUD     = UART1_BASE | 0x68,    // UART Baudrate (bits 15:0)
};

void uart_init();

void uart_send(uint32_t c);
void uart_putc(uint32_t c);
char uart_getc();
void uart_puts(char* s);
void uart_putls(wchar_t* s); // Wide character string

void uart_dec(uint64_t d);
void uart_oct(uintmax_t u);
void uart_hex4(uint8_t d);
void uart_hex8(uint8_t d);
void uart_hex16(uint16_t d);
void uart_hex32(uint32_t d);
void uart_hex64(uint64_t d);

// Print n bytes in hex from starting address src, with a separator string between each byte printed
void uart_nhex(void *src, size_t n, const char *sep);
// Reverse (for little endian numbers without a dedicated uart_hex function.
void uart_rnhex(void *src, size_t n, const char *sep);

// TODO: uart_printf
void uart_printf(char *s, ...);
int uart_printf2(const char* format, ...);

#endif
