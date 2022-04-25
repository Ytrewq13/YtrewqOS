#ifndef __FS_UTIL_H
#define __FS_UTIL_H 1

#include "process/env.h"
#include "fs/fat.h"
#include "printf.h"

int ls(struct process_env *env, const char *path);
void test_printtree(struct block_device *dev);


#endif /* __fs_util_h */
