// mount -> open -> read -> close -> unmount
// no write for cramfs
#ifndef CRAMFS
#define CRAMFS

#include "vfs_lite.h" 
#include "driver.h"

#define DRIVER_NAME "cramfs"
#define NUM_INODES 4
#define MAGIC_BYTES 0xFEF
#define DIR 0
#define FILE 1

typedef struct FileSystemTreeNode FileSystemTreeNode;

extern void cramfs_register(); // fill out the 
extern void* cramfs_build_blob(); // this would not be here, the blob would be given already
extern void cramfs_parse_blob(void*); // this will called within mount, just parsing blob into structs but still referencing the blob for actual data
// this will parse it into a tree using the structures in the blob
// void cramfs_open();
// void cramfs_close();
// void cramfs_read();
// void cramfs_lookup();
inode* cramfs_mount(void*);
void cramfs_unmount(inode*);
FileSystemTreeNode* find_parent(FileSystemTreeNode* node, size_t id);

typedef struct FileSystemTreeNode{
    inode *node; // inode 0 lets say
    size_t num_children;
    FileSystemTreeNode** children; // children docs and hello.txt
} FileSystemTreeNode;

// define global for tree structure, any other structs I need

static FileSystemDriver cramfs_fsd = {
    .mount = cramfs_mount,
    .unmount = NULL,
};

static FileOps cramfs_ops = {
    .lookup = NULL,
    .open = NULL,
    .read = NULL,
    .write = NULL,
    .close = NULL
};

typedef struct __attribute__((packed)) blob_inode{
    size_t id;
    size_t type; // dir or file
    size_t data_size; 
    size_t data_offset; // data offset into the blob
    char path[MAX_FILE_PATH];
}blob_inode;

// these are all for building the blob, not really needed here
static blob_inode blob_inode_table[NUM_INODES]; // chapters

typedef struct __attribute__((packed)) Dirent{ // index of names
    size_t node;
    size_t num_inodes;
    size_t inodes[2];
} Dirent;

typedef struct __attribute__((packed)) SuperBlock{ // table of contents
    int magic_num;
    size_t blob_size;
    size_t inode_table_offset;
    size_t dirent_table_offset;
    size_t num_inodes;
    size_t start_data_block_offset;
}SuperBlock;


#endif /*CRAMFS*/