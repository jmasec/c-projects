#ifndef VFS_LITE
#define VFS_LITE

#define MAX_OPEN_FILES 256
#define MAX_MOUNTED_FILESYSTEMS 10
#define MAX_DRIVERS 8

#define MEMFS_PATH "mem"
#define EXT2_PATH "ext2"
#define DELIMINATOR "/"

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

extern file* fd_table[MAX_OPEN_FILES];
extern RegisteredDriver driver_table[MAX_DRIVERS];
extern MountedFileSystem mount_table[MAX_MOUNTED_FILESYSTEMS];

extern int driver_count;
extern int mount_count;
extern int fd_count;

void vfs_register_driver(const char* name, struct FileSystemDriver* fsd);
void vfs_mount(char* mount_path, char* fs_name, void* blob);
void vfs_unmount(char* mount_path);
file* vfs_open(char* path, int flags);
int vfs_read(file* fd, void* buf, size_t size);
int vfs_close(file** fd);
MountedFileSystem* find_prefix_mount(char *path);
RegisteredDriver* get_driver(char* fs_name);

/*
For performance I want to implement a cache for inodes and filenames so
that I dont have to parse from root all the time
*/

#endif /*VFS_LITE*/