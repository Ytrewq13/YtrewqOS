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
    cluster.idx = fat_idx_to_cluster_idx(exdev->bd, exdev->sb, fat_idx);
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
        sb)
{ return (cluster_idx-2) * sb.clustersize + sb.clusterheap_offset; }

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
            str_from_wchar(volume_label.label, (uint16_t*)dirent_contents+1, volume_label.len);
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

/* Read the directory set of one whole file into a dirent struct */
static int read_file_direntset(struct exfat_dirent_info first_edirent, struct dirent *dirent)
{
    int err;
    struct exfat_dirent_info ed;
    struct exfat_file_contents *file_contents;
    size_t name_cnt = first_edirent.entry_data.file_metadata\
                      .secondary_count - 1;
    char *name_ptr;

    dirent->is_dir = first_edirent.entry_data.file_metadata.file_attributes\
                     .directory;

    ed.parent = first_edirent.parent;
    ed.direntset_idx = first_edirent.direntset_idx + 1;
    err = exfat_read_directory_entry(&ed);
    if (err) return -1;
    if (ed.type != DIRENT_STREAM_EXT) {
        errno = EINVAL;
        return -1;
    }

    dirent->byte_size = ed.data_length;
    file_contents = malloc(sizeof(struct exfat_file_contents));
    if (file_contents == NULL) {
        errno = ENOMEM;
        return -1;
    }
    file_contents->ebd.bd = ed.parent->bd;
    file_contents->ebd.sb = ed.parent->super;
    file_contents->start_cluster = ed.first_cluster;
    file_contents->data_length = ed.data_length;
    dirent->opaque = file_contents;
    dirent->name = malloc(ed.entry_data.stream_extension.name_length + 1);
    // This memset is left out to speed up the process. Our malloc doesn't zero
    // the memory it gives us, so we have to dispose of any sensitive data such
    // as encryption keys in memory we free to the system, as previous data IS
    // leaked to anyone who calls malloc() next.
//    memset(dirent->name, 0, ed.entry_data.stream_extension.name_length + 1);
    name_ptr = dirent->name;
    for (int i = 0; i < name_cnt; i++) {
        ed.direntset_idx++;
        err = exfat_read_directory_entry(&ed);
        if (err) {
            errno = EINVAL;
            return -1;
        }
        name_ptr += str_from_wchar(name_ptr,
                ed.entry_data.filename.filename_part, 15);
    }
    return 0;
}

struct dirent *exfat_readdir_fromblock(struct exfat_block_device *edev,
        uintptr_t dir_start_block)
{
    struct block_device *dev = edev->bd;
    struct exfat_dirent_info edirent;
    struct exfat_directory_info edirectory;
    int err;

    struct dirent *res = NULL, *cur, *prev = NULL;

    edirectory.bd = dev;
    edirectory.super = edev->sb;
    edirectory.start_cluster = (dir_start_block - edev->sb.clusterheap_offset)
        / edev->sb.clustersize + 2;
    edirent.parent = &edirectory;
    edirent.direntset_idx = 0;
    edirent.type = DIRENT_INVALID;
    while (edirent.type != DIRENT_EOD) {
        // TODO: function that only parses the type of a dirent
        err = exfat_read_directory_entry(&edirent);
        if (err) return NULL;
        switch (edirent.type) {
            case DIRENT_FILE:
                cur = malloc(sizeof(struct dirent));
                cur->next = NULL;
                if (prev != NULL) prev->next = cur;
                err = read_file_direntset(edirent, cur);
                if (err) return NULL;
                prev = cur;
                if (res == NULL) res = prev;
                // Skip to after the file directory entry set
                edirent.direntset_idx += edirent.entry_data.file_metadata\
                                         .secondary_count;
                break;
            default:
                break;
        }
        edirent.direntset_idx++;
    }
    return res;
}

enum FILE_MODE {
    /* r    open file for reading */
    FILE_MODE_R,
    /* r+   open file for reading and writing */
    /* w    truncate file to zero length or create text file for writing */
    /* w+   open for reading and writing. The file is created or truncated to
     *      zero length */
    /* a    open for appending. Stream is positioned at the end of the file */
    /* a+   open for reading and appending */
    FILE_MODE_INVALID,  // No writing support yet
};

static enum FILE_MODE get_file_mode(const char *mode)
{
    /* r    open file for reading
     * NOT SUPPORTED YET:
     * r+   open file for reading and writing
     * w    truncate file to zero length or create text file for writing
     * w+   open for reading and writing. The file is created or truncated to
     *      zero length
     * a    open for appending. Stream is positioned at the end of the file
     * a+   open for reading and appending */
    if (strcmp(mode, "r") == 0) return FILE_MODE_R;
    else return FILE_MODE_INVALID;
}

struct exfat_file_opaque {
    struct exfat_superblock *super;
    struct block_device *bd;
    // First cluster of the file's contents
    uint32_t first_cluster;
    // The cluster containing the current position
    uint32_t current_cluster;
    // The block index into the current cluster
    size_t block_cluster_offset;
    // The index into the block
    size_t pos_block_offset;
};

/* "Open" a file */
FILE *exfat_fopen(struct fs *fs, struct dirent *file, const char *mode)
{
    FILE *fp = malloc(sizeof(FILE));
    if (NULL == fp) {
        errno = ENOMEM;
        return NULL;
    }
    fp->fs = fs;
    fp->pos = 0;  // TODO: this depends on the file mode
    fp->pos = get_file_mode(mode);
    if (fp->pos == FILE_MODE_INVALID) {
        errno = EINVAL;
        return NULL;
    }
    struct exfat_file_opaque *opaque = malloc(sizeof(struct exfat_file_opaque));
    if (NULL == opaque) {
        errno = ENOMEM;
        return NULL;
    }
    opaque->super = malloc(sizeof(struct exfat_superblock));
    if (NULL == opaque->super) {
        free(opaque);
        errno = ENOMEM;
        return NULL;
    }
    struct exfat_file_contents *d_opaque = file->opaque;
    memcpy(opaque->super, &d_opaque->ebd.sb, sizeof(struct exfat_superblock));
    opaque->bd = malloc(sizeof(struct block_device));
    if (NULL == opaque->bd) {
        free(opaque);
        errno = ENOMEM;
        return NULL;
    }
    memcpy(opaque->bd, d_opaque->ebd.bd, sizeof(struct block_device));
    opaque->first_cluster = d_opaque->start_cluster;
    opaque->current_cluster = d_opaque->start_cluster;
    opaque->block_cluster_offset = 0;
    opaque->pos_block_offset = 0;
    fp->opaque = opaque;  // TODO: put the file contents location in opaque
    fp->len = file->byte_size;
    fp->flags = 0;  // TODO: are there any important flags?
    fp->fflush_cb = NULL;  // TODO
    return fp;
}

// TODO: test this function
size_t exfat_fread(struct fs *fs, void *ptr, size_t byte_size, FILE *stream)
{
    /*
     * Read to the next full cluster
     *      Need to read (clustersize-currentblock) blocks
     *      - Read the current block to a temporary buffer
     *      - Copy the end of the temporary buffer (from the current position)
     *        into the given buffer
     *      - Read all of the full blocks
     * Read all of the full clusters
     *      For each cluster:
     *      - Read the cluster using exfat_read_cluster_to_buf()
     *      - Get the next cluster using fat_idx_to_cluster_idx()
     * Read the remaining blocks
     *      - Read all of the full blocks
     *      - Read the next block to a temporary buffer
     *      - Copy the data we want to the given buffer
     */
    struct exfat_file_opaque *opaque = stream->opaque;
    // The first cluster in the requested region (The cluster in which the
    // region starts)
    size_t first_cluster = opaque->current_cluster;

    // Create a temporary buffer used to hold single blocks
    void *tmp_buf = malloc(opaque->bd->block_size);
    if (NULL == tmp_buf) {
        errno = ENOMEM;
        return 0;
    }

    // Check the logical end position against the file size
    if (stream->pos + byte_size >= stream->len)
        byte_size = stream->len - stream->pos;

    size_t bytes_read = 0;

    // Read to the end of the current cluster
    /////////////////////////////////////////

    // Read the current block to a temporary buffer
    size_t current_block = cluster_to_block(first_cluster, *opaque->super) +
        opaque->block_cluster_offset;
    opaque->bd->read(opaque->bd, tmp_buf, opaque->bd->block_size, current_block);
    // Copy the end of the temporary buffer into the given buffer
    size_t read_size = opaque->bd->block_size - opaque->pos_block_offset;
    memcpy(ptr, tmp_buf+opaque->pos_block_offset, read_size);
    bytes_read += read_size;
    opaque->pos_block_offset = 0;
    opaque->block_cluster_offset++;
    if (opaque->block_cluster_offset == opaque->super->clustersize) {
        opaque->block_cluster_offset = 0;
        opaque->current_cluster = fat_idx_to_cluster_idx(opaque->bd,
                *opaque->super, opaque->current_cluster);
    } else {
        // Read all of the full blocks
        current_block = cluster_to_block(opaque->current_cluster,
                *opaque->super);
        read_size = opaque->bd->block_size * (opaque->super->clustersize -
                opaque->block_cluster_offset);
        opaque->bd->read(opaque->bd, ptr+bytes_read, read_size, current_block);
        bytes_read += read_size;
        opaque->pos_block_offset = 0;
        opaque->block_cluster_offset = 0;
        opaque->current_cluster = fat_idx_to_cluster_idx(opaque->bd,
                *opaque->super, opaque->current_cluster);
    }

    // Read all of the full clusters
    ////////////////////////////////

    // How many bytes will be left over after the full clusters
    size_t loose_bytes = (byte_size - bytes_read) % (opaque->super->clustersize
            * opaque->bd->block_size);
    // How many full clusters to read
    size_t clusters_count = (byte_size - bytes_read - loose_bytes) /
        opaque->super->clustersize / opaque->bd->block_size;

    while (clusters_count--) {
        struct exfat_cluster cluster;
        cluster.idx = opaque->current_cluster;
        exfat_read_cluster_to_buf(opaque->bd, opaque->super, &cluster,
                ptr+bytes_read, opaque->super->clustersize *
                opaque->bd->block_size);
        bytes_read += opaque->super->clustersize * opaque->bd->block_size;
        opaque->current_cluster = fat_idx_to_cluster_idx(opaque->bd,
                *opaque->super, opaque->current_cluster);
    }

    // Read the remaining blocks
    ////////////////////////////

    // Read all of the full blocks
    current_block = cluster_to_block(opaque->current_cluster, *opaque->super);
    read_size = (loose_bytes / opaque->bd->block_size) * opaque->bd->block_size;
    opaque->bd->read(opaque->bd, ptr+bytes_read, read_size, current_block);
    bytes_read += read_size;
    opaque->block_cluster_offset = read_size / opaque->bd->block_size;
    loose_bytes -= read_size;

    // Read the next block to a temporary buffer
    current_block += opaque->block_cluster_offset;
    read_size = opaque->bd->block_size;
    opaque->bd->read(opaque->bd, tmp_buf, read_size, current_block);
    bytes_read += read_size;

    // Copy the data we want to the given buffer
    memcpy(ptr+bytes_read, tmp_buf, loose_bytes);

    // Update the file stream's position
    stream->pos += bytes_read;
    opaque->pos_block_offset = loose_bytes;

    // Free the temporary buffer
    free(tmp_buf);

    return bytes_read;
}

int exfat_fclose(struct fs *fs, FILE *fp)
{
    free(fp->opaque);
    free(fp);
    return 0;
}
