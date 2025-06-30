#ifndef DRIVER
#define DRIVER

#include <stddef.h>

#define O_RDONLY  0x01   
#define O_WRONLY  0x02 

#define MAX_FILE_PATH 50

typedef struct inode{
    size_t id;
    size_t type; // dir or file
    size_t size; 
    size_t data_offset; // data offset into the blob
    char path[MAX_FILE_PATH];
    struct FileOps* fops;
}inode;

typedef struct file{
    char filename[64]; 
    inode* node;
    size_t file_postion; // offset to where we are pointing to
    int flags;
}file;

typedef struct FileSystemDriver {
    inode* (*mount)(void* blob);
    void (*unmount)(inode* root);
}FileSystemDriver;

typedef struct FileOps {
    inode* (*lookup)(inode* dir, const char* name);
    file*  (*open)(inode* node, int flags);
    size_t        (*read)(file* f, void* buf, size_t len);
    size_t        (*write)(file* f, const void* buf, size_t len);
    int           (*close)(file* f);
} FileOps;

#endif /*DRIVER*/