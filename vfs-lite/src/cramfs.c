#include <stdio.h>    
#include <stdlib.h> 
#include <string.h>
#include "cramfs.h"

inode* cramfs_mount(void*blob){
    cramfs_parse_blob(blob); // tree is made now
    return; // return first node of the tree root inode
}

void cramfs_parse_blob(void* blob){
    SuperBlock* sb = malloc(sizeof(SuperBlock));
    char* read_ptr = blob;
    char* inode_table_ptr = NULL;
    char* dirent_table_ptr = NULL;
    char* data_block_ptr = NULL;

    sb->magic_num = *(int*)read_ptr;
    read_ptr+= sizeof(int);
    sb->blob_size = *(size_t*)read_ptr;
    read_ptr+= sizeof(size_t);
    sb->inode_table_offset = *(size_t*)read_ptr;
    read_ptr+= sizeof(size_t);
    sb->dirent_table_offset = *(size_t*)read_ptr;
    read_ptr+= sizeof(size_t);
    sb->num_inodes = *(size_t*)read_ptr;
    read_ptr+= sizeof(size_t);
    sb->start_data_block_offset = *(size_t*)read_ptr;

    printf("MAGIC NUM: %x\n", sb->magic_num);
    printf("size: %i\n", sb->blob_size);

    inode_table_ptr = (char*)blob + sb->inode_table_offset;
    dirent_table_ptr = (char*)blob + sb->dirent_table_offset;
    data_block_ptr = (char*)blob + sb->start_data_block_offset;
    char* tmp = NULL;

    // later maybe build a hash or lookup table for my tree for faster lookup
    do{
        int node = *(int*)dirent_table_ptr; // get node for that dirent
        // loop through the number of inodes to make nodes
        // FileSystemTreeNode inode build for number node which we grab using
        // inode table pointer
        // parent->children = malloc(sizeof(FileSystemTreeNode*) * dirent->num_nodes);
        // then go into loop and setup the children
        // how do we handle the next dirent?
        for(int i = 0; i < (*(int*)dirent_table_ptr + sizeof(int)); i ++){
            tmp = inode_table_ptr + (sizeof(blob_inode)*i); // get inode num
        }
        
        int dirent_size = sizeof(int) + sizeof(int) + (sizeof(int)*(*(int*)dirent_table_ptr + sizeof(int)));
        dirent_table_ptr += dirent_size; // push to next dirent
    }while (dirent_table_ptr != data_block_ptr); // are we out of dirents?


    // printf("ptr: %x\n", inode_table_ptr);
    // printf("inode table offset: %i\n", sb->inode_table_offset);
    // printf("id: %i\n", *(size_t*)inode_table_ptr);
    // inode_table_ptr += sizeof(size_t);
    // printf("type: %i\n", *(size_t*)inode_table_ptr);
    // inode_table_ptr += sizeof(size_t);
    // printf("size: %i\n", *(size_t*)inode_table_ptr);
    // inode_table_ptr += sizeof(size_t);
    // printf("data offset: %i\n", *(size_t*)inode_table_ptr);
    // inode_table_ptr += sizeof(size_t);
    // printf("path: %s\n", (char*)inode_table_ptr);
}


void cramfs_register(){    
    vfs_register_driver(DRIVER_NAME, &cramfs_fsd);
}

void* cramfs_build_blob(){
    // "/", "/docs", "/hello.txt", "/docs/readme.md"
    void* blob = malloc(4096);
    void* write_ptr = blob;

    SuperBlock* sb = (SuperBlock*)malloc(sizeof(SuperBlock));
    sb->magic_num = 0xFEF;
    sb->num_inodes = 4;
    sb->inode_table_offset = sizeof(SuperBlock);
    sb->dirent_table_offset = sizeof(SuperBlock) + sizeof(blob_inode);
    sb->blob_size = sizeof(SuperBlock) + (sizeof(blob_inode) * 4) + (sizeof(Dirent)*2) + (sizeof(FileData)*2);
    sb->start_data_block_offset = sizeof(SuperBlock) + (sizeof(blob_inode) * 4) + (sizeof(Dirent)*2);

    memcpy(write_ptr, sb, sizeof(SuperBlock));
 
    write_ptr += sizeof(SuperBlock); // increment pointer

    blob_inode_table[0].id = 0;
    blob_inode_table[0].type = DIR;
    blob_inode_table[0].data_size = 0;
    blob_inode_table[0].data_offset = 0;
    snprintf(blob_inode_table[0].path, MAX_FILE_PATH, "%s", "/");

    memcpy(write_ptr, &blob_inode_table[0], sizeof(blob_inode));
    write_ptr += sizeof(blob_inode);

    blob_inode_table[1].id = 1;
    blob_inode_table[1].type = FILE;
    blob_inode_table[1].data_size = 13;
    blob_inode_table[1].data_offset = 0;
    snprintf(blob_inode_table[1].path, MAX_FILE_PATH, "%s", "/hello.txt");

    memcpy(write_ptr, &blob_inode_table[1], sizeof(blob_inode));
    write_ptr += sizeof(blob_inode);

    blob_inode_table[2].id = 0;
    blob_inode_table[2].type = DIR;
    blob_inode_table[2].data_size = 0;
    blob_inode_table[2].data_offset = 0;
    snprintf(blob_inode_table[2].path, MAX_FILE_PATH, "%s", "/docs");

    memcpy(write_ptr, &blob_inode_table[2], sizeof(blob_inode));
    write_ptr += sizeof(blob_inode);

    blob_inode_table[3].id = 1;
    blob_inode_table[3].type = FILE;
    blob_inode_table[3].data_size = 15;
    blob_inode_table[3].data_offset = 0;
    snprintf(blob_inode_table[3].path, MAX_FILE_PATH, "%s", "/docs/readme.md");

    memcpy(write_ptr, &blob_inode_table[3], sizeof(blob_inode));
    write_ptr += sizeof(blob_inode);

    Dirent* d1 = (Dirent*)malloc(sizeof(Dirent));
    d1->node = 0;
    d1->num_inodes = 2;
    d1->inodes[0] = 2;
    d1->inodes[1] = 1;

    memcpy(write_ptr, d1, sizeof(Dirent));
    write_ptr += sizeof(Dirent);

    Dirent* d2 = (Dirent*)malloc(sizeof(Dirent));
    d1->node = 1;
    d1->num_inodes = 1;
    d1->inodes[0] = 3;

    memcpy(write_ptr, d2, sizeof(Dirent));
    write_ptr += sizeof(Dirent);

    FileData* fd = (FileData*)malloc(sizeof(FileData));
    FileData* fd2 = (FileData*)malloc(sizeof(FileData));

    fd->inode_num = 2;
    fd->offset = (size_t)((write_ptr-blob) + sizeof(FileData));
    fd->size = 13;

    memcpy(write_ptr, fd, sizeof(FileData));
    write_ptr += sizeof(FileData);

    memcpy(write_ptr, "Hello, world!", 13);
    write_ptr += 13;

    fd2->inode_num = 3;
    fd2->offset = (size_t)((write_ptr-blob) + sizeof(FileData));
    fd2->size = 15;

    memcpy(write_ptr, fd2, sizeof(FileData));
    write_ptr += sizeof(FileData);

    memcpy(write_ptr, "# Documentation", 15);
    write_ptr += 15;

    free(sb);
    free(d1);
    free(d2);
    free(fd);
    free(fd2);

    return blob;
}


