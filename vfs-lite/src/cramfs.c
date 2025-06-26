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
    void* read_ptr = blob;

    sb->magic_num = *(int*)read_ptr;
    read_ptr+= sizeof(int);
    sb->size = *(size_t*)read_ptr;
    read_ptr+= sizeof(size_t);
    sb->inode_table_offset = *(size_t*)read_ptr;
    read_ptr+= sizeof(size_t);
    sb->dirent_table_offset = *(size_t*)read_ptr;
    read_ptr+= sizeof(size_t);
    sb->num_inodes = *(size_t*)read_ptr;
    read_ptr+= sizeof(size_t);

    read_ptr += sb->inode_table_offset;
    printf("MAGIC NUM: %x\n", sb->magic_num);
    printf("size: %i\n", sb->size);
    printf("offset: %i\n", sb->inode_table_offset);
    printf("ID: %i\n", *(size_t*)read_ptr);
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
    sb->size = sizeof(SuperBlock) + (sizeof(blob_inode) * 4) + (sizeof(Dirent)*2) + (sizeof(FileData)*2);

    memcpy(write_ptr, sb, sizeof(SuperBlock));
 
    write_ptr += sizeof(SuperBlock); // increment pointer

    blob_inode_table[0].id = 0;
    blob_inode_table[0].type = DIR;
    blob_inode_table[0].size = 0;
    blob_inode_table[0].data_offset = 0;
    snprintf(blob_inode_table[0].path, MAX_FILE_PATH, "%s", "/");

    memcpy(write_ptr, &blob_inode_table[0], sizeof(blob_inode));
    write_ptr += sizeof(blob_inode);

    blob_inode_table[1].id = 1;
    blob_inode_table[1].type = FILE;
    blob_inode_table[1].size = 13;
    blob_inode_table[1].data_offset = 0;
    snprintf(blob_inode_table[1].path, MAX_FILE_PATH, "%s", "/hello.txt");

    memcpy(write_ptr, &blob_inode_table[1], sizeof(blob_inode));
    write_ptr += sizeof(blob_inode);

    blob_inode_table[2].id = 0;
    blob_inode_table[2].type = DIR;
    blob_inode_table[2].size = 0;
    blob_inode_table[2].data_offset = 0;
    snprintf(blob_inode_table[2].path, MAX_FILE_PATH, "%s", "/docs");

    memcpy(write_ptr, &blob_inode_table[2], sizeof(blob_inode));
    write_ptr += sizeof(blob_inode);

    blob_inode_table[3].id = 1;
    blob_inode_table[3].type = FILE;
    blob_inode_table[3].size = 15;
    blob_inode_table[3].data_offset = 0;
    snprintf(blob_inode_table[3].path, MAX_FILE_PATH, "%s", "/docs/readme.md");

    memcpy(write_ptr, &blob_inode_table[3], sizeof(blob_inode));
    write_ptr += sizeof(blob_inode);

    Dirent* d1 = (Dirent*)malloc(sizeof(Dirent));
    d1->num_inodes = 2;
    d1->inodes[0] = 2;
    d1->inodes[1] = 1;

    memcpy(write_ptr, d1, sizeof(Dirent));
    write_ptr += sizeof(Dirent);

    Dirent* d2 = (Dirent*)malloc(sizeof(Dirent));
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


