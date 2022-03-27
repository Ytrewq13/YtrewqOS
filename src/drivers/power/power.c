/* power.c
 * Copyright Sam Whitehead, 2022
 * Last updated 2022-03-27
 */

#include "drivers/hw/power.h"

// Power management
void power_off()
{
    /* https://github.com/bztsrc/raspi3-tutorial/blob/master/08_power/power.c */
#define PM_RSTC         ((volatile unsigned int*)(MMIO_BASE+0x0010001c))
#define PM_RSTS         ((volatile unsigned int*)(MMIO_BASE+0x00100020))
#define PM_WDOG         ((volatile unsigned int*)(MMIO_BASE+0x00100024))
#define PM_WDOG_MAGIC   0x5a000000
#define PM_RSTC_FULLRST 0x00000020
    unsigned long r;
    uint32_t box[8];

    // power off devices one by one
    for(r = 0; r < 16; r++) {
        box[0] = 8*4;
        box[1] = MBOX_REQUEST;
        box[2] = MBOX_TAG_SET_POWER_STATE; // set power state
        box[3] = 8;
        box[4] = 8;
        box[5] = (unsigned int)r;   // device id
        box[6] = 0;                 // bit 0: off, bit 1: no wait
        box[7] = MBOX_TAG_LAST;
        mbox_call_raw((void*)box);
    }

    // power off gpio pins (but not VCC pins)
    *(uint32_t*)GPFSEL0 = 0; *(uint32_t*)GPFSEL1 = 0; *(uint32_t*)GPFSEL2 = 0; *(uint32_t*)GPFSEL3 = 0; *(uint32_t*)GPFSEL4 = 0; *(uint32_t*)GPFSEL5 = 0;
    *(uint32_t*)GPPUD = 0;
//    wait_cycles(150);
    for (int i = 0; i < 150; i++) __asm volatile("nop");
    *(uint32_t*)GPPUDCLK0 = 0xffffffff; *(uint32_t*)GPPUDCLK1 = 0xffffffff;
//    wait_cycles(150);
    for (int i = 0; i < 150; i++) __asm volatile("nop");
    *(uint32_t*)GPPUDCLK0 = 0; *(uint32_t*)GPPUDCLK1 = 0;        // flush GPIO setup

    // power off the SoC (GPU + CPU)
    r = *PM_RSTS; r &= ~0xfffffaaa;
    r |= 0x555;    // partition 63 used to indicate halt
    *PM_RSTS = PM_WDOG_MAGIC | r;
    *PM_WDOG = PM_WDOG_MAGIC | 10;
    *PM_RSTC = PM_WDOG_MAGIC | PM_RSTC_FULLRST;
    while (1) __asm volatile("nop");  // Don't return to the caller
}

