#ifndef UART_H
#define UART_H 1

#include <stddef.h>
#include <stdint.h>

#include "mmio.h"

// PL011 (Hardware) UART
enum UART0_REGS {
    UART0_BASE = MMIO_BASE | 0x00201000,

    UART0_DR        = UART0_BASE | 0x00, // Data Register
    UART0_RSRECR    = UART0_BASE | 0x04,
    UART0_FR        = UART0_BASE | 0x18, // Flags Register
    UART0_ILPR      = UART0_BASE | 0x20, // Not in Use
    UART0_IBRD      = UART0_BASE | 0x24, // Integer Baud Rate Divisor
    UART0_FBRD      = UART0_BASE | 0x28, // Fractional Baud Rate Divisor
    UART0_LCRH      = UART0_BASE | 0x2C, // Line Control Register
    UART0_CR        = UART0_BASE | 0x30, // Control Register
    UART0_IFLS      = UART0_BASE | 0x34, // Interrupt FIFO Level Select Register
    UART0_IMSC      = UART0_BASE | 0x38, // Interrupt Mask Set Clear Register
    UART0_RIS       = UART0_BASE | 0x3C, // Raw Interrupt Status Register
    UART0_MIS       = UART0_BASE | 0x40, // Masked Interrupt Status Register
    UART0_ICR       = UART0_BASE | 0x44, // Interrupt Clear Register
    UART0_DMACR     = UART0_BASE | 0x48, // DMA Control Register
    UART0_ITCR      = UART0_BASE | 0x80, // Test Control Register
    UART0_ITIP      = UART0_BASE | 0x84, // Integration Test Input Register
    UART0_ITOP      = UART0_BASE | 0x88, // Integration Test Output Register
    UART0_TDR       = UART0_BASE | 0x8C, // Test Data Register
};

void uart0_init();

void uart0_send(uint32_t c);
void uart0_putc(uint32_t c);
char uart0_getc();
void uart0_puts(char* s);

// Print n bytes in hex from starting address src, with a separator string
// between each byte printed
void uart0_nhex(void* src, size_t n, const char* sep);
// Reverse (for little endian numbers without a dedicated uart_hex function.
void uart0_rnhex(void* src, size_t n, const char* sep);

int uart0_printf(const char* format, ...);


/******************************************************************************/

// miniUART
enum UART1_REGS {
    UART1_BASE = MMIO_BASE | 0x00215000,

    AUX_ENABLE = UART1_BASE | 0x04,
    // Mini UART (UART1)
    AUX_MU_IO  = UART1_BASE | 0x40,  // IO Register
    AUX_MU_IER = UART1_BASE | 0x44,  // Interrupt Enable Register
    AUX_MU_IIR = UART1_BASE | 0x48,  // Interrupt Identify Register
    AUX_MU_LCR = UART1_BASE | 0x4C,  // Line Control Register
    AUX_MU_MCR = UART1_BASE | 0x50,  // Modem Control Register
    AUX_MU_LSR = UART1_BASE | 0x54,  // Line Status Register
    AUX_MU_MSR = UART1_BASE | 0x58,  // Modem Status Register
    AUX_MU_SCRATCH =
        UART1_BASE | 0x5C,  // Single Byte of storage (LSB of this address)
    AUX_MU_CNTL = UART1_BASE |
                  0x60,  // Control for extra features not found on 16650 UART
    AUX_MU_STAT = UART1_BASE | 0x64,  // Internal status of the UART (extra
                                      // information not found on 16650 UART)
    AUX_MU_BAUD = UART1_BASE | 0x68,  // UART Baudrate (bits 15:0)
};

void uart1_init();

void uart1_send(uint32_t c);
void uart1_putc(uint32_t c);
char uart1_getc();
void uart1_puts(char* s);
void uart1_putls(wchar_t* s);  // Wide character string

void uart1_dec(uint64_t d);
void uart1_oct(uintmax_t u);
void uart1_hex4(uint8_t d);
void uart1_hex8(uint8_t d);
void uart1_hex16(uint16_t d);
void uart1_hex32(uint32_t d);
void uart1_hex64(uint64_t d);

// Print n bytes in hex from starting address src, with a separator string
// between each byte printed
void uart1_nhex(void* src, size_t n, const char* sep);
// Reverse (for little endian numbers without a dedicated uart_hex function.
void uart1_rnhex(void* src, size_t n, const char* sep);

int uart1_printf(const char* format, ...);

#endif
