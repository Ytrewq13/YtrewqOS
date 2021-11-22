/* The code in this file was taken from
 * https://github.com/s-matyukevich/raspberry-pi-os/blob/master/src/lesson02/include/arm/sysregs.h
 */

#ifndef _SYSREGS_H
#define _SYSREGS_H

#ifdef BIT
#undef BIT
#endif
#define BIT(n) (1 << (n))

// ***************************************
// CNTHCTL_EL2, Counter-timer Hypervisor Control register
// ***************************************

#define CNTHCTL_TIMER_ENABLE    (BIT(1) | BIT(0))
#define CNTHCTL_EL2_VALUE       (CNTHCTL_TIMER_ENABLE)

// ***************************************
// CNTVOFF_EL2, Counter-timer Virtual Offset register
// ***************************************

#define CNTVOFF_EL2_VALUE   (0)

// ***************************************
// CPACR_EL1, Architectural Feature Access Control Register (EL1)
// ***************************************

#define CPACR_FPEN_EL1  (BIT(20))
#define CPACR_FPEN_ALL  (BIT(21) | BIT(20))
#define CPACR_EL1_VALUE (CPACR_FPEN_ALL)

// ***************************************
// SCTLR_EL1, System Control Register (EL1), Page 2654 of AArch64-Reference-Manual.
// ***************************************

#define SCTLR_RESERVED                  (3 << 28) | (3 << 22) | (1 << 20) | (1 << 11)
#define SCTLR_EE_LITTLE_ENDIAN          (0 << 25)
#define SCTLR_EOE_LITTLE_ENDIAN         (0 << 24)
#define SCTLR_I_CACHE_DISABLED          (0 << 12)
#define SCTLR_D_CACHE_DISABLED          (0 << 2)
#define SCTLR_MMU_DISABLED              (0 << 0)
#define SCTLR_MMU_ENABLED               (1 << 0)

#define SCTLR_VALUE_MMU_DISABLED    (SCTLR_RESERVED | SCTLR_EE_LITTLE_ENDIAN | SCTLR_I_CACHE_DISABLED | SCTLR_D_CACHE_DISABLED | SCTLR_MMU_DISABLED)

// ***************************************
// HCR_EL2, Hypervisor Configuration Register (EL2), Page 2487 of AArch64-Reference-Manual.
// ***************************************

#define HCR_AARCH64 (BIT(31))
#define HCR_SWIO    (BIT(1))
#define HCR_VALUE   (HCR_AARCH64 | HCR_SWIO)

// ***************************************
// SCR_EL3, Secure Configuration Register (EL3), Page 2648 of AArch64-Reference-Manual.
// ***************************************

#define SCR_RW          (BIT(10))
#define SCR_HVC_ENABLE  (BIT(8))
#define SCR_SMC_DISABLE (BIT(7))
#define SCR_RESERVED    (BIT(5) | BIT(4)) // RES1 bits
#define SCR_NS          (BIT(0))
#define SCR_VALUE       (SCR_RW | SCR_HVC_ENABLE | SCR_SMC_DISABLE | SCR_RESERVED | SCR_NS)
// 0x5b1
// 0101 1011 0001

// ***************************************
// SPSR_EL3, Saved Program Status Register (EL3) Page 389 of AArch64-Reference-Manual.
// SPSR_EL2, Saved Program Status Register (EL2)
// ***************************************

#define SPSR_MASK_DBG   (BIT(9))
#define SPSR_MASK_SERR  (BIT(8))
#define SPSR_MASK_IRQ   (BIT(7))
#define SPSR_MASK_FIQ   (BIT(6))
#define SPSR_MASK_ALL   (SPSR_MASK_DBG | SPSR_MASK_SERR | SPSR_MASK_IRQ | SPSR_MASK_FIQ)
// 't' = thread, 'h' = handler
#define SPSR_EL0t       (0)
#define SPSR_EL1t       (BIT(2))
#define SPSR_EL1h       (BIT(2) | BIT(0))
#define SPSR_EL2t       (BIT(3))
#define SPSR_EL2h       (BIT(3) | BIT(0))
#define SPSR_EL3t       (BIT(3) | BIT(2))
#define SPSR_EL3h       (BIT(3) | BIT(2) | BIT(0))
#define SPSR_EL3_VALUE  (SPSR_MASK_ALL | SPSR_EL2h)
// 0x3c9
// 0011 1100 1001
#define SPSR_EL2_VALUE  (SPSR_MASK_ALL | SPSR_EL1t)
// 0x3c4
// 0011 1100 0100

#endif
