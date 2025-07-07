#include <stdio.h>    
#include <stdlib.h> 
#include <string.h>
#include "cramfs.h"

FileSystemTreeNode* root = NULL;
void* file_blob = NULL;

void print_tree(FileSystemTreeNode* node){
    if(node->num_children == 0){
        printf("Node: %d\n", node->node->id);
        return;
    }

    printf("Node: %d\n", node->node->id);
    for(int i = 0; i < node->num_children; i++){
        print_tree(node->children[i]);
    }
}

size_t cramfs_read(file* f, void* buf, size_t len){
    // need to fix, offset to data not right
    size_t offset = f->node->data_offset;
    char* data_ptr = file_blob + offset; 
    memcpy(buf, data_ptr, len);
    f->file_postion = len;

    return len;
}

file* cramfs_open(inode* node, int flags){
    char* tok = strtok(node->path, "/");
    file* fd = (file*)malloc(sizeof(file));
    snprintf(fd->filename, 63, "%s", tok);
    fd->node = node;
    fd->flags = flags;
    fd->file_postion = 0;

    return fd;
}

inode* cramfs_lookup(char* file_path){
    if(root == NULL)return;

    FileSystemTreeNode* queue[MAX_QUEUE];
    size_t front = 0; //dequeue
    size_t rear = 0; // enqueue

    queue[rear++] = root;

    while (front < rear){
        FileSystemTreeNode* curr_node = queue[front++];

        if(strcmp(curr_node->node->path, file_path) == 0){
            return curr_node->node;
        }

        for(int i = 0; i < curr_node->num_children; i++){
            queue[rear++] = curr_node->children[i];
        }
    }
    return NULL;

}

void cramfs_unmount(){
    
}

inode* cramfs_mount(void*blob){
    inode* root_inode = cramfs_parse_blob(blob); // tree is made now
    return root_inode; // return first node of the tree root inode
}

// parse out parts into functions, parse sb, parse inode, make node
inode* cramfs_parse_blob(void* blob){
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
    read_ptr+= sizeof(size_t);

    printf("MAGIC NUM: %x\n", sb->magic_num);
    printf("size: %d\n", sb->blob_size);
    printf("num nodes: %d\n", sb->num_inodes);

    inode_table_ptr = (char*)blob + sb->inode_table_offset;
    dirent_table_ptr = (char*)blob + sb->dirent_table_offset;
    data_block_ptr = (char*)blob + sb->start_data_block_offset;
    char* tmp = NULL;

    // later maybe build a hash or lookup table for my tree for faster lookup
    do{
        size_t node_id = *(size_t*)dirent_table_ptr; // get node for that dirent
        char* tmp_node = inode_table_ptr + sizeof(blob_inode)*node_id; // grab node
        size_t num_children = *(size_t*)(dirent_table_ptr + sizeof(size_t));
        FileSystemTreeNode* tmp_tree_node;

        // if its not root then we need to find the child that will be a new parent
        if(root != NULL){
            tmp_tree_node = root;
            tmp_tree_node = find_parent(tmp_tree_node, node_id);

            tmp_tree_node->num_children = num_children;
            tmp_tree_node->children = malloc(sizeof(FileSystemTreeNode*) * tmp_tree_node->num_children);
        }
        else{
            tmp_tree_node = malloc(sizeof(FileSystemTreeNode));
            root = tmp_tree_node;

            inode* r_node = malloc(sizeof(inode));
            r_node->id = *(size_t*)tmp_node;
            tmp_node += sizeof(size_t);
            r_node->type = *(size_t*)tmp_node;
            tmp_node += sizeof(size_t);
            r_node->size = *(size_t*)tmp_node;
            tmp_node += sizeof(size_t);
            r_node->data_offset = *(size_t*)tmp_node;
            tmp_node += sizeof(size_t);
            snprintf(r_node->path, MAX_FILE_PATH, "%s", (char*)tmp_node);
            r_node->fops = &cramfs_ops;

            tmp_tree_node->node = r_node;
            tmp_tree_node->num_children = num_children;
            tmp_tree_node->children = malloc(sizeof(FileSystemTreeNode*) * tmp_tree_node->num_children);
        }

        for(int i = 0; i < num_children; i++){
            // inode_number = dirent table + ( the two size_t attributes + the inode we are currently on) 
            int node_num = *(size_t*)(dirent_table_ptr + ((sizeof(size_t) * (2+i))));
            // then get pointer to actual inode structure to begin building the inode
            char* tmp = inode_table_ptr + (sizeof(blob_inode)*node_num); // get inode num
            FileSystemTreeNode* child_tree_node = malloc(sizeof(FileSystemTreeNode));

            inode* tmp_inode = malloc(sizeof(inode));
            tmp_inode->id = *(size_t*)tmp;
            tmp += sizeof(size_t);
            tmp_inode->type = *(size_t*)tmp;
            tmp += sizeof(size_t);
            tmp_inode->size = *(size_t*)tmp;
            tmp += sizeof(size_t);
            tmp_inode->data_offset = *(size_t*)tmp;
            tmp += sizeof(size_t);
            snprintf(tmp_inode->path, MAX_FILE_PATH, "%s", (char*)tmp);
            tmp_inode->fops = &cramfs_ops;

            child_tree_node->node = tmp_inode;
            child_tree_node->num_children = 0;
            child_tree_node->children = NULL;

            tmp_tree_node->children[i] = child_tree_node;
        }

        dirent_table_ptr += (sizeof(Dirent)); // push to next dirent
    }while (dirent_table_ptr != data_block_ptr); // are we out of dirents?

    return root->node;
}

FileSystemTreeNode* find_parent(FileSystemTreeNode* node, size_t id){
    // trying to find a child that exists that will be a parent for the next dirent of children
    // if this is our parent node, node of a dirent
    if(node->num_children == 0){
        return;
    }

    for(int i = 0; i < node->num_children; i++){
        if(node->children[i]->node->id != id){
            // if its not our node, then grab that cild node and look at its children and do that for all children
            find_parent(node->children[i], id);
        }
        else{
            return node->children[i];
        }
    }
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
    sb->dirent_table_offset = sizeof(SuperBlock) + sizeof(blob_inode_table);
    sb->blob_size = sizeof(SuperBlock) + (sizeof(blob_inode) * 4) + (sizeof(Dirent)*2) + 13 + 15;
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
    blob_inode_table[1].type = DIR;
    blob_inode_table[1].data_size = 0;
    blob_inode_table[1].data_offset = 0;
    snprintf(blob_inode_table[1].path, MAX_FILE_PATH, "%s", "/docs");

    memcpy(write_ptr, &blob_inode_table[1], sizeof(blob_inode));
    write_ptr += sizeof(blob_inode);

    blob_inode_table[2].id = 2;
    blob_inode_table[2].type = FILE;
    blob_inode_table[2].data_size = 13;
    snprintf(blob_inode_table[2].path, MAX_FILE_PATH, "%s", "/hello.txt");
    blob_inode_table[2].data_offset = sb->start_data_block_offset;

    memcpy(write_ptr, &blob_inode_table[2], sizeof(blob_inode));
    write_ptr += sizeof(blob_inode);

    blob_inode_table[3].id = 3;
    blob_inode_table[3].type = FILE;
    blob_inode_table[3].data_size = 15;
    snprintf(blob_inode_table[3].path, MAX_FILE_PATH, "%s", "/docs/readme.md");
    blob_inode_table[3].data_offset = sb->start_data_block_offset + 13;

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
    d2->node = 1;
    d2->num_inodes = 1;
    d2->inodes[0] = 3;

    memcpy(write_ptr, d2, sizeof(Dirent));
    write_ptr += sizeof(Dirent);

    memcpy(write_ptr, "Hello, world!", 13);
    write_ptr += 13;

    memcpy(write_ptr, "# Documentation", 15);
    write_ptr += 15;

    free(sb);
    free(d1);
    free(d2);

    file_blob = blob;

    return blob;
}


