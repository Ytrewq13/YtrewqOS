#ifndef __KERNEL_SHELL_H
#define __KERNEL_SHELL_H

#include "stdlib.h"
#include "printf.h"
#include "errno.h"
#include "string.h"
#include "process/env.h"
#include "drivers/hw/power.h"
#include "fs/util.h"

// Capacity of shell input lines (including null-terminator)
#define SHELL_LINE_CAPACITY 64

// FIXME: Move internal structs to .c files, make static

// struct that contains a command string and a function pointer to execute the
// command
struct shell_command {
    int (*fun)(int argc, char **argv, struct process_env *envp);
    const char *cmd_str;
};

struct shell_cmdlist {
    struct shell_command *cmd;
    struct shell_cmdlist *next;
};

int shell_init();
void shell_destroy();

int shell_getline(struct process_env *env, char *prompt);
int shell_execute_command(struct process_env *env);

#endif /* __kernel_shell_h */
