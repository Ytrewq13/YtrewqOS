#include "syscall.h"

long sys_ni_syscall(void)
{
    return 0;
}

long __arm64_sys_ni_syscall(const struct pt_regs *__unused)
{
    return sys_ni_syscall();
}

#undef __SYSCALL
// Declare the syscall functions
#define __SYSCALL(nr, sym)  long __arm64_##sym(const struct pt_regs *);
#include "unistd.h"
#undef __SYSCALL

#define __SYSCALL(nr, sym)  [nr] = __arm64_##sym,

const syscall_fn_t __attribute__((aligned(4))) sys_call_table[__NR_syscalls] = {
//    [0 ... __NR_syscalls - 1] = __arm64_sys_ni_syscall,
#include "unistd.h"
};
