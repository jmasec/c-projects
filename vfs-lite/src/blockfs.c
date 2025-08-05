#include <stdio.h>    
#include <stdlib.h> 
#include <string.h>
#include "blockfs.h"

void blockfs_register(){    
    vfs_register_driver(BLOCKFS_DRIVER_NAME, MAGIC_BYTES, &blockfs_fsd);
}

VFSSuperBlock* blockfs_mount(void* fd){
    int file_d = *(int*)fd;
    VFSSuperBlock* vfs_super_block = blockfs_fill_super(file_d);
}

VFSSuperBlock* blockfs_fill_super(int fd){
    BlockSuperblock* block_sb = malloc(sizeof(BlockSuperblock));
    VFSSuperBlock* vfs_sb = malloc(sizeof(VFSSuperBlock));

    char block0[BLOCK_SIZE];

    ssize_t read_sb = read(fd, block0, sizeof(block0));

    if(read_sb == -1){
        perror("Failed reading from disk");
        //return NULL;
        exit(EXIT_FAILURE);
    }

    char* ptr_block0 = block0;

    read_and_advance(&(block_sb->magic), sizeof(int), &ptr_block0);
    read_and_advance(&(block_sb->total_blocks), sizeof(uint32_t), &ptr_block0);
    read_and_advance(&(block_sb->block_size), sizeof(uint32_t), &ptr_block0);
    read_and_advance(&(block_sb->inode_table_start), sizeof(uint32_t), &ptr_block0);
    read_and_advance(&(block_sb->data_block_start), sizeof(uint32_t), &ptr_block0);
    read_and_advance(&(block_sb->root_inode), sizeof(uint32_t), &ptr_block0);

    vfs_sb->s_dirt = 0;
    vfs_sb->s_type = BLOCKFS_DRIVER_NAME;
    vfs_sb->s_magic = MAGIC_BYTES;

    FileSystemSource* fs = malloc(sizeof(FileSystemSource));
    fs->blob = NULL;
    fs->fd = fd;
    vfs_sb->s_src = fs;

}

static inline void read_and_advance(void* dest, size_t size, char** ptr) {
    memcpy(dest, *ptr, size);
    *ptr += size;
}