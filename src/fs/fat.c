/* fat.c
 * Copyright Sam Whitehead, 2021
 * Last updated 2022-01-21
 */

#include "fs/fat.h"

int exfat_read_boot_block(struct block_device *dev, struct exfat_fs_info
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

int exfat_read_fat_entry(struct block_device *dev, struct exfat_fs_info
        *fat_info, struct exfat_cluster *cluster)
{
    size_t fat_idx = cluster->fat_idx;
    size_t block_size = dev->block_size;
    size_t block_fat_entries = block_size / 4;
    size_t active_fat_blocks_offset = fat_info->active_fat *
        fat_info->fat_length;
    size_t fat_start = (fat_info->fat_offset + active_fat_blocks_offset) *
        block_size;
    size_t block_idx = (fat_start + fat_idx) / block_fat_entries;
    size_t idx_in_block = (fat_start + fat_idx) % block_fat_entries;
    int blk_err;

    /* Read the block containing the correct FatEntry */
    uint32_t *buf = malloc(block_size);
    if (buf == NULL) {
        errno = ENOMEM;
        return -1;
    }
    blk_err = dev->read(dev, (uint8_t*)buf, block_size, block_idx);
    if (blk_err != block_size) {
        errno = EIO;
        free(buf);
        return -1;
    }

    /* Read the cluster index and determine if it is a special value */
    cluster->cluster_bad = false;
    cluster->end_of_chain = false;
    cluster->cluster_idx = buf[idx_in_block];
    if (cluster->cluster_idx == 0xFFFFFFF7)
        cluster->cluster_bad = true;
    if (cluster->cluster_idx == 0xFFFFFFFF)
        cluster->end_of_chain = true;

    free(buf);
    return 0;
}

int exfat_read_cluster_to_buf(struct block_device *dev, struct
        exfat_fs_info *fat_info, struct exfat_cluster *cluster, uint8_t *buf,
        size_t buf_size)
{
    int read_cnt;
    uintptr_t cluster_address = fat_info->clusterheap_offset +
        fat_info->clustersize * cluster->cluster_idx;
    size_t cluster_start_block = cluster_address / dev->block_size;
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
        errno = EINVAL;
        return -1;
    }
    errno = 0;
    return 0;
}

int exfat_read_dir(struct block_device *dev, struct exfat_fs_info *fat_info, struct exfat_path *dir)
{
    // TODO:
    // - Read first DirectoryEntry for this dir
    return 0;
}
