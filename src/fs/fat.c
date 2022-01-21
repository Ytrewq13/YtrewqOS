/* fat.c
 * Copyright Sam Whitehead, 2021
 * Last updated 2022-01-21
 */

#include "fs/fat.h"

ERROR_TYPE fat_write_boot_block(struct block_device *dev)
{
    size_t block_size = dev->block_size;
    uint8_t blocks_per_allocation_unit = 1;
    uint16_t reserved_blocks_cnt = 1;
    uint8_t fat_cnt = 1;
    uint16_t rootdirs = 1;
    uint16_t total_blocks16 = dev->num_blocks > 0xffff ? 0 : dev->num_blocks;
    uint16_t fat_sz_blocks = 1; // TODO
    uint32_t total_blocks32 = dev->num_blocks;

    uint8_t *blk_buf = malloc(block_size);

    // Bytes per block (u16)
    blk_buf[0x0b] = dev->block_size;
    blk_buf[0x0c] = dev->block_size >> 8;
    // Blocks per allocation unit (u8)
    blk_buf[0x0d] = blocks_per_allocation_unit;
    // Reserved blocks - the boot block (u16)
    blk_buf[0x0e] = reserved_blocks_cnt;
    blk_buf[0x0f] = reserved_blocks_cnt >> 8;
    // Number of FATs (u8)
    blk_buf[0x10] = fat_cnt;
    // Number of root directory entries (including unused ones) (u16) TODO
    blk_buf[0x11] = rootdirs;
    blk_buf[0x12] = rootdirs >> 8;
    // Total number of blocks on the disk (if it fits in 16 bits) (u16)
    blk_buf[0x13] = total_blocks16;
    blk_buf[0x14] = total_blocks16 >> 8;
    // Media descriptor (unused) (u8)
    blk_buf[0x15] = 0;
    // Number of blocks occupied by one copy of the FAT (u16)
    blk_buf[0x16] = fat_sz_blocks;
    blk_buf[0x17] = fat_sz_blocks >> 8;
    // Blocks per track (u16)
    blk_buf[0x18] = 1;
    blk_buf[0x19] = 0;
    // Number of heads (u16)
    blk_buf[0x1a] = 1;
    blk_buf[0x1b] = 0;
    // The number of hidden blocks - historical (u32)
    blk_buf[0x1c] = 0;
    blk_buf[0x1d] = 0;
    blk_buf[0x1e] = 0;
    blk_buf[0x1f] = 0;
    // Total number of blocks on the disk (u32)
    blk_buf[0x20] = total_blocks32;
    blk_buf[0x21] = total_blocks32 >> 8;
    blk_buf[0x22] = total_blocks32 >> 16;
    blk_buf[0x23] = total_blocks32 >> 24;
    // Drive number (u16)
    blk_buf[0x24] = 0;
    blk_buf[0x25] = 0;
    // ... (TODO)
    // Boot signature (2 x u8)
    blk_buf[0x1fe] = 0x55;
    blk_buf[0x1ff] = 0xaa;

    sd_write(dev, blk_buf, block_size, 0);
    free(blk_buf);
    return FS_FAT_SUCCESS;
}
