#ifndef DRIVER
#define DRIVER

#include <stddef.h>

struct file;
struct inode;

typedef struct FileSystemDriver {
    struct inode* (*mount)(void* blob);
    void (*unmount)(struct inode* root);
}FileSystemDriver;

typedef struct FileOps {
    struct inode* (*lookup)(struct inode* dir, const char* name);
    struct file*  (*open)(struct inode* node, int flags);
    size_t        (*read)(struct file* f, void* buf, size_t len);
    size_t        (*write)(struct file* f, const void* buf, size_t len);
    int           (*close)(struct file* f);
} FileOps;

typedef struct inode{
    unsigned int id;
    unsigned type;
    size_t size; 
    struct FileSystemOps* fops;
}inode;

#endif /*DRIVER*/