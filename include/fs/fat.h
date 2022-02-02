#ifndef __FS_FAT_H
#define __FS_FAT_H 1

#include <stdint.h>

#define SD_WRITE_SUPPORT

#include "errno.h"
#include "error_types.h"
#include "fs/block.h"
#include "drivers/hw/eMMC.h"
#include "stdlib.h"
#include "string.h"

enum {
    FS_FAT_ERRORS_BASE = 0x90000000,

    FS_FAT_SUCCESS = FS_FAT_ERRORS_BASE | 0,
    FS_FAT_ERR_SD_FAIL = FS_FAT_ERRORS_BASE | 1,
    FS_FAT_ERR_WRONG_FS = FS_FAT_ERRORS_BASE | 2,
    FS_FAT_ERR_FS_CORRUPT = FS_FAT_ERRORS_BASE | 3,
    FS_FAT_INCORRECT_BUFFER_SIZE = FS_FAT_ERRORS_BASE | 4,
    FS_FAT_ERR_NULLPTR = FS_FAT_ERRORS_BASE | 5,
};

// TODO: Abstract away to a types.h file. Use typedef instead?
#define u64 uint64_t
#define u32 uint32_t
#define u16 uint16_t
#define u8  uint8_t

// volume flags indices
#define EXFAT_VOLFLAGS_ACTIVE_FAT       (1 << 15)
#define EXFAT_VOLFLAGS_VOL_DIRTY        (1 << 14)
#define EXFAT_VOLFLAGS_MEDIA_FAILURE    (1 << 13)
#define EXFAT_VOLFLAGS_CLEAR_ZERO       (1 << 12)

struct exfat_fs_info {
    u64 partition_offset;
    u64 volume_length;
    u64 sectorsize;  // Bytes per sector
    u64 clustersize;  // Sectors per cluster
    u32 fat_offset;
    u32 fat_length;
    u32 clusterheap_offset;
    u32 cluster_count;
    u32 rootdir_start; // First cluster of the root directory
    u32 volume_serialnumber;
    u16 fs_revision;
    u8  fat_cnt;  // Number of FAT tables
    u8  use_percent;  // TODO: function to update this
    // Volume flags
    u8 active_fat;  // Which fat is active
    bool volume_dirty;
    bool media_failure;
    bool clear_to_zero;
};

struct exfat_cluster {
    u32 fat_idx;  // The FAT index of this cluster
    u32 cluster_idx;  // The ClusterHeap index of this cluster
    bool cluster_bad;  // This cluster is "bad"
    bool end_of_chain;  // This cluster is the last in its ClusterChain
};

enum fs_path_type {
    FS_FAT_PATH_FILE,
    FS_FAT_PATH_DIR,
};

struct exfat_path {
    char *path;
    enum fs_path_type type;
    struct exfat_path *children;
    size_t child_cnt;
};

//ERROR_TYPE exfat_write_boot_block(struct block_device *dev);
//ERROR_TYPE exfat_read_boot_block(struct block_device *dev, struct exfat_fs_info
//        *fat_info);
//ERROR_TYPE exfat_read_fat_entry(struct block_device *dev, struct exfat_fs_info
//        *fat_info, struct exfat_cluster *cluster);
//ERROR_TYPE exfat_read_cluster_to_buf(struct block_device *dev, struct
//        exfat_fs_info *fat_info, struct exfat_cluster *cluster, uint8_t *buf,
//        size_t buf_size);

#endif /* fs_fat_h */
