.section ".text"
.globl syscall
// up to 6 args in x0 to x5, syscall nr in x8
syscall:
    // Syscall NR in x8
    // TODO: Lookup the sys_### function defined in include/sys/unistd.h
    //          - how to do this?
    //          - Need an area of memory to store a table of system call NR and
    //            function pointers
    // TODO: Call the function
    bl syscall_hello

.globl system_call
// int system_call(long nr, ...);
// up to 6 args in x1 to x6, syscall nr in x0
system_call:
    // Move nr to x8
//     mov x8, x0
    // Move x1..x6 to x0..x5 (same convention as Linux)
//     mov x0, x1
//     mov x1, x2
//     mov x2, x3
//     mov x3, x4
//     mov x4, x5
//     mov x5, x6
    // Generate an interrupt
    svc #0

// RiscOS - first OS for ARM - easier to follow than Linux
// NetBSD - cleaner code than Linux
