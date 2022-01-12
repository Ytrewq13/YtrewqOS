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
    ldr x8, =sys_call_table
    add x9, x8, x0
    ldr x10, [x9, #6]
    // Call the function
    blr x10
//     bl syscall_hello
    eret

.globl syscall
// int system_call(long nr, ...);
// up to 6 args in x1 to x6, syscall nr in x0
syscall:
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
    ret
