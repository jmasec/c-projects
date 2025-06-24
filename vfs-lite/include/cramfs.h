// mount -> open -> read -> close -> unmount
// no write for cramfs
#ifndef CRAMFS
#define CRAMFS

#include "vfs_lite.h" 
#include "driver.h"

#define DRIVER_NAME "cramfs"
#define NUM_INODES 10

static FileSystemDriver cramfs_fsd = {
    .mount = cramfs_mount,
    .unmount = cramfs_unmount
};

static FileOps cramfs_ops = {
    .lookup = cramfs_lookup,
    .open = cramfs_open,
    .read = cramfs_read,
    .write = NULL,
    .close = cramfs_close
};

static inode inode_table[NUM_INODES];

typedef struct Dirent{
    char* name;
    int inode_num;
} Dirent;

typedef struct FileData{
    int inode_num;
}FileData;

extern void cramfs_register(); // fill out the 
extern void* cramfs_build_blob(); // this would not be here, the blob would be given already
void cramfs_parse_blob(); // this will called within mount, just parsing blob into structs but still referencing the blob for actual data
void cramfs_open();
void cramfs_close();
void cramfs_read();
inode* cramfs_mount(void*);
void cramfs_unmount(inode*);
void cramfs_lookup();

#endif /*CRAMFS*/