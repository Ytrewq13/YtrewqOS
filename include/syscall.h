#ifndef __SYSCALL_H
#define __SYSCALL_H 1

#include "ptrace.h"

#define __NR_syscalls 443

typedef long (*syscall_fn_t)(const struct pt_regs *regs);

const syscall_fn_t sys_call_table[__NR_syscalls];

#endif /* __syscall_h */
