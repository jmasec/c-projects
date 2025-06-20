#ifndef VFS_LITE
#define VFS_LITE

#include <stddef.h>

#define MAX_FILE_PATH 4096
#define MAX_OPEN_FILES 256
#define MAX_DRIVERS 8

#define MEMFS_PATH "mem"
#define EXT2_PATH "ext2"


// node for linked list of mount filesystems
// so eventually our register 
// I am gonna call the cramfs register function from main.c to get its file pointers, in actual OS, it puts its init function in .initcall (special linker section)
// in the final kernel mod binary for a driver
typedef struct MountedFileSystem{
    char* path;
    RegisteredDriver* driver; 
    MountedFileSystem next_filesystem;
} MountedFileSystem;

// file system operations from the driver
typedef struct FileSystemOps{
    struct inode* (*mount)(void*);
    struct inode* (*lookup)(struct inode*, const char*); // resolve inode of path
    struct file*  (*open)(struct inode*, int);
    size_t (*read)(struct file*, void*, size_t);
    size_t (*write)(struct file*, const void*, size_t);
    size_t (*close)(struct file*);
} FileSystemOps;

// registered drivers on the system, hardcoded at this point
typedef struct RegisteredDriver{
    char name[16];
    struct FileSystemOps* fops;
} RegisteredDriver;

typedef struct inode{
    unsigned int id;
    unsigned type;
    size_t size;
    RegisteredDriver* driver;
}inode;

typedef struct file{
    inode* node;
    size_t offset;
    int flags;
}file;

static struct file fd_table[MAX_OPEN_FILES];
static struct RegisteredDriver driver_table[MAX_DRIVERS];
static int driver_count = 0;

void vfs_register_driver(const char* name, struct FileSystemOps* fops);

#endif /*VFS_LITE*/