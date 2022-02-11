#ifndef __FS_FAT_H
#define __FS_FAT_H 1

#include <stdint.h>

#include "config.def.h"
#include "errno.h"
#include "error_types.h"
#include "fs/block.h"
#include "drivers/hw/eMMC.h"
#include "stdlib.h"
#include "string.h"

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

/* DEFINITIONS
 * Block: a sector on the volume
 * Superblock: the first block in an exFAT filesystem. Contains general
 *             information about the filesystem
 * Cluster Chain: A singly linked list of clusters which contains a directory.
 * Cluster: A collection of a fixed number of consecutive sectors.
 * Directory: A set of directory entries. A directory occupies an entire
 *            cluster chain.
 * Directory Entry: A 32-byte blob containing information about files and the
 *                  filesystem
 */

// The info we read from the exFAT superblock
struct exfat_superblock {
    u64 partition_offset;
    u64 volume_length;
    u64 sectorsize;  // Bytes per sector
    u64 clustersize;  // Sectors per cluster
    u32 fat_offset;
    u32 fat_length;
    u32 clusterheap_offset;
    u32 cluster_count;
    u32 rootdir_start; // First cluster of the root directory (cluster idx)
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

struct exfat_block_device {
    struct block_device bd;
    struct exfat_superblock sb;
};

// TODO: remove this?
struct exfat_cluster {
    u32 idx;  // The ClusterHeap index of this cluster
    bool cluster_bad;  // This cluster is "bad"
    bool end_of_chain;  // This cluster is the last in its ClusterChain
};

// code | importance | category | use
// (C)ritical/(B)enign (P)rimary/(S)econdary
#define ENTRYTYPE_CP(code) ((((code) & 0x1F) << 0) | (0 << 5) | (0 << 6) | (1 << 7))
#define ENTRYTYPE_BP(code) ((((code) & 0x1F) << 0) | (1 << 5) | (0 << 6) | (1 << 7))
#define ENTRYTYPE_CS(code) ((((code) & 0x1F) << 0) | (0 << 5) | (1 << 6) | (1 << 7))
#define ENTRYTYPE_BS(code) ((((code) & 0x1F) << 0) | (1 << 5) | (1 << 6) | (1 << 7))

struct exfat_directory_info {
    struct block_device *bd;
    struct exfat_superblock *super;
    uint32_t start_cluster; // First cluster of the directory
};

#define EXFAT_DIRECTORY_ENTRY_SIZE 32

// TODO: just use a union of each data type?
struct __attribute__((packed)) exfat_dirent_bitmap_chunk {
    uint8_t bitmap_flags;
    uint8_t reserved[18];
};

struct __attribute__((packed)) exfat_dirent_upcase_chunk {
    uint8_t reserved1[3];
    uint32_t table_checksum;
    uint8_t reserved2[12];
};

struct __attribute__((packed)) exfat_dirent_vollab_chunk {
    uint8_t len;
    char label[22];
};

struct __attribute__((packed)) exfat_dirent_file_chunk {
    uint8_t secondary_count;
    uint16_t set_checksum;
    uint16_t file_attributes;
    uint16_t reserved1;
    uint32_t create_timestamp;
    uint32_t modified_timestamp;
    uint32_t accessed_timestamp;
    uint8_t create_10ms_increment;
    uint8_t modified_10ms_increment;
    uint8_t create_utc_offset;
    uint8_t modified_utc_offset;
    uint8_t accessed_utc_offset;
    uint8_t reserved2[7];
};

struct __attribute__((packed)) exfat_dirent_volguid_chunk {
    uint8_t secondary_count;
    uint16_t set_checksum;
    uint16_t general_primary_flags;
    uint8_t volume_guid[16];
    uint8_t reserved[10];
};

struct __attribute__((packed)) exfat_dirent_strext_chunk {
    uint8_t general_secondary_flags;
    uint8_t reserved1[1];
    uint8_t name_length;
    uint16_t name_hash;
    uint8_t reserved2[2];
    uint64_t valid_data_length;
    uint8_t reserved3[4];
};

struct __attribute__((packed)) exfat_dirent_fname_chunk {
    uint8_t general_secondary_flags;
    uint16_t filename_part[15];  // 15 unicode characters per fname dirent
};

struct __attribute__((packed)) exfat_dirent_vendor_guid_chunk {
    uint8_t general_secondary_flags;
    uint8_t vendor_guid[16];
    uint8_t vendor_defined[14];
}; // No cluster/data associated with this dirent type

struct __attribute__((packed)) exfat_dirent_vendor_alloc_chunk {
    uint8_t general_secondary_flags;
    uint8_t vendor_guid[16];
    uint8_t vendor_defined[2];
}; // cluster/data-size associated with this dirent type

struct exfat_dirent_info {
    struct exfat_directory_info *parent;
    uint32_t direntset_idx;  // Index into the "Directory"
    enum {
        DIRENT_ALLOC_BMP = ENTRYTYPE_CP(1),
        DIRENT_UPCASE_TABLE = ENTRYTYPE_CP(2),
        DIRENT_VOLUME_LABEL = ENTRYTYPE_CP(3),
        DIRENT_FILE = ENTRYTYPE_CP(5),
        DIRENT_VOLUME_GUID = ENTRYTYPE_BP(0),
        DIRENT_STREAM_EXT = ENTRYTYPE_CS(0),
        DIRENT_FILE_NAME = ENTRYTYPE_CS(1),
        DIRENT_VENDOR_EXT = ENTRYTYPE_BS(0),
        DIRENT_VENDOR_ALLOC = ENTRYTYPE_BS(1),
        DIRENT_TEXFAT_PADDING = ENTRYTYPE_BP(1),
    } type;
    // TODO: union of extra data structs for different types of directory
    // entries.
    // If the dirent doesn't point to another cluster chain, then data_length
    // will be 0.
    uint32_t first_cluster;
    uint64_t data_length;
};

int exfat_read_boot_block(struct block_device *, struct exfat_superblock *);

#endif /* fs_fat_h */
