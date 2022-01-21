#include <stdint.h>

#define SD_WRITE_SUPPORT

#include "error_types.h"
#include "fs/block.h"
#include "drivers/hw/eMMC.h"
#include "stdlib.h"

enum {
    FS_FAT_ERRORS_BASE = 0x90000000,

    FS_FAT_SUCCESS = FS_FAT_ERRORS_BASE | 0,
};

ERROR_TYPE fat_write_boot_block(struct block_device *dev);
