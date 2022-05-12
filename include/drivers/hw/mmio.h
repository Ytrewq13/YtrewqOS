#ifndef MMIO_H
#define MMIO_H 1

#include <stdint.h>

enum {
    // Base of MMIO
    MMIO_BASE = 0x3F000000
};

void mmio_write(uintptr_t reg, uint32_t data);
uint32_t mmio_read(uintptr_t reg);

#endif
