/* fs/util.c
 * Copyright Sam Whitehead, 2022
 * Last updated 2022-03-29
 *
 * This file contains filesystem utilities such as tree, ls, cat, etc.
 */

#include "fs/util.h"

int ls(struct process_env *env, const char *path)
{
    printf("ls '%s'\n", path);
    printf("ERROR: ls is not yet implemented!\n");
    errno = ENOSYS;
    return -1;
}

// TODO:
// - Add path parsing
// - Improve directory formatting
void test_printtree(struct block_device *dev)
{
    struct exfat_superblock exfat_info;
    exfat_read_boot_block(dev, &exfat_info);
    uintptr_t rootdir_block = (exfat_info.rootdir_start - 2) * exfat_info.clustersize + exfat_info.clusterheap_offset;
    struct exfat_block_device ebd = { .bd = dev, .sb = exfat_info };
    struct dirent *d, *dirent_it;
    struct dirent_chain *chain = NULL, *tmpchain;
    d = exfat_readdir_fromblock(&ebd, rootdir_block);

    size_t tree_depth = 0;
    dirent_it = d;
    while (dirent_it != NULL || chain != NULL) {
        for (int i = 0; i < tree_depth; i++) {
            printf("| ");
        }
        if (dirent_it->is_dir) console_set_tmp_fg_color(CONFIG_COLOR_TEST_PASS);
        printf("%s", dirent_it->name);
        if (dirent_it->is_dir) {
            console_reset_colors();
            printf("/");
            // Save the dirent iterator so we can pick up where we left off
            tmpchain = malloc(sizeof(struct dirent_chain));
            if (tmpchain == NULL) {
                errno = ENOMEM;
                return;
            }
            tmpchain->next = chain;
            tmpchain->d = dirent_it;
            chain = tmpchain;
            tree_depth++;
            dirent_it = exfat_readdir_fromblock(&ebd, (((struct exfat_file_contents*)dirent_it->opaque)->start_cluster - 2) * exfat_info.clustersize + exfat_info.clusterheap_offset);
        } else {
            dirent_it = dirent_it->next;
        }
        printf("\n");
        while (dirent_it == NULL && chain != NULL) {
            tree_depth--;
            tmpchain = chain->next;
            dirent_it = chain->d->next;
            chain = tmpchain;
        }
    }
}

