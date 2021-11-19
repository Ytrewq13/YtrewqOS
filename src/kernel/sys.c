
#define __SYSCALL(nr, sym)    long __arm64_##sym(const struct pt_regs *);
#include "unistd.h"
#undef __SYSCALL
