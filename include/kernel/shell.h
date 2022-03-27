#ifndef __KERNEL_SHELL_H
#define __KERNEL_SHELL_H

#include "printf.h"
#include "errno.h"
#include "string.h"
#include "process/env.h"
#include "drivers/hw/power.h"

// Capacity of shell input lines (including null-terminator)
#define SHELL_LINE_CAPACITY 64

// struct that contains a command string and a function pointer to execute the
// command
struct shell_command {
    int (*fun)(char*, struct process_env*);
    char *cmd_str;
};

// TODO: function to initialise this list / add commands?
struct shell_cmdlist {
    struct shell_command *cmd;
    struct shell_cmdlist *next;
};

int shell_getline(char *prompt, char *buf, size_t buf_cap);
int shell_execute_command(char *line, struct shell_cmdlist *commands, struct
        process_env *env);

/* Command functions (TODO: add more) */
int shell_cmd_tree(char*, struct process_env*);
int shell_cmd_shutdown(char*, struct process_env*);
int shell_cmd_ls(char*, struct process_env*); // TODO

#endif /* __kernel_shell_h */
