#ifndef VFS_LITE
#define VFS_LITE

#define MAX_FILE_PATH 4096
#define MAX_OPEN_FILES 256
#define MAX_MOUNTED_FILESYSTEMS 10
#define MAX_DRIVERS 8

#define MEMFS_PATH "mem"
#define EXT2_PATH "ext2"

#include "driver.h"


// node for linked list of mount filesystems
// so eventually our register 
// I am gonna call the cramfs register function from main.c to get its file pointers, in actual OS, it puts its init function in .initcall (special linker section)
// in the final kernel mod binary for a driver
typedef struct MountedFileSystem{
    const char* mount_path;
    inode* root;
    RegisteredDriver* driver; 
} MountedFileSystem;

// registered drivers on the system, hardcoded at this point
typedef struct RegisteredDriver{
    char name[16];
    struct FileSystemDriver* fsd;
} RegisteredDriver;

typedef struct file{
    char* filename; 
    inode* node;
    size_t offset;
    int flags;
}file;

static file fd_table[MAX_OPEN_FILES];
static RegisteredDriver driver_table[MAX_DRIVERS];
static MountedFileSystem mount_table[MAX_MOUNTED_FILESYSTEMS];
static int driver_count = 0;
static int mount_count = 0;
static int fd_count = 0;

void vfs_register_driver(const char* name, struct FileSystemDriver* fsd);
void vfs_mount(char* mount_path);
char *tokenize_path(char *);
file* vfs_open(char* path, int flags);
void vfs_close(char* path);

/*
For performance I want to implement a cache for inodes and filenames so
that I dont have to parse from root all the time
*/

#endif /*VFS_LITE*/