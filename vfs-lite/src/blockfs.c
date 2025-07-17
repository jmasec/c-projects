#include <stdio.h>    
#include <stdlib.h> 
#include <string.h>
#include "blockfs.h"
#include "vfs_lite.h"

int total_blocks = 0;

void blockfs_register(){    
    vfs_register_driver(DRIVER_NAME, MAGIC_NUM, &blockfs_fsd);
}

inode* blockfs_mount(int fd){
    inode* root_inode = blockfs_parse_disk(fd); // tree is made now
    return root_inode; // return first node of the tree root inode
}

inode* blockfs_parse_disk(int disk_image_fd){
    
}