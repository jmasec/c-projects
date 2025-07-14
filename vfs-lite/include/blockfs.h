#include <stdint.h>
#include "driver.h"

#define DRIVER_NAME "blockfs"
#define MAGIC_NUM 0x4D465346 
#define BLOCK_SIZE 512

extern void blockfs_register();
inode* blockfs_parse_disk(int);
inode* blockfs_mount(FileSystemSource* soruce);

static FileSystemDriver blockfs_fsd = {
    .mount = blockfs_mount,
    .unmount = NULL,
};

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
