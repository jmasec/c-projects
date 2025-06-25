#ifndef VFS_LITE
#define VFS_LITE

#define MAX_FILE_PATH 4096
#define MAX_OPEN_FILES 256
#define MAX_MOUNTED_FILESYSTEMS 10
#define MAX_DRIVERS 8

#define MEMFS_PATH "mem"
#define EXT2_PATH "ext2"

#include "driver.h"


// registered drivers on the system, hardcoded at this point
typedef struct RegisteredDriver{
    char name[16];
    int magic_bytes;
    struct FileSystemDriver* fsd;
} RegisteredDriver;

typedef struct MountedFileSystem{
    char mount_path[64];
    inode* root;
    RegisteredDriver* driver; 
} MountedFileSystem;

typedef struct file{
    char filename[64]; 
    inode* node;
    size_t offset;
    int flags;
}file;

extern file fd_table[MAX_OPEN_FILES];
extern RegisteredDriver driver_table[MAX_DRIVERS];
extern MountedFileSystem mount_table[MAX_MOUNTED_FILESYSTEMS];

extern int driver_count;
extern int mount_count;
extern int fd_count;

void vfs_register_driver(const char* name, struct FileSystemDriver* fsd);
int vfs_mount(char* mount_path, char* fs_name, void* blob);
char *tokenize_path(char *);
file* vfs_open(char* path, int flags);
MountedFileSystem* find_prefix_mount(char *path);
// void vfs_close(char* path);

/*
For performance I want to implement a cache for inodes and filenames so
that I dont have to parse from root all the time
*/

#endif /*VFS_LITE*/