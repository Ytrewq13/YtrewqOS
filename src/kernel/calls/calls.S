// TODO: move this exceptions table to a file "exceptions.S"
// The linker puts this section in the correct position
// (the correct position should be 0x0)
// We only get 32 bits for each entry in the exception table
.section ".text.exceptions"
interrupt_reset:
    bl interrupt_reset // TODO: Reset
interrupt_undefined_instruction:
    bl interrupt_undefined_instruction // TODO: Undefined instructions
interrupt_sw: // TODO: This may not be a syscall - could be SVC n, syscall is only SVC #0
    bl syscall // Branch to the syscall function
interrupt_prefetch_abort:
    bl interrupt_prefetch_abort // TODO: Prefetch abort
interrupt_data_abort:
    bl interrupt_data_abort // TODO: Data abort
interrupt_6:
    nop // Reserved
interrupt_irq:
    bl interrupt_irq // TODO: IRQ - Interrupt
interrupt_fiq:
    bl interrupt_fiq // TODO: FIQ - Fast Interrupt

.section ".text"
.globl syscall
// int syscall(long nr, ...);
// up to 6 args in x0 to x5, syscall nr in x8
syscall:
    // Syscall NR in x8
    // TODO: Lookup the sys_### function defined in include/sys/unistd.h
    //          - how to do this?
    // TODO: Call the function
    bl syscall

// RiscOS - first OS for ARM - easier to follow than Linux
// NetBSD - cleaner code than Linux
