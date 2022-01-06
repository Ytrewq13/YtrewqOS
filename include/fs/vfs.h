#ifndef VFS_H
#define VFS_H 1

#include <stdlib.h>

struct vfs_file;

#include "fs/dirent.h"
#include "fs/file.h"

// An entry in the Virtual FileSystem
struct vfs_entry {
    char *device_name;
    struct fs *fs;
    struct vfs_entry *next;
};

struct vfs_file {
    struct fs *fs;
    long pos;
    int mode;
    void *opaque;
    long len;
    int flags;
    int (*fflush_cb)(FILE *f);
};

int fseek(FILE *stream, long offset, int whence);
long ftell(FILE *stream);
long fsize(FILE *stream);
int feof(FILE *stream);
int ferror(FILE *stream);
int fflush(FILE *stream);
void rewind(FILE *stream);


size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t fwrite(void *ptr, size_t size, size_t nmemb, FILE *stream);
FILE *fopen(const char *path, const char *mode);
int fclose(FILE *fp);
DIR *opendir(const char *name);
struct dirent *readdir(DIR *dirp);
int closedir(DIR *dirp);

#endif /* VFS_H */
