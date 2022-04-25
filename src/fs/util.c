/* fs/util.c
 * Copyright Sam Whitehead, 2022
 * Last updated 2022-03-29
 *
 * This file contains filesystem utilities such as tree, ls, cat, etc.
 */

#include "fs/util.h"

int ls(struct process_env *env, const char *path)
{
    // Path is an absolute path, so scan from the root of the filesystem
    struct block_device *dev = env->root_fs->parent;
    struct exfat_superblock exfat_info;
    exfat_read_boot_block(dev, &exfat_info);
    struct exfat_block_device ebd = { .bd = dev, .sb = exfat_info };
    uintptr_t rootdir_block = (exfat_info.rootdir_start - 2) *
        exfat_info.clustersize + exfat_info.clusterheap_offset;
    struct dirent *d = exfat_readdir_fromblock(&ebd, rootdir_block);

    char *path2 = malloc(strlen(path) + 1);
    strcpy(path2, path);
    memset(path2+strlen(path), 0, 1);

    if (! (strlen(path) == 1 && path[0] == '/')) {
        int i = 0, j = 0;
        while (i <= strlen(path)) {
            if (path2[i] == '/') i++;
            j = i;
            while (path2[i] != '/' && path2[i] != 0) {
                i++;
            }
            path2[i] = 0;
            while (d != NULL && strcmp(path2+j, d->name) != 0) d = d->next;
            i++;
            if (NULL == d) {
                errno = EINVAL;
                return -1;
            }
            if (!d->is_dir) {
                errno = ENOTDIR;
                return -1;
            }
            d = exfat_readdir_fromblock(&ebd, (((struct exfat_file_contents*)d->opaque)->start_cluster - 2) * exfat_info.clustersize + exfat_info.clusterheap_offset);
        }
    }

    // Print the ls
    while (NULL != d) {
        printf("%s", d->name);
        if (d->is_dir) printf("/");
        printf("\n");
        d = d->next;
    }

    return 0;
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

