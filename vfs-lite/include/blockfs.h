#define BLOCKFS_DRIVER_NAME "blockfs"
#define MAGIC_BYTES 0x4D465346 
#define BLOCK_SIZE 512
#define FILENAME_MAX 28

#define INODE_TYPE_FILE 1
#define INODE_TYPE_DIR  2

#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include "vfs_lite.h"

extern void blockfs_register();
// VFSInode* blockfs_parse_disk(int);
VFSSuperBlock* blockfs_mount(void* fd);
static inline void read_and_advance(void* dest, size_t size, char** ptr);
VFSSuperBlock* blockfs_fill_super(int fd);

static FileSystemDriver blockfs_fsd = {
    .mount = blockfs_mount,
    .fill_super = NULL,
    .unmount = NULL,
};

typedef struct BlockSuperblock {
    int magic;
    uint32_t total_blocks;
    uint32_t total_inodes;
    uint32_t block_size;
    uint32_t inode_table_start;  // in blocks
    uint32_t data_block_start;   // in blocks
    uint32_t root_inode;         // inode number
    uint32_t inode_bitmap_start;
    uint32_t block_bitmap_start;
    uint8_t* block_bitmap;
    uint8_t* inode_bitmap;
} BlockSuperblock;

typedef struct BlockInode {
    uint32_t type;       // File or directory
    uint32_t size;       // Size in bytes
    uint32_t direct[4];  // Block numbers for file/dir contents
} BlockInode;

typedef struct BlockDirent {
    uint32_t inode_num;
    char name[FILENAME_MAX]; // null-terminated
} BlockDirent;

