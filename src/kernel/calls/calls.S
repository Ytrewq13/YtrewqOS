#include "asm/syscalls.h"
#include "asm/errno.h"

.extern errno

.section ".text"
.globl sys_call_handler
// up to 6 args in x0 to x5, syscall nr in x8
sys_call_handler:
    // Move syscall args from x0..x5 to x1..x6
    mov x6, x5
    mov x5, x4
    mov x4, x3
    mov x3, x2
    mov x2, x1
    mov x1, x0
    // Move syscall nr to x0
    mov x0, x8
    // Get the function pointer for the given syscall function
    lsl x8, x0, #3  // offset = NR * 8
    ldr x9, =sys_call_table
    add x10, x9, x8
    ldr x11, [x10]  // fn = sys_call_table[NR]
    // Call the function
    blr x11
    eret

.globl syscall
// int system_call(long nr, ...);
// up to 6 args in x1 to x6, syscall nr in x0
syscall:
    mov x9, __NR_syscalls
    cmp x0, x9
    bgt __syscall_nr_einval
    cmp x0, #0
    blt __syscall_nr_einval
    // Move nr to x8
    mov x8, x0
    // Move x1..x6 to x0..x5 (same convention as Linux)
    mov x0, x1
    mov x1, x2
    mov x2, x3
    mov x3, x4
    mov x4, x5
    mov x5, x6
    // Save the return address
    sub sp, sp, #0x10
    stp x30, xzr, [sp, #0]
    // Generate an interrupt
    svc #0
    ldp x30, xzr, [sp, #0]  // x30 is the Link Register (used by ret)
    add sp, sp, #0x10
    b __syscall_ret
__syscall_nr_einval:
    // Put EINVAL into errno
    mov x10, EINVAL
    ldr x11, =errno
    str x10, [x11]
    mov x0, #-1
__syscall_ret:
    ret
