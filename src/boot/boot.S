#include "cpu/sysregs.h"
// To keep this in the first portion of the binary.
.section ".text.boot"

// Make _start global.
.globl _start

_start:
    // read cpu id, stop slave cores
    mrs     x1, mpidr_el1
    and     x1, x1, #3
    cbz     x1, cpu_init
    // cpu id > 0, stop
proc_hang: // If a processor core goes here, it hangs
    wfe
    bl proc_hang
cpu_init:  // cpu id == 0
    // set stack before our code
    ldr     x1, =_start
    mov     sp, x1

cpu_init_check_el:
    // Get the current EL
    bl      GET_EL
    // If we are in EL3 (shouldn't happen)
    cmp     x0, #3
    beq     el3_entry
    // If we are in EL2
    cmp     x0, #2
    beq     el2_entry
    // If we are already in EL1, go to the EL1 entry point
    cmp     x0, #1
    beq     el1_entry
    // If we are in EL0, something is wrong, so we panic/abort
    beq     proc_hang

el3_entry:
    // Setup system registers for entry to Exception level 2
    mov     x2, #0x05b1 // 0b0000 0101 1011 0001
    msr     scr_el3, x2
    mov     x2, #0x03c9 // 0b0000 0011 1100 1001
    msr     spsr_el3, x2 // Saved Program Status Register
    adr     x2, el2_entry
    msr     elr_el3, x2
    eret

el2_entry:
    // Setup system registers for entry to Exception level 1
    msr     sp_el1, x1
    mov     x2, #0x05b1
    // enable CNTP for EL1
    mrs     x0, cnthctl_el2
    orr     x0, x0, #3
    msr     cnthctl_el2, x0
    msr     cntvoff_el2, xzr
    // enable AArch64 in EL1
    mov     x0, #(1 << 31)      // AArch64
    orr     x0, x0, #(1 << 1)   // SWIO hardwired on Pi3
    msr     hcr_el2, x0
    mrs     x0, hcr_el2
    // change execution level to EL1
    mov     x2, #0x3c4
    msr     spsr_el2, x2
    adr     x2, el1_entry
    msr     elr_el2, x2
    eret


el1_entry:
    // clear bss
    ldr     x1, =__bss_start // Start of BSS section
    ldr     w2, =__bss_size // Size of BSS section
    cbz     w2, start_kernel // Skip loop if BSS size is 0
    // Store #0 to address in x1 indexed by #8, then increment x1 by #8
bss_loop:
    str     xzr, [x1], #8
    sub     w2, w2, #1 // Decrement BSS size
    cbnz    w2, bss_loop // Loop if remaining BSS size not 0

    // jump to C code, should not return
start_kernel:
    bl      kernel_main
    // for failsafe, halt this core too
    bl proc_hang


.globl PUT32
// void PUT32(uint64_t, uint32_t);
PUT32:
    str w1,[x0] // Store the value in the second argument at the address in the first argument
    ret

.globl GET32
// uint32_t GET32(uint64_t);
GET32:
    ldr w0,[x0]
    ret


.globl GET_EL
// uint64_t GET_EL(void);
GET_EL:
    mrs x0, CurrentEL
    and x0, x0, #12
    lsr x0, x0, #2
    ret
