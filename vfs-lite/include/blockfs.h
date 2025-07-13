#include <stdint.h>
#include "driver.h"

#define DRIVER_NAME "blockfs"


void blockfs_register();

static FileSystemDriver blockfs_fsd = {
    .mount = NULL,
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
} SuperBlock;
