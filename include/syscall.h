#ifndef __SYSCALL_H
#define __SYSCALL_H 1

#include "ptrace.h"
#include "asm/syscall.h"

typedef long (*syscall_fn_t)(const struct pt_regs *regs);

const syscall_fn_t sys_call_table[__NR_syscalls];

#endif /* __syscall_h */
