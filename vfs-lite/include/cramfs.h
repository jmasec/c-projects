// mount -> open -> read -> close -> unmount
// no write for cramfs
#ifndef CRAMFS
#define CRAMFS

#include "vfs_lite.h"

#define CRAMFS_DRIVER_NAME "cramfs"

#define MAX_QUEUE 100
#define NUM_INODES 4
#define MAGIC_BYTES 0xFEF
#define DIR 0
#define FILE 1


extern void cramfs_register(); // fill out the 
extern void* cramfs_build_blob(); // this would not be here, the blob would be given already
VFSInode* cramfs_get_root_inode(void* blob, VFSSuperBlock* vfs_super_block); // File* cramfs_open(Inode* node, int flags);
// size_t cramfs_read(File* f, void* buf, size_t len);
VFSInode* cramfs_lookup(VFSInode* node, char* file);
VFSSuperBlock* cramfs_mount(void* blob);
VFSSuperBlock* cramfs_fill_super(void* blob);
DirEntry* cramfs_make_direnty(void* blob, VFSInode* inode);
// void cramfs_unmount(); // this needs to free all of the memory
// FileSystemTreeNode* find_parent(FileSystemTreeNode* node, size_t id);
// void print_tree(FileSystemTreeNode* node);
static inline void read_and_advance(void* dest, size_t size, char** ptr);
File* cramfs_open(VFSInode* node, int flags, char* filename);
// Inode* build_inode(char** ptr);

// define global for tree structure, any other structs I need

static FileSystemDriver cramfs_fsd = {
    .mount = cramfs_mount,
    .fill_super = cramfs_fill_super,
    .unmount = NULL,
};

static VFSSuperOps cramfs_super_op = {
    .write_inode = NULL,
    .alloc_inode = NULL,
    .destroy_inode = NULL,
};

static FileOps cramfs_op = {
    .open = cramfs_open,
    .read = NULL,
    .write = NULL,
    .close = NULL
};

static InodeDirOps cramfs_dir_inode_op = {
    .lookup = cramfs_lookup,
};

typedef struct __attribute__((packed)) CramfsInode{
    size_t id;
    size_t type; // dir or file
    size_t data_size;
    size_t data_offset; // data offset into the blob or to dirents
}CramfsInode;

// these are all for building the blob, not really needed here
static CramfsInode blob_inode_table[NUM_INODES]; // chapters

// name and inode link, then you search the data block of the inode to find dirents
typedef struct __attribute__((packed)) CramfsDirent{ // index of names
    size_t inode_offset;
    char name[MAX_NAME];
} CramfsDirent;

typedef struct __attribute__((packed)) CramfsSuperBlock{ // table of contents
    int magic_num;
    size_t blob_size;
    size_t inode_table_offset;
    size_t dirent_table_offset;
    size_t num_inodes;
    size_t start_data_block_offset;
}CramfsSuperBlock;

VFSInode* cramfs_make_vfsinode(CramfsInode* inode, VFSSuperBlock* sb);


#endif /*CRAMFS*/