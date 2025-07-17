#ifndef DRIVER
#define DRIVER

#include <stddef.h>
#include <fcntl.h>
#include <stdint.h>

#define MAX_FILE_PATH 50
#define MAX_INODES 10
#define MAX_NAME 255

typedef enum {
    SOURCE_TYPE_BLOB,
    SOURCE_TYPE_BLOCK
} SourceType;

typedef struct {
    SourceType type;
    union {
        void* blob;
        int fd;
    };
} FileSystemSource;

typedef struct DirEntry {
    char name[MAX_NAME];
    inode* node;  
} DirEntry;

typedef struct inode{
    size_t id;
    size_t type; // dir or file
    size_t size; 
    size_t data_offset; // data offset into the blob
    char path[MAX_FILE_PATH];
    struct FileOps* fops;
}inode;

typedef struct file{
    size_t id;
    char filename[64]; 
    inode* node;
    size_t file_postion; // offset to where we are pointing to
    int flags;
}file;

typedef struct FileOps {
    inode* (*lookup)(const char* name);
    file*  (*open)(inode* node, int flags);
    size_t        (*read)(file* f, void* buf, size_t len);
    size_t        (*write)(file* f, const void* buf, size_t len);
    int           (*close)(file* f);
} FileOps;

// call read_dir to get list of DirEntrys
typedef struct FileSystemDriver {
    inode* (*mount)(FileSystemSource* source);
    void (*unmount)(inode* root);
    int (*read_dir)(inode* dir_inode, struct DirEntry* out_entries, size_t max_entries);
    int (*is_directory)(inode* node);
}FileSystemDriver;

typedef struct {
    int block_fd;
    inode* root_inode;
    inode* inode_cache[MAX_INODES];  // optional
    // maybe a hashmap of name->inode for speed
} BlockDevice;


typedef struct {
    uint32_t magic;
    uint32_t total_blocks;
    uint32_t free_blocks;
    uint32_t total_inodes;
    uint32_t free_inodes;
    uint32_t block_size;
    uint32_t inode_bitmap_block;
    uint32_t block_bitmap_block;
    uint32_t inode_table_start;
    uint32_t data_block_start;
} SuperBlockDisk;


#endif /*DRIVER*/