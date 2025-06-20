#ifndef VFS_LITE
#define VFS_LITE

#define MAX_FILE_PATH 4096
#define MAX_OPEN_FILES 256
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
    MountedFileSystem next_filesystem;
} MountedFileSystem;

// registered drivers on the system, hardcoded at this point
typedef struct RegisteredDriver{
    char name[16];
    struct FileSystemDriver* fsd;
} RegisteredDriver;

typedef struct file{
    inode* node;
    size_t offset;
    int flags;
}file;

static struct file fd_table[MAX_OPEN_FILES];
static struct RegisteredDriver driver_table[MAX_DRIVERS];
static int driver_count = 0;

void vfs_register_driver(const char* name, struct FileSystemDriver* fsd);
char *tokenize_path(char *);
void vfs_open(char* path, int flags);

#endif /*VFS_LITE*/