#include <stdio.h>    
#include <stdlib.h> 
#include <string.h>
#include "vfs_lite.h" 

File* fd_table[MAX_OPEN_FILES];
RegisteredDriver driver_table[MAX_DRIVERS];
MountedFileSystem mount_table[MAX_MOUNTED_FILESYSTEMS];

unsigned long driver_count = 0;
unsigned long mount_count = 0;
unsigned long fd_count = 0;


// drivers can call this, and it fills the vfs array of registered drivers in the vfs.h file
// have to call extern func from drivers to send back the mount and unmount functions
void vfs_register_driver(const char* name, unsigned long magic_num, struct FileSystemDriver* fsd) {
    if (driver_count < MAX_DRIVERS) {
        snprintf(driver_table[driver_count].name, 15, "%s", name);
        driver_table[driver_count].fsd = fsd;
        driver_table[driver_count].magic_bytes = magic_num;
        driver_count++;
    }
}

// we want to add a MountedFileSystem. So we need the driver, superblock, root_inode
// how do we know what driver!!! Magic num needs to read into data
// so mount should already have the data passed into of the filesystem type
// filesystem can be a blob or a file path to a physical device or disk img based on name
void vfs_mount(char* mount_path, char* filesystem_name, void* filesystem){

    RegisteredDriver* reg_driver = get_driver(filesystem_name);
    if (reg_driver == NULL){
            printf("[-] Driver for this filesystem is not registered yet!\n");
            return;
    }

    if(mount_count < MAX_MOUNTED_FILESYSTEMS){
            VFSSuperBlock* super_block = reg_driver->fsd->mount(filesystem);

            for(int i = 0; i < MAX_MOUNTED_FILESYSTEMS; i++){
                if(mount_table[i].root_inode == NULL){
                    mount_table[mount_count].driver = reg_driver;
                    snprintf(mount_table[mount_count].mount_path, 63, "%s", mount_path);
                    mount_table[mount_count].root_inode = super_block->s_root->node;
                    mount_table[mount_count].super_block = super_block;
                    mount_count++;
                    break;
                }
            }
    }
}

// we need to go through the dirents and build dentry and update a tree that will be
// in the superblock. We use lookup from the driver to get the next inode for us
// and send back that inode which we wrap in a dentry
// how do we handle children and num children?
File* vfs_open(char* path, int flags){
    // call vfs_lookup till I find the right file
    // call the inode open I get back for that file
    MountedFileSystem* curr_filesystem = NULL;
    char* mntpath_ptr = NULL;

    // find fs type
    for(int i = 0; i < mount_count; i++){
        if((mntpath_ptr = strstr(path, mount_table[i].mount_path)) != NULL){
            curr_filesystem = &mount_table[i];
        }
    }
    if(mntpath_ptr == NULL){
        printf("[-] Filesystem is not mounted before trying to open!\n");
        return NULL;
    }
    char* file_path;
    // not include the mnt path into the file path I am looking for once we know the fs
    char* file_path_ = mntpath_ptr + strlen(curr_filesystem->mount_path)-1;
    snprintf(file_path, MAX_NAME, "%s", file_path_);

    printf("FILE PATH: %s\n", file_path);

    // we need to assume we have root taken care of at this point
    // so on mount set up the hash table and we would fetch that from there
    // but for now lets just lookup the root node, it should still handle it
    // we should always have the root hashed and setup to fetch
    // inode = hash_get("/") -> inode.lookup(inode, token)
    // realisticaly the lookup would use the inode data offset/block num
    // the use the filepath to strcmp with the names on the dirents in
    // that data block or data blob

    char*token = strtok(file_path, "/");

    printf("TOKEN: %s\n", token);

    // here we would call hash_get on the hash token, if its not there add it
    // and then do a lookup to grab from disk, then after make the cache

    VFSInode* node = curr_filesystem->root_inode->i_op->lookup(curr_filesystem->root_inode,file_path);

    File* fd;
    if(fd_count < MAX_OPEN_FILES){
        fd = node->f_op->open(node,flags);
        if (fd == NULL){
            printf("[-] Failed to open the file, driver open died");
            return NULL;
        }
        // look for empty fd slot
        for(int i = 0; i < MAX_OPEN_FILES; i++){
            if (fd_table[i] == NULL) {
                fd->id = i;
                fd_table[i] = fd;
                fd_count++;
                break;
            }
        }
    }
    else{
        printf("[-] Out of open file descriptors");
        return NULL;
    }

    return fd;
}

// need to add a reset function for the fd file posistion ptr

// size_t vfs_read(File* fd, void* buf, size_t size){
//     if(fd == NULL){
//         printf("[-] File Descriptor is invalid\n");
//         return -1;
//     }
//     if((fd->node->size - fd->cursor_postion) < size){ // if file posistion is moved up then different size
//         return fd->node->fops->read(fd, buf, (fd->node->size - fd->cursor_postion));
//     }
//     else if(fd->node->size < size){ // if the file size is less than asking size
//         return fd->node->fops->read(fd, buf, fd->node->size);
//     }
//     else{ // if size is less than file size
//         return fd->node->fops->read(fd, buf, size);
//     }
// }

// void vfs_unmount(char* mount_path){
//     MountedFileSystem* filesystem_to_unmount = NULL;
//     char* mntpath_ptr = NULL;
//     // find fs type
//     for(int i = 0; i < mount_count; i++){
//         if((mntpath_ptr = strstr(mount_path, mount_table[i].mount_path)) != NULL){
//             filesystem_to_unmount = &mount_table[i];
//         }
//     }

//     if(mntpath_ptr == NULL){
//         printf("[-] Filesystem is not mounted before trying to open!\n");
//         return NULL;
//     }

//     filesystem_to_unmount->driver->fsd->unmount(filesystem_to_unmount->root);

//     memset(filesystem_to_unmount, 0, sizeof(MountedFileSystem));
//     mount_count--;
// }


// int vfs_close(File** fd){

//     int id = (*fd)->id;
//     free(*fd);
//     fd_table[id] = NULL;
//     *fd = NULL; 

//     fd_count--;
//     return 0;
// }


RegisteredDriver* get_driver(char* fs_name){
   for(int i = 0; i < driver_count; i ++){
        if((strcmp(driver_table[i].name, fs_name)) == 0){
            return &driver_table[i];
        }
   }
   return NULL;
}

