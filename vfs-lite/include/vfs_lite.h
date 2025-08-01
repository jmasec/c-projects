#ifndef VFS_LITE
#define VFS_LITE

#define MAX_OPEN_FILES 256
#define MAX_MOUNTED_FILESYSTEMS 10
#define MAX_DRIVERS 8
#define MAX_NAME 64
#define MAX_FILES 255
#define MAX_PATH 100

#include <stddef.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include "hash_table.h"

typedef struct DirEntry DirEntry;
typedef struct FileOps FileOps;
typedef struct InodeDirOps InodeDirOps;
typedef struct File File;
typedef struct VFSSuperBlock VFSSuperBlock;

typedef struct VFSInode {
    unsigned long inode_number;
    size_t file_type; // dir or file
    size_t file_size; 
    size_t timestamp;
    VFSSuperBlock* sb;
    void   *inode_info; // handle data pointers/ block pointers, for dirs they hold ptrs to directory entry block
    struct FileOps* f_op; // read, write, etc.
    struct InodeDirOps* i_op; // lookup
}VFSInode;

typedef struct InodeDirOps {
    VFSInode* (*lookup)(VFSInode *dir, const char* name); // find file by name
    // VFSInode** (*read_dir)(const char* name); // return array of vfsinodes if directory
    // // called in context where we can build direntrys from then, arr[0] first part of path etc
    // VFSInode* (*create)(const char* name); // create new file
    // VFSInode* (*link)(const char* name); // creating hard link
    // VFSInode* (*unlink)(const char* name); // removing file or dir
    // VFSInode* (*symlink)(const char* name); //create symlink
    // VFSInode* (*mkdir)(const char* name); // create new dir
} InodeDirOps;

typedef struct FileOps {
    File*         (*open)(VFSInode* node, int flags, char* filename);
    size_t        (*read)(File* f, void* buf, size_t len);
    size_t        (*write)(File* f, const void* buf, size_t len);
    int           (*close)(File* f);
} FileOps;

// handle all of the tree operations or hash operations depending
typedef struct DirEntryOps {
    void          (*d_lookup)(void);
    void          (*d_hash)(const char* name, DirEntry* direntry);
} DirEntryOps;

typedef struct FileSystemSource{
    int fd;
    void* blob;
} FileSystemSource;

typedef struct VFSSuperOps {
       int (*write_inode) (struct inode *);
       struct inode *(*alloc_inode)();
       void (*destroy_inode)(struct inode *);
}VFSSuperOps;

// gonna have the driver handle the opening of fd to the disk img or the blob
typedef struct VFSSuperBlock {
    unsigned char           s_dirt;             /* dirty flag */
    char                    *s_type;            /* filesystem type */
    VFSSuperOps             *s_op;              /* superblock methods */
    unsigned long           s_magic;            /* filesystem’s magic number */
    DirEntry                *s_root;            /* directory mount point */
    void                    *s_fs_info;         /* filesystem private info */
    FileSystemSource        *s_src;
}VFSSuperBlock;

typedef struct File{
    unsigned long id;
    char filename[MAX_NAME];
    VFSInode* node;
    size_t cursor_postion; // offset to where we are pointing to. Can use this as ofset into blob and block num maybe
    unsigned long flags;
}File;

typedef struct DirEntry {
    VFSInode* node;
    //DirEntry* parent;
    char name[MAX_NAME];
    //DirEntryOps* d_op;
    //VFSSuperBlock* d_superblock;
} DirEntry;

// registered drivers on the system, hardcoded at this point
typedef struct RegisteredDriver{
    char name[MAX_NAME];
    unsigned long magic_bytes;
    struct FileSystemDriver* fsd;
} RegisteredDriver;

typedef struct MountedFileSystem {
    char mount_path[MAX_PATH];                  // e.g. "/mnt/minifs"
    RegisteredDriver* driver;             // FS driver (minifs, cramfs, etc.)
    VFSInode* root_inode;
    VFSSuperBlock* super_block;
    HT *hash_table;
} MountedFileSystem;


// call read_dir to get list of DirEntrys
typedef struct FileSystemDriver {
    VFSInode* (*mount)(); // return root inode
    VFSSuperBlock* (*fill_super)(void*); // fill super block out
    void (*unmount)(VFSInode* root);
}FileSystemDriver;

extern File* fd_table[MAX_OPEN_FILES];
extern RegisteredDriver driver_table[MAX_DRIVERS];
extern MountedFileSystem mount_table[MAX_MOUNTED_FILESYSTEMS];

extern unsigned long driver_count;
extern unsigned long mount_count;
extern unsigned long fd_count;

void vfs_register_driver(const char* name, unsigned long magic_bytes, struct FileSystemDriver* fsd);
void vfs_mount(char* mount_path, char* filesystem_name, void* filesystem);
void vfs_unmount(char* mount_path);
File* vfs_open(char* path, int flags);
size_t vfs_read(File* fd, void* buf, size_t size);
int vfs_close(File** fd);
RegisteredDriver* get_driver(char* fs_name);
VFSInode* get_inode_of_file(VFSInode* node, char* path, HT* hash_table);
char* get_last_token(char* str, char delimiter);
DirEntry* node_from_cache(char* key, HT* table);
int build_direntry(HT* table, VFSInode* node, char* filename);

/*
For performance I want to implement a cache for inodes and filenames so
that I dont have to parse from root all the time
*/

/*
For our tree, we are going to build nodes based on whats accessed not all on mount
which will basically be our cache since itll be in RAM and faster than disk

if cramfs magic num, we just read the whole blob and make the whole tree on mount
else block then we dont do that
*/

#endif /*VFS_LITE*/