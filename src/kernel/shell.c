/* shell.c
 * Copyright Sam Whitehead, 2022
 * Last updated 2022-03-23
 */

#include "kernel/shell.h"

/* TODO: implement some input-line editing (i.e. backspace) */
int shell_getline(char *prompt, char *buf, size_t buf_cap)
{
    if (prompt == NULL || buf == NULL) {
        errno = EINVAL;
        return -1;
    }
    // Print the prompt
    printf("%s", prompt);
    // Read the input
    for (size_t i = 0; i < buf_cap; i++) {
        // Read a character
        char c = uart0_getc();
        // If the character was a newline, put a null-terminator and return
        if (c == '\n') {
            buf[i] = 0;
            uart0_putc(c);
            console_putc(c);
            return 0;
        }
        // Put the character into the buffer
        buf[i] = c;
        uart0_putc(c);
        console_putc(c);
    }
    // We ran out of space if we arrived here
    errno = E2BIG;
    return -1;
}

int shell_execute_command(char *line, struct shell_cmdlist *commands, struct
        process_env *env)
{
    // Allow blank lines
    if (strlen(line) == 0) return 0;

    // Iterate through the list of available commands
    struct shell_cmdlist *cmds = commands;
    while (cmds != NULL) {
        // TODO: use strstr() or similar to detect if the command string is a
        // prefix substring of the line
        if (cmds->cmd != NULL && strcmp(line, cmds->cmd->cmd_str) == 0) {
            return cmds->cmd->fun(line, env);
        }
        cmds = cmds->next;
    }
    // No command was matched
    printf("Error: '%s' does not match any known commands\n", line);
    return 0;
}



int shell_cmd_tree(char *line, struct process_env *env)
{
    printf("TODO: Implement a wrapper for tree\n");
    errno = ENOSYS;
    return -1;
}

int shell_cmd_shutdown(char *line, struct process_env *env)
{
    power_off();
    errno = EPERM;  // FIXME: this may not be a permission error
    return -1;  // Power_off has always failed if we continue to execute code
}

int shell_cmd_ls(char *line, struct process_env *env)
{
    printf("ERROR: this command is not yet implemented!\n");
    errno = ENOSYS;
    return -1;
}
