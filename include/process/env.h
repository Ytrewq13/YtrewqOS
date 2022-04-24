#ifndef __PROCESS_ENV_H
#define __PROCESS_ENV_H 1

#include <stdint.h>
#include <stddef.h>
#include "fs/fs.h"
#include "fs/block.h"

// A struct which contains all the information needed for process control
struct process_env {
    size_t pid; // Not yet used
    // TODO:
    // - exit code of commands (only useful to shell processes)
    // - last_running_timestamp?
    // - priority (like niceness)?
    // - user/group (no plans to implement these yet)
    enum {
        PROC_STATE_NEW,
        PROC_STATE_READY,
        PROC_STATE_RUNNING,
        PROC_STATE_BLOCKED,
        PROC_STATE_TERMINATED,
    } process_state; // Not yet used
    char *wdir;
    // Block device of the SD card
//    struct block_device *sd_dev;  // TODO: make this global to the kernel?
    struct fs *root_fs;
};

#endif
