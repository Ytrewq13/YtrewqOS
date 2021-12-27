// The linker puts this section in the correct position
// (the address in VBAR_EL1 - __exceptions_start)
// This address is aligned to 2048 bytes
// We get 128 bytes (32 instructions) for each entry in the exception table
.balign 0x800
.section ".text.exceptions"
.globl Vector_table_el321
Vector_table_el321:
curr_el_sp0_sync:
// The exception handler for a synchronous exception from the current EL using
// SP0.
    b .
.balign 0x80
curr_el_sp0_irq:
// The exception handler for an IRQ exception from the current EL using SP0.
    b .
.balign 0x80
curr_el_sp0_fiq:
// The exception handler for an FIQ exception from the current EL using SP0.
    b .
.balign 0x80
curr_el_sp0_serror:
// The exception handler for a System Error exception from the current EL using
// SP0.
    b .
.balign 0x80
curr_el_spx_sync:
// The exception handler for a synchronous exception from the current EL using
// the current SP.
// THIS COULD BE A KERNEL SYSCALL (FROM EL1)
// Syscall args are in x0..x5 and x8, so we can use x6,x7
    mrs x6, esr_el1  // Get the Exception Syndrome Register
    // Check if the instruction that caused the interrupt was SVC
    lsr x7, x6, #26  // Shift right (ESR_EL1 >> 26)
    and x7, x7, #0x3f  // Mask only the bits we want ((ESR_EL1 >> 26) & 0x3f)
    cmp x7, #0x15  // 0x15 is the EC for SVC
    bne _curr_el_spx_sync_ret  // FIXME: Branch to a different handler if not SVC
    // Check if the instruction was `svc #0` - this is a syscall
    // The imm16 argument of SVC instruction is placed in ESR_EL1[15:0]
    and x7, x6, #0xffff  // Mask bits [15:0]
    cmp x7, #0
    bne _curr_el_spx_sync_ret  // FIXME: Branch to a different handler if not syscall
    bl syscall
_curr_el_spx_sync_ret:
    eret
.balign 0x80
curr_el_spx_irq:
// TODO: The exception handler for an IRQ exception from the current EL using
// the current SP.
    b .
.balign 0x80
curr_el_spx_fiq:
// TODO: The exception handler for an FIQ from the current EL using the current
// SP.
    b .
.balign 0x80
curr_el_spx_serror:
// TODO: The exception handler for a System Error exception from the current EL
// using the current SP.
    b .
.balign 0x80
lower_el_aarch64_sync:
// TODO: The exception handler for a synchronous exception from a lower EL
// (AArch64).
// THIS COULD BE A SYSCALL FROM EL0
    b .
.balign 0x80
lower_el_aarch64_irq:
// TODO: The exception handler for an IRQ from a lower EL (AArch64).
    b .
.balign 0x80
lower_el_aarch64_fiq:
// TODO: The exception handler for an FIQ from a lower EL (AArch64).
    b .
.balign 0x80
lower_el_aarch64_serror:
// TODO: The exception handler for a System Error exception from a lower EL
// (AArch64).
    b .
.balign 0x80
// The Aarch32 exceptions should never be needed
lower_el_aarch32_sync:
// The exception handler for a synchronous exception from a lower EL (AArch32).
    b .
.balign 0x80
lower_el_aarch32_irq:
// The exception handler for an IRQ exception from a lower EL (AArch32).
    b .
.balign 0x80
lower_el_aarch32_fiq:
// The exception handler for an FIQ exception from a lower EL (AArch32).
    b .
.balign 0x80
lower_el_aarch32_serror:
// The exception handler for a System Error exception from a lower EL (AArch32).
    b .

// For system calls (SVC #0)
// ---------------------------
//
// x8 contains the syscall nr
// x0..x5 contain syscall arguments
//
// The register ESR_ELn contains:
// - Bits 31:26  - Exception Class (101111 = SError, 0x15 = 0b10101 = SVC)
// - Bit 25  - Length of trapped instruction (0 = 16 bits, 1 = 32 bits) - Only set for certain exception classes
// - Bits 24:16  - RES0
// - Bits 15:0  - Imm16 (the argument of the SVC instruction)
//
// So:
// if ((ESR_EL1[31:26] == 0x15) && (ESR_EL1[15:0] == 0x0))
//      SYSTEM_CALL()
