// mount -> open -> read -> close -> unmount
// no write for cramfs
#ifndef CRAMFS
#define CRAMFS

#include "vfs_lite.h" 
#include "driver.h"

#define DRIVER_NAME "cramfs"
#define NUM_INODES 4
#define DIR 0
#define FILE 1

extern void cramfs_register(); // fill out the 
extern void* cramfs_build_blob(); // this would not be here, the blob would be given already
extern void cramfs_parse_blob(void*); // this will called within mount, just parsing blob into structs but still referencing the blob for actual data
// void cramfs_open();
// void cramfs_close();
// void cramfs_read();
// void cramfs_lookup();
inode* cramfs_mount(void*);
void cramfs_unmount(inode*);

static FileSystemDriver cramfs_fsd = {
    .mount = cramfs_mount,
    .unmount = cramfs_unmount
};

static FileOps cramfs_ops = {
    .lookup = NULL,
    .open = NULL,
    .read = NULL,
    .write = NULL,
    .close = NULL
};

static inode inode_table[NUM_INODES]; // chapters

typedef struct Dirent{ // index of names
    int num_inodes;
    int inodes[2];
} Dirent;

typedef struct SuperBlock{ // table of contents
    int magic_num;
    size_t size;
    int inode_table_offset;
    int dirent_table_offset;
    int num_inodes;
}SuperBlock;

typedef struct FileData{
    int inode_num;
    size_t offset;
}FileData;


#endif /*CRAMFS*/