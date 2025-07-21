#include <stdio.h>    
#include <stdlib.h> 
#include <string.h>
#include "cramfs.h"

void cramfs_register(){    
    vfs_register_driver(CRAMFS_DRIVER_NAME, MAGIC_BYTES, &cramfs_fsd);
}

VFSInode* cramfs_mount(void* blob){
    VFSSuperBlock* vfs_super_block = cramfs_fill_super(blob);
    return cramfs_get_root_inode(blob, vfs_super_block);
}

VFSSuperBlock* cramfs_fill_super(void* blob){
        CramfsSuperBlock* sb = malloc(sizeof(CramfsSuperBlock));
        VFSSuperBlock* vfs_super_block = malloc(sizeof(VFSSuperBlock));
        char* read_ptr = blob;

        read_and_advance(&(sb->magic_num), sizeof(int), &read_ptr);
        read_and_advance(&(sb->blob_size), sizeof(size_t), &read_ptr);
        read_and_advance(&(sb->inode_table_offset), sizeof(size_t), &read_ptr);
        read_and_advance(&(sb->dirent_table_offset), sizeof(size_t), &read_ptr);
        read_and_advance(&(sb->num_inodes), sizeof(size_t), &read_ptr);
        read_and_advance(&(sb->start_data_block_offset), sizeof(size_t), &read_ptr);

        vfs_super_block->s_fs_info = sb;
        vfs_super_block->s_type = CRAMFS_DRIVER_NAME;
        vfs_super_block->s_dirt = 1;
        vfs_super_block->s_op = &cramfs_super_op;
        vfs_super_block->s_magic = MAGIC_BYTES;
        vfs_super_block->s_root = NULL;

        printf("MAGIC NUM: %x\n", sb->magic_num);
        printf("size: %d\n", sb->blob_size);
        printf("num nodes: %d\n", sb->num_inodes);

        return vfs_super_block;
}

// parse out parts into functions, parse sb, parse inode, make node
VFSInode* cramfs_get_root_inode(void* blob, VFSSuperBlock* vfs_super_block){
    char* inode_table_ptr = NULL;

    CramfsSuperBlock* cramfs_super_block = (CramfsSuperBlock*) vfs_super_block->s_fs_info;

    inode_table_ptr = (char*)blob + cramfs_super_block->inode_table_offset;

    VFSInode* vfs_inode = malloc(sizeof(VFSInode));
    CramfsInode* cramfs_inode = malloc(sizeof(CramfsInode));

    cramfs_inode = (CramfsInode*)inode_table_ptr;

    vfs_inode->inode_info = cramfs_inode;

    vfs_inode->file_size = cramfs_inode->data_size;
    vfs_inode->file_type = cramfs_inode->type;
    vfs_inode->inode_number = cramfs_inode->id;
    vfs_inode->f_op = &cramfs_op;
    vfs_inode->i_op = &cramfs_inode_op;
    vfs_inode->timestamp = 0;

    free(cramfs_super_block);
    free(vfs_super_block);

    return vfs_inode;
}

// void print_tree(FileSystemTreeNode* node){
//     if (node == NULL) return;

//     if(node->num_children == 0){
//         printf("Node: %d\n", node->node->id);
//         return;
//     }

//     printf("Node: %d\n", node->node->id);
//     for(int i = 0; i < node->num_children; i++){
//         print_tree(node->children[i]);
//     }
// }

// void free_tree(FileSystemTreeNode* node){
//     if (node == NULL) return;

//     for(int i = 0; i < node->num_children; i++){
//         free_tree(node->children[i]);
//     }

//     free(node->children);  
//     free(node->node);     
//     free(node);      
// }

// size_t cramfs_read(File* f, void* buf, size_t len){
//     // need to fix, offset to data not right
//     size_t offset = f->node->data_offset;
//     char* data_ptr = file_blob + offset; 
//     memcpy(buf, data_ptr, len);
//     f->cursor_postion = len;

//     return len;
// }

// File* cramfs_open(VFSInode* node, int flags){
//     char* tok = strtok(node->path, "/");
//     File* fd = (File*)malloc(sizeof(File));
//     snprintf(fd->filename, 63, "%s", tok);
//     fd->node = node;
//     fd->flags = flags;
//     fd->cursor_postion = 0;

//     return fd;
// }

// VFSInode* cramfs_lookup(char* file_path){
//     if(root == NULL)return;

//     FileSystemTreeNode* queue[MAX_QUEUE];
//     size_t front = 0; //dequeue
//     size_t rear = 0; // enqueue

//     queue[rear++] = root;

//     while (front < rear){
//         FileSystemTreeNode* curr_node = queue[front++];

//         if(strcmp(curr_node->node->path, file_path) == 0){
//             return curr_node->node;
//         }

//         for(int i = 0; i < curr_node->num_children; i++){
//             queue[rear++] = curr_node->children[i];
//         }
//     }
//     return NULL;

// }

// void cramfs_unmount(){
//     free_tree(root);
//     free(file_blob);
// }

// Inode* build_inode(char** ptr){
//     Inode* node = malloc(sizeof(Inode));
//     read_and_advance(&(node->id), sizeof(size_t), ptr);
//     read_and_advance(&(node->type), sizeof(size_t), ptr);
//     read_and_advance(&(node->size), sizeof(size_t), ptr);
//     read_and_advance(&(node->data_offset), sizeof(size_t), ptr);
//     snprintf(node->path, MAX_FILE_PATH, "%s", *ptr);
//     node->fops = &cramfs_ops;
//     return node;
// }



static inline void read_and_advance(void* dest, size_t size, char** ptr) {
    memcpy(dest, *ptr, size);
    *ptr += size;
}

// FileSystemTreeNode* find_parent(FileSystemTreeNode* node, size_t id){
//     if(node->num_children == 0){
//         return;
//     }

//     for(int i = 0; i < node->num_children; i++){
//         if(node->children[i]->node->id != id){
//             // if its not our node, then grab that cild node and look at its children and do that for all children
//             find_parent(node->children[i], id);
//         }
//         else{
//             return node->children[i];
//         }
//     }
// }


void* cramfs_build_blob(){
    // "/", "/docs", "/hello.txt", "/docs/readme.md"
    void* blob = malloc(4096);
    void* write_ptr = blob;

    CramfsSuperBlock* sb = (CramfsSuperBlock*)malloc(sizeof(CramfsSuperBlock));
    sb->magic_num = 0xFEF;
    sb->num_inodes = 4;
    sb->inode_table_offset = sizeof(CramfsSuperBlock);
    sb->dirent_table_offset = sizeof(CramfsSuperBlock) + sizeof(blob_inode_table);
    sb->blob_size = sizeof(CramfsSuperBlock) + (sizeof(CramfsInode) * 4) + (sizeof(CramfsDirent)*2) + 13 + 15;
    sb->start_data_block_offset = sizeof(CramfsSuperBlock) + (sizeof(CramfsInode) * 4) + (sizeof(CramfsDirent)*2);

    memcpy(write_ptr, sb, sizeof(CramfsSuperBlock));
 
    write_ptr += sizeof(CramfsSuperBlock); // increment pointer

    blob_inode_table[0].id = 0;
    blob_inode_table[0].type = DIR;
    blob_inode_table[0].data_size = 0;
    blob_inode_table[0].data_offset = 0;
    snprintf(blob_inode_table[0].path, MAX_FILE_PATH, "%s", "/");

    memcpy(write_ptr, &blob_inode_table[0], sizeof(CramfsInode));
    write_ptr += sizeof(CramfsInode);

    blob_inode_table[1].id = 1;
    blob_inode_table[1].type = DIR;
    blob_inode_table[1].data_size = 0;
    blob_inode_table[1].data_offset = 0;
    snprintf(blob_inode_table[1].path, MAX_FILE_PATH, "%s", "/docs");

    memcpy(write_ptr, &blob_inode_table[1], sizeof(CramfsInode));
    write_ptr += sizeof(CramfsInode);

    blob_inode_table[2].id = 2;
    blob_inode_table[2].type = FILE;
    blob_inode_table[2].data_size = 13;
    snprintf(blob_inode_table[2].path, MAX_FILE_PATH, "%s", "/hello.txt");
    blob_inode_table[2].data_offset = sb->start_data_block_offset;

    memcpy(write_ptr, &blob_inode_table[2], sizeof(CramfsInode));
    write_ptr += sizeof(CramfsInode);

    blob_inode_table[3].id = 3;
    blob_inode_table[3].type = FILE;
    blob_inode_table[3].data_size = 15;
    snprintf(blob_inode_table[3].path, MAX_FILE_PATH, "%s", "/docs/readme.md");
    blob_inode_table[3].data_offset = sb->start_data_block_offset + 13;

    memcpy(write_ptr, &blob_inode_table[3], sizeof(CramfsInode));
    write_ptr += sizeof(CramfsInode);

    CramfsDirent* d1 = (CramfsDirent*)malloc(sizeof(CramfsDirent));
    d1->node = 0;
    d1->num_inodes = 2;
    d1->inodes[0] = 2;
    d1->inodes[1] = 1;

    memcpy(write_ptr, d1, sizeof(CramfsDirent));
    write_ptr += sizeof(CramfsDirent);

    CramfsDirent* d2 = (CramfsDirent*)malloc(sizeof(CramfsDirent));
    d2->node = 1;
    d2->num_inodes = 1;
    d2->inodes[0] = 3;

    memcpy(write_ptr, d2, sizeof(CramfsDirent));
    write_ptr += sizeof(CramfsDirent);

    memcpy(write_ptr, "Hello, world!", 13);
    write_ptr += 13;

    memcpy(write_ptr, "# Documentation", 15);
    write_ptr += 15;

    free(sb);
    free(d1);
    free(d2);

    return blob;
}


