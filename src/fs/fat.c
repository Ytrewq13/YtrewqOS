/* fat.c
 * Copyright Sam Whitehead, 2021
 * Last updated 2022-01-21
 */

#include "fs/fat.h"

int exfat_read_boot_block(struct block_device *dev, struct exfat_superblock
        *fat_info)
{
    size_t block_size = dev->block_size;
    uint8_t *buf = malloc(block_size);
    uint16_t flags;
    int blk_err;

    /* Read the boot sector */
    blk_err = dev->read(dev, buf, block_size, 0);
    if (blk_err != block_size) {
        free(buf);
        errno = EIO;
        return -1;
    }
    /* Read the struct members */
    memcpy(&fat_info->partition_offset, buf+64, 8);
    memcpy(&fat_info->volume_length, buf+72, 8);
    fat_info->sectorsize = 1L << buf[108];
    fat_info->clustersize = 1L << buf[109];
    memcpy(&fat_info->fat_offset, buf+80, 4);
    memcpy(&fat_info->fat_length, buf+84, 4);
    memcpy(&fat_info->clusterheap_offset, buf+88, 4);
    memcpy(&fat_info->cluster_count, buf+92, 4);
    memcpy(&fat_info->rootdir_start, buf+96, 4);
    memcpy(&fat_info->volume_serialnumber, buf+100, 4);
    memcpy(&fat_info->fs_revision, buf+104, 2);
    fat_info->fat_cnt = buf[110];
    fat_info->use_percent = buf[112];
    /* Volume flags */
    memcpy(&flags, buf+106, 2);
    fat_info->active_fat = (flags & EXFAT_VOLFLAGS_ACTIVE_FAT) ? 1 : 0;
    fat_info->volume_dirty = flags & EXFAT_VOLFLAGS_VOL_DIRTY;
    fat_info->media_failure = flags & EXFAT_VOLFLAGS_MEDIA_FAILURE;
    fat_info->clear_to_zero = flags & EXFAT_VOLFLAGS_CLEAR_ZERO;
    /* Throw away the buffer and return sucessfully */
    free(buf);
    return 0;
}

/* Returns 1 on error. */
static uint32_t fat_idx_to_cluster_idx(struct block_device *dev, struct
        exfat_superblock sb, uint32_t fat_idx)
{
    uint32_t cluster_idx;
    size_t block_size = dev->block_size;
    size_t block_fat_entries = block_size / 4;
    size_t active_fat_blocks_offset = sb.active_fat * sb.fat_length;
    size_t fat_start = (sb.fat_offset + active_fat_blocks_offset) * block_size;
    size_t block_idx = (fat_start + fat_idx) / block_fat_entries;
    size_t idx_in_block = (fat_start + fat_idx) % block_fat_entries;
    int blk_err;

    /* Read the block containing the correct FatEntry */
    uint32_t *buf = malloc(block_size);
    if (buf == NULL) {
        errno = ENOMEM;
        return 1;
    }
    blk_err = dev->read(dev, (uint8_t*)buf, block_size, block_idx);
    if (blk_err != block_size) {
        errno = EIO;
        free(buf);
        return 1;
    }

    /* Read the cluster index */
    cluster_idx = buf[idx_in_block];

    free(buf);
    return cluster_idx;
}

/* On error, the cluster index is set to 1. */
struct exfat_cluster exfat_interpret_fat_entry(struct exfat_block_device
        *exdev, uint32_t fat_idx)
{
    struct exfat_cluster cluster;
    errno = 0;  // Reset errno
    /* Read the cluster index and determine if it is a special value */
    cluster.cluster_bad = false;
    cluster.end_of_chain = false;
    /* NOTE: fat_idx_to_cluster_idx() sets errno on error */
    cluster.idx = fat_idx_to_cluster_idx(&exdev->bd, exdev->sb, fat_idx);
    cluster.cluster_bad = (cluster.idx == 0xFFFFFFF7);
    cluster.end_of_chain = (cluster.idx == 0xFFFFFFFF);

    return cluster;
}

static size_t cluster_to_block(uint32_t cluster_idx, struct exfat_superblock *sb)
{ return cluster_idx * sb->clustersize + sb->clusterheap_offset; }

static size_t directory_start_block(struct exfat_directory_info *dir)
{ return cluster_to_block(dir->start_cluster, dir->super); }

/* Read an exFAT DirectoryEntry */
int exfat_read_directory_entry(struct exfat_dirent_info dirent)
{
    struct block_device *dev = dirent.parent->bd;
    size_t block_size = dev->block_size;
    int read_cnt;
    uint8_t *buf;
    size_t dir_start;  // First block of the directory
    size_t dirent_addr;
    size_t dirent_blk;
    uint32_t dirent_offset;  // Offset into the block
    uint8_t dirent_contents[EXFAT_DIRECTORY_ENTRY_SIZE];

    dir_start = directory_start_block(dirent.parent);
    dirent_addr = dir_start * block_size + 4 * dirent.direntset_idx;
    dirent_blk = dirent_addr / block_size;
    dirent_offset = dirent_addr % block_size;

    buf = malloc(block_size);
    if (buf == NULL) {
        errno = ENOMEM;
        return -1;
    }
    read_cnt = dev->read(dev, buf, block_size, dirent_blk);
    if (read_cnt != block_size) {
        errno = EIO;
        free(buf);
        return -1;
    }
    memcpy(dirent_contents, buf+dirent_offset, EXFAT_DIRECTORY_ENTRY_SIZE);
    free(buf);  // Free the block buffer - we have read the DirectoryEntry
    // TODO: parse the directory entry that is now in dirent_contents[] and
    // read the information into the struct `dirent`.
    errno = ENOSYS;  // Function not implemented
    return -1;
}

// TODO: work out the correct function signature (and implement any needed
// structs)
// - global variable to store allocation bitmap? <- this is a bad idea:
// requires 4k for a 1GB HDD, scales linearly. Too much RAM.
// - Store allocation changes in memory and flush multiple at once?
int exfat_read_allocation_bitmap()
{
    errno = ENOSYS;
    return -1;
}

/* TODO:
 * - When reading files, do we want to only read one cluster at a time to save
 *   memory?
 */

/* Reads a cluster's data into a buffer of the correct size. Sets errno and
 * returns -1 on error, 0 on success. */
int exfat_read_cluster_to_buf(struct block_device *dev, struct
        exfat_superblock *fat_info, struct exfat_cluster *cluster, uint8_t *buf,
        size_t buf_size)
{
    int read_cnt;
    size_t cluster_start_block = fat_info->clusterheap_offset +
        fat_info->clustersize * cluster->idx;
    if (buf == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (buf_size != fat_info->clustersize) {
        errno = EINVAL;
        return -1;
    }
    read_cnt = dev->read(dev, buf, buf_size, cluster_start_block);
    if (read_cnt != buf_size) {
        errno = EIO;
        return -1;
    }
    errno = 0;
    return 0;
}

int exfat_read_dir(struct block_device *dev, struct exfat_superblock *fat_info,
        char *dir)
{
    // TODO
    errno = ENOSYS;  // Function not implemented
    return -1;
}
