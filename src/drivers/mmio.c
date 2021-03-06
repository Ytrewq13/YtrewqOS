#include "drivers/hw/mmio.h"

void mmio_write(uintptr_t reg, uint32_t data)
{
    *(volatile uint32_t *)(reg + MMIO_BASE) = data;
}

uint32_t mmio_read(uintptr_t reg)
{
    return *(volatile uint32_t *)(reg + MMIO_BASE);
}
