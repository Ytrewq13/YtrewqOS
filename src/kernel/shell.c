/* shell.c
 * Copyright Sam Whitehead, 2022
 * Last updated 2022-03-23
 */

#include "kernel/shell.h"

static char shell_input_buf[SHELL_LINE_CAPACITY];
static struct shell_cmdlist *shell_all_cmds = NULL;

int shell_cmd_tree(struct command_args);
int shell_cmd_shutdown(struct command_args);
int shell_cmd_ls(struct command_args); // TODO
int shell_cmd_pwd(struct command_args);
int shell_cmd_cd(struct command_args);
int shell_cmd_cat(struct command_args);

/* TODO: implement some input-line editing (i.e. backspace) - look at VT100 */
int shell_getline(struct process_env *env, char *prompt)
{
    if (prompt == NULL) {
        errno = EINVAL;
        return -1;
    }
    // Print the prompt
    printf("%s%s", env->wdir, prompt);
    // Read the input
    for (size_t i = 0; i < SHELL_LINE_CAPACITY; i++) {
        // Read a character
        char c = uart0_getc();
        // If the character was a newline, put a null-terminator and return
        if (c == '\n') {
            shell_input_buf[i] = 0;
            uart0_putc(c);
            console_putc(c);
            return 0;
        }
        // Put the character into the buffer
        shell_input_buf[i] = c;
        uart0_putc(c);
        console_putc(c);
    }
    // We ran out of space if we arrived here
    errno = E2BIG;
    return -1;
}

int shell_execute_command(struct process_env *env)
{
    int ret = 0;
    // Allow blank lines
    if (strlen(shell_input_buf) == 0) return ret;

    char *cmdline = malloc(SHELL_LINE_CAPACITY);
    char *argv0;
    strcpy(cmdline, shell_input_buf);
    struct command_args args;
    args.argc = 0;
    int argcap = 1;
    args.argv = malloc(sizeof(*args.argv) * argcap);
    // Tokenize the input buffer into argv
    char *next_arg = shell_input_buf;
    char *arg = strtok_r(shell_input_buf, " ", &next_arg);
    while (NULL != arg) {
        args.argv[args.argc] = arg;
        arg = strtok_r(NULL, " ", &next_arg);
        args.argc++;
        if (args.argc == argcap) {
            argcap *= 2;
            args.argv = realloc(args.argv, sizeof(*args.argv) * argcap);
        }
    }
    args.argv[args.argc] = next_arg;
    args.argc++;
    args.envp = env;
//    printf("argc is %d\n", args.argc);
//    for (int i = 0; i < args.argc; i++)
//        printf("argv[%d] is '%s'\n", i, args.argv[i]);
    /* TODO: Tokenize the command line on any whitespace (not just spaces) */
    if ((argv0 = strtok(shell_input_buf, " ")) == NULL) argv0 = shell_input_buf;
    // Iterate through the list of available commands
    struct shell_cmdlist *cmds = shell_all_cmds;
    while (cmds != NULL) {
        // TODO: use strstr() or similar to detect if the command string is a
        // prefix substring of the line (or strtok())
        if (cmds->cmd != NULL && strcmp(argv0, cmds->cmd->cmd_str) == 0) {
            ret = cmds->cmd->fun(args);
            free(cmdline);
            return ret;
        }
        cmds = cmds->next;
    }
    // No command was matched
    printf("Error: '%s' is not a known command\n", argv0);
    free(cmdline);
    return 0;
}

int add_command(const char *cmd_str, int (*fun)(struct command_args))
{
    struct shell_command *cmd = malloc(sizeof(struct shell_command));
    if (cmd == NULL) {
        errno = ENOMEM;
        return -1;
    }
    cmd->cmd_str = (char*)cmd_str;
    cmd->fun = fun;
    struct shell_cmdlist *ptr, *this;
    this = malloc(sizeof(struct shell_cmdlist));
    this->cmd = cmd;
    this->next = NULL;
    if (shell_all_cmds == NULL) {
        shell_all_cmds = this;
        return 0;
    }
    ptr = shell_all_cmds;
    while (ptr->next != NULL) ptr = ptr->next;
    ptr->next = this;
    return 0;
}

/* Initialise the shell with all of the valid commands (uses malloc) */
int shell_init()
{
    add_command("shutdown", shell_cmd_shutdown);
    add_command("exit", shell_cmd_shutdown);
    add_command("tree", shell_cmd_tree);
    add_command("cd", shell_cmd_cd);
    add_command("ls", shell_cmd_ls);
    add_command("pwd", shell_cmd_pwd);

    return 0;
}

void shell_destroy()
{
    struct shell_cmdlist *next = shell_all_cmds;
    while (NULL != shell_all_cmds) {
        next = shell_all_cmds->next;
        if (NULL != shell_all_cmds->cmd) free(shell_all_cmds->cmd);
        free(shell_all_cmds);
        shell_all_cmds = next;
    }
}



int shell_cmd_tree(struct command_args args)
{
    // FIXME: Use a version of tree which interprets paths
    test_printtree(args.envp->root_fs->parent);
    return 0;
}

int shell_cmd_shutdown(struct command_args args)
{
    power_off();
    errno = EPERM;  // FIXME: this may not be a permission error
    return -1;  // Power_off has always failed if we continue to execute code
}

int shell_cmd_ls(struct command_args args)
{
    int ret;
    int wdirlen = strlen(args.envp->wdir);
    char *relpath = "";
    if (args.argc > 1) relpath = args.argv[1];
    int relpathlen = strlen(relpath);
    char *path = malloc(wdirlen + relpathlen + 1);
    strcpy(path, args.envp->wdir);
    strcpy(path+wdirlen, relpath);
    memset(path+wdirlen+relpathlen, 0, 1);
    // TODO: change the ls() function to take an absolute path as its argument.
    // This function can format the path based on the command line, the FS
    // function can handle the filesystem stuff.
    ret = ls(args.envp, path);
    free(path);
    return ret;
}

int shell_cmd_pwd(struct command_args args)
{
    printf("%s\n", args.envp->wdir);
    return 0;
}

int shell_cmd_cd(struct command_args args)
{
    if (args.argc == 1) {
        args.envp->wdir = "/";
        return 0;
    } else if (args.argc > 2) {
        errno = E2BIG;
        return -1;
    }
    args.envp->wdir = args.argv[1];
    return 0;
}

struct dirs_ll {
    char *this_lvl;
    int depth;
    struct dirs_ll *next;
};

// FIXME: test for malloc errors
int shell_cmd_cat(struct command_args args)
{
    if (args.argc < 2) {
        errno = EINVAL;
        return -1;
    }
    // Just read the first 4 bytes of the file for now
    char *path = malloc(strlen(args.envp->wdir) + 1 + strlen(args.argv[1]) + 1);
    size_t wdirlen = strlen(args.envp->wdir);
    memcpy(path, args.envp->wdir, wdirlen);
    memset(path+wdirlen, '/', 1);
    strcpy(path+wdirlen+1, args.argv[1]);
    char *pathtok = malloc(strlen(args.envp->wdir) + 1 + strlen(args.argv[1]) + 1);
    strcpy(pathtok, path);
    // Tokenise the path
//    char *arg = strtok_r(shell_input_buf, " ", &next_arg);
//        arg = strtok_r(NULL, " ", &next_arg);
    char *next_lvl = pathtok;
    char *tok = strtok_r(pathtok, "/", &next_lvl);
    struct dirs_ll *top_lvl = NULL;
    struct dirs_ll *prev = NULL;
    size_t cur_depth = 0;
    while (NULL != tok) {
        struct dirs_ll *cur = malloc(sizeof(struct dirs_ll));
        strcpy(cur->this_lvl, tok);
        cur->depth = cur_depth;
        if (NULL != prev) prev->next = cur;
        else {
            top_lvl = cur;
            prev = cur;
        }
    }

    // TODO: tidy up this setup - put all of these details somewhere out of the
    // way in the environment
    struct exfat_superblock super;
    exfat_read_boot_block(args.envp->root_fs->parent, &super);
    struct exfat_block_device ebd = {.bd = args.envp->root_fs->parent,
        .sb = super};
    uintptr_t this_dir_block = super.rootdir_start * super.clustersize;
    struct dirent *the_dirent = NULL;
    bool done = false;
    while (!done) {  // FIXME
        struct dirent *dirent_ll = exfat_readdir_fromblock(&ebd, this_dir_block);
        struct dirent *the_dirent = dirent_ll;
        while (strcmp(the_dirent->name, args.argv[1]) != 0)
            the_dirent = the_dirent->next;
    }

    // Open the file
    FILE *f = args.envp->root_fs->fopen(args.envp->root_fs, the_dirent, "r");

    // Close the file
    args.envp->root_fs->fclose(args.envp->root_fs, f);
    errno = ENOSYS;
    return -1;
}
