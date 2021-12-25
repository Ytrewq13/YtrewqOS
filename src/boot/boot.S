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
    // GET_EL() shouldn't work in EL0, so an exception would have been
    // generated rather than returning 0 (or does it just give garbage?)
    b       proc_hang

el3_entry:
    // Setup system registers for entry to Exception level 2
    ldr     x2, =SCR_VALUE
    msr     scr_el3, x2
    ldr     x2, =SPSR_EL3_VALUE
    msr     spsr_el3, x2 // Saved Program Status Register
    // Setup the stack pointer for EL2
    mov     x2, sp
    msr     sp_el2, x2
    // Setup the Vector Base Address Register for EL3 (used for exceptions)
    ldr     x2, =Vector_table_el321
    msr     vbar_el3, x2
    // Setup the jump into EL2
    adr     x2, el2_entry
    msr     elr_el3, x2 // Exception return address (Exception Link Register)
    eret // "Return from exception" - jumps to el2_entry and puts CPU into EL2

el2_entry:
    // Setup system registers for entry to Exception level 1
    msr     sp_el1, x1
    // Setup the Vector Base Address Register for EL2 (used for exceptions)
    ldr     x2, =Vector_table_el321
    msr     vbar_el2, x2
    // enable CNTP for EL1
    ldr     x0, =CNTHCTL_EL2_VALUE
    msr     cnthctl_el2, x0
    ldr     x0, =CNTVOFF_EL2_VALUE
    msr     cntvoff_el2, x0
    // enable AArch64 in EL1
    ldr     x0, =HCR_VALUE
    msr     hcr_el2, x0
    mrs     x0, hcr_el2
    // change execution level to EL1
    ldr     x2, =SPSR_EL2_VALUE
    msr     spsr_el2, x2
    // Setup the stack pointer for EL1
    mov     x2, sp
    msr     sp_el1, x2
    // Set the exception return address
    adr     x2, el1_entry // Exception Link Register
    msr     elr_el2, x2
    // Exception return, enter EL1
    eret


el1_entry:
    // Setup the Vector Base Address Register for EL1 (used for exceptions)
    ldr     x2, =Vector_table_el321
    msr     vbar_el1, x2
    // enable Floating point registers (this allows variadic functions to work)
    ldr     x0, =CPACR_EL1_VALUE
    msr     cpacr_el1, x0
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
