/* shell.c
 * Copyright Sam Whitehead, 2022
 * Last updated 2022-03-23
 */

#include "kernel/shell.h"

static char shell_input_buf[SHELL_LINE_CAPACITY];
static struct shell_cmdlist *shell_all_cmds = NULL;

int shell_cmd_tree(int argc, char **argv, struct process_env *envp);
int shell_cmd_shutdown(int argc, char **argv, struct process_env *envp);
int shell_cmd_ls(int argc, char **argv, struct process_env *envp);
int shell_cmd_pwd(int argc, char **argv, struct process_env *envp);
int shell_cmd_cd(int argc, char **argv, struct process_env *envp);
int shell_cmd_cat(int argc, char **argv, struct process_env *envp);

// Novelty commands added for project demo
int shell_cmd_graphics(int argc, char **argv, struct process_env *envp);
int shell_cmd_asciiart(int, char**, struct process_env*);
int shell_cmd_reverse_argv(int, char**, struct process_env*);

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
    int args_argc = 0;
    char **args_argv;
    int argcap = 1;
    args_argv = malloc(sizeof(*args_argv) * argcap);
    // Tokenize the input buffer into argv
    char *next_arg = shell_input_buf;
    char *arg = strtok_r(shell_input_buf, " ", &next_arg);
    while (NULL != arg) {
        args_argv[args_argc] = arg;
        arg = strtok_r(NULL, " ", &next_arg);
        args_argc++;
        if (args_argc == argcap) {
            argcap *= 2;
            args_argv = realloc(args_argv, sizeof(*args_argv) * argcap);
        }
    }
    args_argv[args_argc] = next_arg;
    args_argc++;
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
            ret = cmds->cmd->fun(args_argc, args_argv, env);
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

int add_command(const char *cmd_str, int (*fun)(int, char**, struct process_env*))
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
    add_command("cat", shell_cmd_cat);

    add_command("clock", shell_cmd_asciiart);
    add_command("rev", shell_cmd_reverse_argv);
    add_command("graphics", shell_cmd_graphics);

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



int shell_cmd_tree(int argc, char **argv, struct process_env *envp)
{
    // FIXME: Use a version of tree which interprets paths
    test_printtree(envp->root_fs->parent);
    return 0;
}

int shell_cmd_shutdown(int argc, char **argv, struct process_env *envp)
{
    power_off();
    errno = EPERM;  // FIXME: this may not be a permission error
    return -1;  // Power_off has always failed if we continue to execute code
}

int shell_cmd_ls(int argc, char **argv, struct process_env *envp)
{
    int ret;
    int wdirlen = strlen(envp->wdir);
    char *relpath = "";
    if (argc > 1) relpath = argv[1];
    int relpathlen = strlen(relpath);
    char *path = malloc(wdirlen + relpathlen + 1);
    strcpy(path, envp->wdir);
    strcpy(path+wdirlen, relpath);
    memset(path+wdirlen+relpathlen, 0, 1);
    // TODO: change the ls() function to take an absolute path as its argument.
    // This function can format the path based on the command line, the FS
    // function can handle the filesystem stuff.
    ret = ls(envp, path);
    free(path);
    return ret;
}

int shell_cmd_pwd(int argc, char **argv, struct process_env *envp)
{
    printf("%s\n", envp->wdir);
    return 0;
}

/* Change directory to a target (in argv[1]).
 * TODO: check target is a directory. */
int shell_cmd_cd(int argc, char **argv, struct process_env *envp)
{
    if (argc == 1) {
        // If no arguments, go to home directory ('/' for now)
        envp->wdir = "/";
        return 0;
    } else if (argc > 2) {
        errno = E2BIG;
        return -1;
    }
    if (argv[1][0] == '/')
        envp->wdir = argv[1];
    else {
        int wdirlen = strlen(envp->wdir);  // Length of the cwd
        int sep = envp->wdir[wdirlen-1] != '/';  // Do we need space for a sep?
        int tgtdirlen = strlen(argv[1]);  // Length of the cd target
        int nul = argv[1][tgtdirlen-1] != '/';  // Do we need space for NULL?
        char *newwdir = malloc(wdirlen + sep + tgtdirlen + nul);
        strcpy(newwdir, envp->wdir);
        if (sep) memset(newwdir+wdirlen, '/', 1);  // Add separator if needed
        strcpy(newwdir+wdirlen+sep, argv[1]);  // Write the target directory
        memset(newwdir+wdirlen+sep+tgtdirlen-1+nul, 0, 1);
        envp->wdir = newwdir;
    }
    return 0;
}

// Directories linked list
struct dirs_ll {
    char *this_lvl;  // Name of the file at this level
    int depth;  // Current depth
    struct dirs_ll *next;
};

// FIXME: this doesn't work yet
int shell_cmd_cat(int argc, char **argv, struct process_env *envp)
{
    printf("[NOTE] This command is a work in progress and currently doesn't work at all\n");
    if (argc < 2) {
        errno = EINVAL;
        return -1;
    }
//    char *path = malloc(strlen(args.envp->wdir) + 1 + strlen(args.argv[1]) + 1);
    char *path = malloc(strlen(argv[1]) + 1);  // TODO: allow relative paths
//    size_t wdirlen = strlen(args.envp->wdir);
//    memcpy(path, args.envp->wdir, wdirlen);
//    memset(path+wdirlen, '/', 1);
//    strcpy(path+wdirlen+1, args.argv[1]);
    strcpy(path, argv[1]);

    // Tokenise the path (FIXME: doesn't work - needs refactoring (use a parsepath() syscall))
    char *pathtok = malloc(strlen(path) + 1);
    strcpy(pathtok, path);
//    char *arg = strtok_r(shell_input_buf, " ", &next_arg);
//        arg = strtok_r(NULL, " ", &next_arg);
    char *next_lvl = pathtok;
    char *tok = strtok_r(pathtok, "/", &next_lvl);
    struct dirs_ll *top_lvl = malloc(sizeof(struct dirs_ll));
    top_lvl->this_lvl = pathtok;
    struct dirs_ll *prev = NULL;
    size_t cur_depth = 0;
    while (NULL != tok && 0 != *tok) {
        struct dirs_ll *cur = malloc(sizeof(struct dirs_ll));
        cur->this_lvl = tok;
//        strcpy(cur->this_lvl, tok);
        cur->depth = cur_depth;
        if (NULL != prev) prev->next = cur;
        else {
            top_lvl = cur;
            prev = cur;
        }
    }
    struct dirs_ll *cur = malloc(sizeof(struct dirs_ll));
    cur->this_lvl = next_lvl;

    // TODO: tidy up this setup - put all of these details somewhere out of the
    // way in the environment
    struct exfat_superblock super;
    exfat_read_boot_block(envp->root_fs->parent, &super);
    struct exfat_block_device ebd = {.bd = envp->root_fs->parent,
        .sb = super};
    // Get the root directory block (to start from)
    uintptr_t this_dir_block = (super.rootdir_start - 2) * super.clustersize +
        super.clusterheap_offset;
    struct dirent *d = NULL;
    bool done = false;
    cur = top_lvl;
    while (!done) {  // FIXME
        struct dirent *dirent_ll = exfat_readdir_fromblock(&ebd,
                this_dir_block);
        d = dirent_ll;
        while (strcmp(d->name, cur->this_lvl) != 0) {
            d = d->next;
            if (NULL == d) {
                printf("ERROR: no such file or directory!\n");
                errno = ENOENT;
                return -1;
            }
        }
        if (!d->is_dir) done = true;
        struct exfat_file_contents *opaque = d->opaque;
        this_dir_block = (opaque->start_cluster - 2) * super.clustersize +
            super.clusterheap_offset;
        cur = cur->next;
        printf("A");
    }

    // Open the file
    FILE *f = envp->root_fs->fopen(envp->root_fs, d, "r");

    char fread_buf[4];
    envp->root_fs->fread(envp->root_fs, fread_buf, 4, f);
    printf("%c%c%c%c\n", fread_buf[0], fread_buf[1], fread_buf[2], fread_buf[3]);

    // Close the file
    envp->root_fs->fclose(envp->root_fs, f);
    errno = ENOSYS;
    return -1;
}




/* Demonstration commands */
int shell_cmd_graphics(int argc, char **argv, struct process_env *envp)
{
    useconds_t step_time = 16667;  // 16.6ms (60fps)

    struct timer_wait frame_tw;

    // Get frame dimensions
    framebuf_dims_t fb;
    fb_dims(FB_ATTR_GET, NULL, &fb);

    // Disable the graphical console
    console_descriptor.enabled = 0;

    // TODO: Allocate memory and save the frame buffer
    // FIXME: malloc doesn't work for 900KB or more (800KB < limit <= 900KB)

    int32_t box_x = 100, box_y = 100;
    int32_t box_v_x = 0, box_v_y = 0;
    uint32_t box_w = 100, box_h = 100;

    uint32_t bg_color1 = 0x00000077;
    uint32_t bg_color2 = 0x00007700;
    uint32_t bg_color3 = 0x00770000;
    uint32_t bg_color4 = 0x00770077;
    bg_color1 = bg_color2 = bg_color3 = bg_color4 = 0x00777777;
    uint32_t box_color = 0x00ffff00;

    size_t frame_counter = 1;
    bool show_boxes = false;
    // Frame loop
    while (true) {
        frame_tw = register_timer(step_time);
        printf("Frame %d, box: (%d, %d)\r", frame_counter++, box_x, box_y);
        // Blank the screen (around where the box is to remove flickering)
        set_rectangle(0, 0, box_x, fb.height, bg_color1);
        set_rectangle(box_x, 0, fb.width-box_x-1, box_y, bg_color2);
        set_rectangle(box_x, box_y+box_h, fb.width-box_x-1, fb.height-(box_y+box_h), bg_color3);
        set_rectangle(box_x+box_w, box_y, fb.width-(box_x+box_w)-1, box_h, bg_color4);

        if (show_boxes) {
            bg_color1 = 0x00000077;
            bg_color2 = 0x00007700;
            bg_color3 = 0x00770000;
            bg_color4 = 0x00770077;
        } else {
            bg_color1 = bg_color2 = bg_color3 = bg_color4 = 0x00777777;
        }

        // How to access the serial port buffer directly? Let's look at the
        // source code of uart0_getc()...
//char uart0_getc()
//{
//    char r;
//    // Wait until something is in the buffer
//    while ((GET32(UART0_FR) & 0x10)) __asm volatile("nop");
//    // Read it and return
//    r = GET32(UART0_DR);
//    // Convert carriage return to newline
//    return (r == '\r' ? '\n' : r);
//}
        extern uint32_t GET32(uint64_t);
        // If there is a key
        if (!(GET32(UART0_FR) & 0x10)) {
            char r = GET32(UART0_DR);  // Get the character
            switch (r) {
                case 'w':
                    box_v_y = -4;
                    break;
                case 'a':
                    box_v_x = -4;
                    break;
                case 's':
                    box_v_y = 4;
                    break;
                case 'd':
                    box_v_x = 4;
                    break;
                case ' ':
                    box_v_x = 0;
                    box_v_y = 0;
                    break;
                case 'q':
                    goto graphics_loop_end;
                    break;
                case 'b':
                    show_boxes = !show_boxes;
                    break;
            }
        }

        // Draw the box
        set_rectangle(box_x, box_y, box_w, box_h, box_color);

        // Apply the physics
        if (box_x+box_v_x < 0 || box_x+box_v_x+box_w >= fb.width)
            box_v_x *= -1;
        box_x += box_v_x;
        if (box_y+box_v_y < 0 || box_y+box_v_y+box_h >= fb.height)
            box_v_y *= -1;
        box_y += box_v_y;

        // Wait for the end of the frame
        while (!compare_timer(frame_tw));
    }
graphics_loop_end:
    printf("\n");

    // TODO: Restore the frame buffer

    // Enable the graphical console
    console_descriptor.enabled = 1;

    errno = ENOSYS;
    return -1;
}

/* This OS is packaged with a free ROLEX! */
int shell_cmd_asciiart(int argc, char **argv, struct process_env *envp)
{
    printf("       ,--.-----.--.\n"
           "       |--|-----|--|\n"
           "       |--|     |--|\n"
           "       |  |-----|  |\n"
           "     __|--|     |--|__\n"
           "    /  |  |-----|  |  \\\n"
           "   /   \\__|-----|__/   \\\n"
           "  /   ______---______   \\/\\\n"
           " /   /  11  1 2 / 1  \\   \\/\n"
           "{   /10    ROLEX     2\\   }\n"
           "|  {     ,_.    /  ,_. }  |-,\n"
           "|  |9  {   }  O--{- } 3|  | |\n"
           "|  {   `-'  /    `-'   }  |-'\n"
           "{   \\8   7 /     5   4/   }\n"
           " \\   `------_6_------'   /\\\n"
           "  \\     __|-----|__     /\\/\n"
           "   \\   /  |-----|  \\   /\n"
           "    \\  |--|     |--|  /\n"
           "     --|  |-----|  |--\n"
           "       |--|     |--|\n"
           "       |--|-----|--|\n"
           "       `--'-----`--'\n");
    return 0;
    // TODO: make an analogue clock?
}

int shell_cmd_reverse_argv(int argc, char **argv, struct process_env *envp)
{
    // For each argument (in reverse) (not the command name)
    for (size_t i = argc-1; i > 0; i--) {
        size_t len = strlen(argv[i]);
        // Print each character in the string (in reverse)
        for (int j = len-1; j >= 0; j--) {
            char c = *(argv[i]+j);
            printf("%c", c);
        }
        printf(" ");
    }
    printf("\n");
    return -1;
}
