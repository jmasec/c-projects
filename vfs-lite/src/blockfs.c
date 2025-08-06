#include <stdio.h>    
#include <stdlib.h> 
#include <string.h>
#include "blockfs.h"

void blockfs_register(){    
    vfs_register_driver(BLOCKFS_DRIVER_NAME, MAGIC_BYTES, &blockfs_fsd);
}

int is_block_used(uint8_t* bitmap, size_t block_number) {
    size_t byte_index = block_number / 8;
    size_t bit_offset = block_number % 8;
    return (bitmap[byte_index] >> bit_offset) & 1;
}


VFSSuperBlock* blockfs_mount(void* fd){
    int file_d = *(int*)fd;
    VFSSuperBlock* vfs_super_block = blockfs_fill_super(file_d);
}

DirEntry* blockfs_make_direntry(int fd, VFSInode* inode){

}

VFSInode* blockfs_get_root_inode(VFSSuperBlock* sb){

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
    read_and_advance(&(block_sb->block_size), sizeof(uint32_t), &ptr_block0);
    read_and_advance(&(block_sb->total_blocks), sizeof(uint32_t), &ptr_block0);
    read_and_advance(&(block_sb->inode_table_start), sizeof(uint32_t), &ptr_block0);
    read_and_advance(&(block_sb->data_block_start), sizeof(uint32_t), &ptr_block0);
    read_and_advance(&(block_sb->root_inode), sizeof(uint32_t), &ptr_block0);

    block_sb->inode_bitmap_start = 2;
    block_sb->block_bitmap_start = 1;

    printf("Magic num: %x\n", block_sb->magic);
    printf("Num blocks: %i\n", block_sb->total_blocks);
    printf("block size: %i\n", block_sb->block_size);
    printf("block inode: %i\n", block_sb->inode_table_start);
    printf("data block: %i\n", block_sb->data_block_start);
    printf("inode num: %i\n", block_sb->root_inode);

    char block1[BLOCK_SIZE];

    ssize_t read_sb_1 = read(fd, block1, sizeof(block1));

    char* ptr_block1 = block1;

    block_sb->block_bitmap = (uint8_t*)ptr_block1;

    for (int i = 0; i < 20; i++) {
        printf("Block %d used? %s\n", i,
        is_block_used(block_sb->block_bitmap, i) ? "yes" : "no");
    }

    printf("block bitmap: %x", *block_sb->block_bitmap);

    char block2[BLOCK_SIZE];

    ssize_t read_sb_2 = read(fd, block2, sizeof(block2));

    char* ptr_block2 = block2;

    block_sb->inode_bitmap = (uint8_t*)ptr_block2;

    for (int i = 0; i < 20; i++) {
        printf("Inode %d used? %s\n", i,
        is_block_used(block_sb->inode_bitmap, i) ? "yes" : "no");
    }

    printf("inode bitmap: %x", *block_sb->inode_bitmap);

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