#include <stdio.h>    
#include <stdlib.h> 
#include <string.h>
#include "cramfs.h"

void cramfs_register(){    
    vfs_register_driver(CRAMFS_DRIVER_NAME, MAGIC_BYTES, &cramfs_fsd);
}

VFSSuperBlock* cramfs_mount(void* blob){
    VFSSuperBlock* vfs_super_block = cramfs_fill_super(blob);
    VFSInode* root_inode = cramfs_get_root_inode(blob, vfs_super_block);
    DirEntry* root_direntry = cramfs_make_direnty(blob, root_inode);
    root_direntry->d_superblock = vfs_super_block;
    vfs_super_block->s_root = root_direntry;
    return vfs_super_block;
}

DirEntry* cramfs_make_direnty(void* blob, VFSInode* inode){
    DirEntry* direntry = malloc(sizeof(DirEntry));
    CramfsInode* cramfs_inode = (CramfsInode*)inode->inode_info;
    CramfsDirent* cramfs_dirent = (CramfsDirent*)blob + cramfs_inode->data_offset;
    direntry->node = inode;
    direntry->parent = NULL;
    snprintf(direntry->name, MAX_NAME, "%s", cramfs_dirent->name);
    return direntry;
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
        FileSystemSource* fs = malloc(sizeof(FileSystemSource));
        fs->blob = blob;
        fs->fd = -1;
        vfs_super_block->s_src = fs;

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
    // check if type dir or file and set the right inode fop
    vfs_inode->i_op = &cramfs_dir_inode_op;
    vfs_inode->timestamp = 0;
    vfs_inode->sb = vfs_super_block;

    return vfs_inode;
}

VFSInode* cramfs_make_vfsinode(CramfsInode* inode, VFSSuperBlock* sb){
    VFSInode* vfs_inode = malloc(sizeof(VFSInode));

    vfs_inode->file_type = inode->type;
    vfs_inode->file_size = inode->data_size;
    vfs_inode->inode_number = inode->id;
    vfs_inode->timestamp = 0;
    vfs_inode->inode_info = inode;
    vfs_inode->f_op = &cramfs_op;
    vfs_inode->i_op = &cramfs_dir_inode_op;
    vfs_inode->sb = sb;

    return vfs_inode;
}

File* cramfs_open(VFSInode* node, int flags, char* filename){
    File* fd = (File*)malloc(sizeof(File));
    snprintf(fd->filename, 63, "%s", filename);
    fd->node = node;
    fd->flags = flags;
    fd->cursor_postion = 0;

    return fd;
}

// take dir and node. 
// check if node->type is file or dir
// then if dir, we go to data offset and loop and look for
// the dir name within the dirents, if none are found, grab the next
// dir and return the dir->inode
// then lookup again and do this recursively going down directory paths
// then whenever we get a node back, we add to cache
// if not dir and a file, then return that inode
VFSInode* cramfs_lookup(VFSInode* node, const char* file){
    CramfsInode* c_inode = (CramfsInode*)node->inode_info;
    char* blob = (char*)node->sb->s_src->blob;
    if(node->file_type == DIR){
        for(int i = 0; i < c_inode->data_size; i++){
            CramfsDirent* dirent = (CramfsDirent*)(blob + (c_inode->data_offset + (sizeof(CramfsDirent)*i)));
            if(strcmp(dirent->name, file) == 0){
                printf("Dirents %s\n",dirent->name);
                printf("File %s\n",file);
                return cramfs_make_vfsinode((CramfsInode*)(blob + dirent->inode_offset), (VFSSuperBlock*)node->sb);
            }
        }
    }
    else if (node->file_type == FILE){
        return node;
    }

    return NULL;
}


size_t cramfs_read(File* f, void* buf, size_t len){
    CramfsInode* c_inode = (CramfsInode*)f->node->inode_info;
    size_t offset = c_inode->data_offset + f->cursor_postion;
    char* data_ptr = f->node->sb->s_src->blob + offset; 
    memcpy(buf, data_ptr, len);
    f->cursor_postion += len;

    return len;
}

void cramfs_unmount(VFSSuperBlock* sb){
    free(sb->s_fs_info);
    free(sb->s_src);
    free(sb);
}


static inline void read_and_advance(void* dest, size_t size, char** ptr) {
    memcpy(dest, *ptr, size);
    *ptr += size;
}


void* cramfs_build_blob(){
    // "/", "/docs", "/hello.txt", "/docs/readme.md"
    void* blob = malloc(4096);
    void* write_ptr = blob;

    CramfsSuperBlock* sb = (CramfsSuperBlock*)malloc(sizeof(CramfsSuperBlock));
    sb->magic_num = 0xFEF;
    sb->num_inodes = 4;
    sb->inode_table_offset = sizeof(CramfsSuperBlock);
    sb->dirent_table_offset = sizeof(CramfsSuperBlock) + sizeof(blob_inode_table);
    sb->blob_size = sizeof(CramfsSuperBlock) + (sizeof(CramfsInode) * 4) + (sizeof(CramfsDirent)*3) + 13 + 15;
    sb->start_data_block_offset = sizeof(CramfsSuperBlock) + (sizeof(CramfsInode) * 4) + (sizeof(CramfsDirent)*3);

    memcpy(write_ptr, sb, sizeof(CramfsSuperBlock));
 
    write_ptr += sizeof(CramfsSuperBlock); // increment pointer

    blob_inode_table[0].id = 0;
    blob_inode_table[0].type = DIR;
    blob_inode_table[0].data_size = 2; // on dir, its the num of dirents
    blob_inode_table[0].data_offset = sb->dirent_table_offset;

    memcpy(write_ptr, &blob_inode_table[0], sizeof(CramfsInode));
    write_ptr += sizeof(CramfsInode);

    blob_inode_table[1].id = 1;
    blob_inode_table[1].type = DIR;
    blob_inode_table[1].data_size = 1;
    blob_inode_table[1].data_offset = sb->dirent_table_offset + sizeof(CramfsDirent)*2;

    memcpy(write_ptr, &blob_inode_table[1], sizeof(CramfsInode));
    write_ptr += sizeof(CramfsInode);

    blob_inode_table[2].id = 2;
    blob_inode_table[2].type = FILE;
    blob_inode_table[2].data_size = 13;
    blob_inode_table[2].data_offset = sb->start_data_block_offset;

    memcpy(write_ptr, &blob_inode_table[2], sizeof(CramfsInode));
    write_ptr += sizeof(CramfsInode);

    blob_inode_table[3].id = 3;
    blob_inode_table[3].type = FILE;
    blob_inode_table[3].data_size = 15;
    blob_inode_table[3].data_offset = sb->start_data_block_offset + 13;

    memcpy(write_ptr, &blob_inode_table[3], sizeof(CramfsInode));
    write_ptr += sizeof(CramfsInode);

    CramfsDirent* d1 = (CramfsDirent*)malloc(sizeof(CramfsDirent));
    // inode of docs
    d1->inode_offset = sb->inode_table_offset + sizeof(CramfsInode);
    snprintf(d1->name, MAX_NAME, "%s", "docs");

    memcpy(write_ptr, d1, sizeof(CramfsDirent));
    write_ptr += sizeof(CramfsDirent);

    CramfsDirent* d2 = (CramfsDirent*)malloc(sizeof(CramfsDirent));
    // inode of hello.txt -> points to the data of hello.txt
    d2->inode_offset = sb->inode_table_offset + (sizeof(CramfsInode)*2);
    snprintf(d2->name, MAX_NAME, "%s", "hello.txt");

    memcpy(write_ptr, d2, sizeof(CramfsDirent));
    write_ptr += sizeof(CramfsDirent);

    CramfsDirent* d3 = (CramfsDirent*)malloc(sizeof(CramfsDirent));
    d3->inode_offset = sb->inode_table_offset + sizeof(CramfsInode)*3;
    snprintf(d3->name, MAX_NAME, "%s", "readme.md");

    memcpy(write_ptr, d3, sizeof(CramfsDirent));
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


