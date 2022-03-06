/* fat.c
 * Copyright Sam Whitehead, 2021
 * Last updated 2022-01-21
 */

#include "fs/fat.h"
#include "printf.h"

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
// FIXME: this function gets the *NEXT* cluster in the cluster chain, not the
// current cluster. The current cluster is just the fat index.
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

static int exfat_dirent_interpret_entrytype(uint8_t entrytype, enum EXFAT_DIRENT_TYPE
        *type, bool *use)
{
    *use = entrytype & ENTRYTYPE_USE_MASK;
#define CRITICAL    (0 << 5)
#define BENIGN      (1 << 5)
#define PRIMARY     (0 << 6)
#define SECONDARY   (1 << 6)
    switch (entrytype & ~ENTRYTYPE_USE_MASK) {
        case 0x00:
            if (*use) *type = DIRENT_INVALID;
            else *type = DIRENT_EOD;
            break;
        case (CRITICAL | PRIMARY   | EXFAT_TYPECODE_ALLOCATION_BITMAP):
            *type = DIRENT_ALLOC_BMP; break;
        case (CRITICAL | PRIMARY   | EXFAT_TYPECODE_UPCASE_TABLE):
            *type = DIRENT_UPCASE_TABLE; break;
        case (CRITICAL | PRIMARY   | EXFAT_TYPECODE_VOLUME_LABEL):
            *type = DIRENT_VOLUME_LABEL; break;
        case (CRITICAL | PRIMARY   | EXFAT_TYPECODE_FILE):
            *type = DIRENT_FILE; break;
        case (BENIGN   | PRIMARY   | EXFAT_TYPECODE_VOLUME_GUID):
            *type = DIRENT_VOLUME_GUID; break;
        case (CRITICAL | SECONDARY | EXFAT_TYPECODE_STREAM_EXTENSION):
            *type = DIRENT_STREAM_EXT; break;
        case (CRITICAL | SECONDARY | EXFAT_TYPECODE_FILE_NAME):
            *type = DIRENT_FILE_NAME; break;
        case (BENIGN   | SECONDARY | EXFAT_TYPECODE_VENDOR_EXTENSION):
            *type = DIRENT_VENDOR_EXT; break;
        case (BENIGN   | SECONDARY | EXFAT_TYPECODE_VENDOR_ALLOC):
            *type = DIRENT_VENDOR_ALLOC; break;
        case (BENIGN   | PRIMARY   | EXFAT_TYPECODE_TEXFAT_PADDING):
            *type = DIRENT_TEXFAT_PADDING; break;
        default:
            return -1;
    }
#undef CRITICAL
#undef BENIGN
#undef PRIMARY
#undef SECONDARY
    return 0;
}

static size_t cluster_to_block(uint32_t cluster_idx, struct exfat_superblock
        *sb)
{ return (cluster_idx-2) * sb->clustersize + sb->clusterheap_offset; }

static size_t directory_start_block(struct exfat_directory_info *dir)
{ return cluster_to_block(dir->start_cluster, dir->super); }

static int exfat_read_directory_entry_data(struct exfat_dirent_info *dirent,
        uint8_t *dirent_contents)
{
    struct exfat_dirent_bitmap_chunk bitmap = {0};
    struct exfat_dirent_upcase_chunk upcase = {0};
    struct exfat_dirent_vollab_chunk volume_label = {0};
    struct exfat_dirent_file_chunk file = {0};
    struct exfat_dirent_volguid_chunk vguid = {0};
    struct exfat_dirent_strext_chunk stream_extension = {0};
    struct exfat_dirent_fname_chunk filename = {0};
    struct exfat_dirent_vendor_guid_chunk vend_guid = {0};
    struct exfat_dirent_vendor_alloc_chunk vend_alloc = {0};
    int err;
    err = exfat_dirent_interpret_entrytype(dirent_contents[0], &dirent->type,
            &dirent->in_use);
    if (err) {
        // Invalid type code - maybe corrupt FS or just wrong offset?
        errno = EINVAL;
        return -1;
    }
    switch ((dirent->type)) {
        case DIRENT_EOD: break;
        case DIRENT_ALLOC_BMP:
            bitmap.bitmap_flags = dirent_contents[1];
            dirent->entry_data.bitmap = bitmap;
            memcpy(&dirent->first_cluster, dirent_contents+20, 4);
            memcpy(&dirent->data_length, dirent_contents+24, 8);
            break;
        case DIRENT_UPCASE_TABLE:
            upcase.table_checksum = dirent_contents[2];
            dirent->entry_data.upcase_table = upcase;
            memcpy(&dirent->first_cluster, dirent_contents+20, 4);
            memcpy(&dirent->data_length, dirent_contents+24, 8);
            break;
        case DIRENT_VOLUME_LABEL:
            volume_label.len = dirent_contents[1];
            if (volume_label.len > 11) {
                errno = EINVAL;
                return -1;
            }
            // Copy the LSB of each unicode (UTF-16) character. TODO: Add
            // support for UTF-16 characters (Windows WCHARs)
            for (uint_fast8_t i = 0; i < volume_label.len; i++)
                volume_label.label[i] = dirent_contents[2+i*2];
            dirent->entry_data.volume_label = volume_label;
            break;
        case DIRENT_FILE:
            file.secondary_count = dirent_contents[1];
            memcpy(&file.set_checksum, dirent_contents+2, 2);
            memcpy(&file.file_attributes, dirent_contents+4, 2);
            memcpy(&file.create_timestamp, dirent_contents+8, 4);
            memcpy(&file.modified_timestamp, dirent_contents+12, 4);
            memcpy(&file.accessed_timestamp, dirent_contents+16, 4);
            file.create_10ms_increment = dirent_contents[20];
            file.modified_10ms_increment = dirent_contents[21];
            file.create_utc_offset = dirent_contents[22];
            file.modified_utc_offset = dirent_contents[23];
            file.accessed_utc_offset = dirent_contents[24];
            dirent->entry_data.file_metadata = file;
            break;
        case DIRENT_VOLUME_GUID:
            vguid.secondary_count = dirent_contents[1];
            memcpy(&vguid.set_checksum, dirent_contents+2, 2);
            memcpy(&vguid.general_primary_flags, dirent_contents+4, 2);
            memcpy(&vguid.volume_guid, dirent_contents+6, 16);
            dirent->entry_data.volume_guid = vguid;
            break;
        case DIRENT_STREAM_EXT:
            stream_extension.general_secondary_flags = dirent_contents[1];
            stream_extension.name_length = dirent_contents[3];
            memcpy(&stream_extension.name_hash, dirent_contents+4, 2);
            memcpy(&stream_extension.valid_data_length, dirent_contents+8, 8);
            dirent->entry_data.stream_extension = stream_extension;
            memcpy(&dirent->first_cluster, dirent_contents+20, 4);
            memcpy(&dirent->data_length, dirent_contents+24, 8);
            break;
        case DIRENT_FILE_NAME:
            filename.general_secondary_flags = dirent_contents[1];
            memcpy(&filename.filename_part, dirent_contents+2, 30);
            dirent->entry_data.filename = filename;
            break;
        case DIRENT_VENDOR_EXT:
            vend_guid.general_secondary_flags = dirent_contents[1];
            memcpy(&vend_guid.vendor_guid, dirent_contents+2, 16);
            memcpy(&vend_guid.vendor_defined, dirent_contents+18, 14);
            break;
        case DIRENT_VENDOR_ALLOC:
            vend_alloc.general_secondary_flags = dirent_contents[1];
            memcpy(&vend_alloc.vendor_guid, dirent_contents+2, 16);
            memcpy(&vend_alloc.vendor_defined, dirent_contents+18, 2);
            memcpy(&dirent->first_cluster, dirent_contents+20, 4);
            memcpy(&dirent->data_length, dirent_contents+24, 8);
            break;
        case DIRENT_TEXFAT_PADDING:
            // TexFAT Padding Directory Entry is not implemented/supported
            errno = ENOSYS;
            return -1;
            break;
        case DIRENT_INVALID:
            errno = EIO;
            return -1;
    }
    return 0;
}

/* Read an exFAT DirectoryEntry */
int exfat_read_directory_entry(struct exfat_dirent_info *dirent)
{
    struct block_device *dev = dirent->parent->bd;
    size_t block_size = dev->block_size;
    int read_cnt;
    uint8_t *buf;
    size_t dir_start;  // First block of the directory
    size_t dirent_addr;
    size_t dirent_blk;
    uint32_t dirent_offset;  // Offset into the block
    uint8_t dirent_contents[EXFAT_DIRECTORY_ENTRY_SIZE];
    int err;

    dir_start = directory_start_block(dirent->parent);
    dirent_addr = dir_start * block_size + EXFAT_DIRECTORY_ENTRY_SIZE * dirent->direntset_idx;
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
    /* Free the block buffer - we have read the DirectoryEntry */
    free(buf);
    /* Parse the directory entry that is now in dirent_contents[] and read the
     * information into the struct `dirent`. */
    err = exfat_read_directory_entry_data(dirent, dirent_contents);
    if (err != 0) return err; // errno is already set
    return 0;
}

/* TODO:
 * - When reading files, do we want to only read one cluster at a time to save
 *   memory? (Or one block at a time - read clusters block-by block?)
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

/* Returns the exfat hash of a filename. Filename argument is given as a wide
 * character string (2 bytes per character) and should be up-cased prior to
 * calling this function. */
static uint16_t exfat_name_hash(int16_t *filename, uint8_t namelength)
{
    uint16_t *buffer = (uint16_t *)filename;
    uint16_t number_of_bytes = (uint16_t)namelength * 2;
    uint16_t hash = 0;
    uint16_t index;

    for (index = 0; index < number_of_bytes; index++) {
        hash = ((hash & 1) ? 0x8000 : 0) + (hash >> 1) +
            (uint16_t)buffer[index];
    }
    return hash;
}

int exfat_read_dir(struct block_device *dev, struct exfat_superblock *fat_info,
        char *dir)
{
    // TODO
    errno = ENOSYS;  // Function not implemented
    return -1;
}
