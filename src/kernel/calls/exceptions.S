// The linker puts this section in the correct position
// (the correct position should be 0x0)
// We only get 32 bits for each entry in the exception table
// TODO: exception vector tables are more complicated than this - there are
// different ecxeptions for different ELs and also for synchronous vs.
// asynchronous, etc.
.section ".text.exceptions"
interrupt_reset:
//     bl interrupt_reset // TODO: Reset
    bl syscall
interrupt_undefined_instruction:
//     bl interrupt_undefined_instruction // TODO: Undefined instructions
    bl syscall
interrupt_sw: // TODO: This may not be a syscall - could be SVC n, syscall is only SVC #0
    bl syscall // Branch to the syscall function
interrupt_prefetch_abort:
//     bl interrupt_prefetch_abort // TODO: Prefetch abort
    bl syscall
interrupt_data_abort:
//     bl interrupt_data_abort // TODO: Data abort
    bl syscall
interrupt_6:
//     nop // Reserved
    bl syscall
interrupt_irq:
//     bl interrupt_irq // TODO: IRQ - Interrupt
    bl syscall
interrupt_fiq:
//     bl interrupt_fiq // TODO: FIQ - Fast Interrupt
    bl syscall
