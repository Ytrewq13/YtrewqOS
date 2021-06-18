#ifndef GPIO_H
#define GPIO_H

#include "mmio.h"

enum GPIO_REGS {
    // Base of GPIO registers
    GPIO_BASE = MMIO_BASE | 0x200000,

    // GPIO Function Select
    GPFSEL0     = GPIO_BASE | 0x00, // Bits 29:0 = Pins 9:0
    GPFSEL1     = GPIO_BASE | 0x04, // Bits 29:0 = Pins 19:10
    GPFSEL2     = GPIO_BASE | 0x08, // Bits 29:0 = Pins 29:20
    GPFSEL3     = GPIO_BASE | 0x0C, // Bits 29:0 = Pins 39:30
    GPFSEL4     = GPIO_BASE | 0x10, // Bits 29:0 = Pins 49:40
    GPFSEL5     = GPIO_BASE | 0x14, // Bits 11:0 = Pins 53:50

    // GPIO Pin Output Set (Write-only)
    GPSET0      = GPIO_BASE | 0x1C,
    GPSET1      = GPIO_BASE | 0x20,
    // GPIO Pin Output Clear (Write-only)
    GPCLR0      = GPIO_BASE | 0x28,
    GPCLR1      = GPIO_BASE | 0x2C,
    // GPIO Pin Level (Read-only)
    GPLEV0      = GPIO_BASE | 0x34,
    GPLEV1      = GPIO_BASE | 0x38,

    // GPIO Pin Event Detect Status
    GPEDS0      = GPIO_BASE | 0x40,
    GPEDS1      = GPIO_BASE | 0x44,
    // GPIO Pin Rising Edge Detect Enable
    GPREN0      = GPIO_BASE | 0x4C,
    GPREN1      = GPIO_BASE | 0x50,
    // GPIO Pin Falling Edge Detect Enable
    GPFEN0      = GPIO_BASE | 0x58,
    GPFEN1      = GPIO_BASE | 0x5C,
    // GPIO Pin High Detect Enable
    GPHEN0      = GPIO_BASE | 0x64,
    GPHEN1      = GPIO_BASE | 0x68,
    // GPIO Pin Low Detect Enable
    GPLEN0      = GPIO_BASE | 0x70,
    GPLEN1      = GPIO_BASE | 0x74,
    // GPIO Pin Async. Rising Edge Detect
    GPAREN0     = GPIO_BASE | 0x7C,
    GPAREN1     = GPIO_BASE | 0x80,
    // GPIO Pin Async. Falling Edge Detect
    GPALEN0     = GPIO_BASE | 0x88,
    GPALEN1     = GPIO_BASE | 0x8C,

    // GPIO Pin Pull-up/down Enable
    GPPUD       = GPIO_BASE | 0x94,
    // GPIO Pin Pull-up/down Enable Clock
    GPPUDCLK0   = GPIO_BASE | 0x98,
    GPPUDCLK1   = GPIO_BASE | 0x9C
};

#endif
