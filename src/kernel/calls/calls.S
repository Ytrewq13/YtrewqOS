.section ".text"
.globl syscall
// up to 6 args in x0 to x5, syscall nr in x8
syscall:
    // Move syscall args from x0..x5 to x1..x6
    mov x6, x5
    mov x5, x4
    mov x4, x3
    mov x3, x2
    mov x2, x1
    mov x1, x0
    // Move syscall nr to x0
    mov x0, x8
    // Syscall NR in x8
    // TODO: Lookup the sys_### function defined in include/sys/unistd.h
    //          - how to do this?
    //          - Need an area of memory to store a table of system call NR and
    //            function pointers
    // TODO: Call the function
    bl syscall_hello
    eret

.globl system_call
// int system_call(long nr, ...);
// up to 6 args in x1 to x6, syscall nr in x0
system_call:
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
