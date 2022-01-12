#ifndef __PTRACE_H
#define __PTRACE_H 1

#include <stdint.h>

/* From linux/arch/arm64/include/asm/ptrace.h */
struct pt_regs {
    struct {
        uint64_t regs[31];
        uint64_t sp;
        uint64_t pc;
        uint64_t pstate;
    };
    uint64_t orig_x0;
#ifdef __AARCH64EB__
    uint32_t unused2;
    int32_t syscallno;
#else
    int32_t syscallno;
    uint32_t unused2;
#endif
    uint64_t sdei_ttbr1;
    /* Only valid when ARM64_HAS_IRQ_PRIO_MASKING is enabled. */
    uint64_t pmr_save;
    uint64_t stackframe[2];

    /* Only valid for some EL1 exceptions. */
    uint64_t lockdep_hardirqs;
    uint64_t exit_rcu;
};

#endif /* __ptrace_h */
