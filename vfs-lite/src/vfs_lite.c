#include <stdio.h>    
#include <stdlib.h> 
#include <string.h>
#include "vfs_lite.h" 

file* fd_table[MAX_OPEN_FILES];
RegisteredDriver driver_table[MAX_DRIVERS];
MountedFileSystem mount_table[MAX_MOUNTED_FILESYSTEMS];

int driver_count = 0;
int mount_count = 0;
int fd_count = 0;

// need to add a reset function for the fd file posistion ptr

size_t vfs_read(file* fd, void* buf, size_t size){
    if(fd == NULL){
        printf("[-] File Descriptor is invalid\n");
        return -1;
    }
    if((fd->node->size - fd->file_postion) < size){ // if file posistion is moved up then different size
        return fd->node->fops->read(fd, buf, (fd->node->size - fd->file_postion));
    }
    else if(fd->node->size < size){ // if the file size is less than asking size
        return fd->node->fops->read(fd, buf, fd->node->size);
    }
    else{ // if size is less than file size
        return fd->node->fops->read(fd, buf, size);
    }
}

// drivers can call this, and it fills the vfs array of registered drivers in the vfs.h file
// have to call extern func from drivers to send back the mount and unmount functions
void vfs_register_driver(const char* name, struct FileSystemDriver* fsd) {
    if (driver_count < MAX_DRIVERS) {
        snprintf(driver_table[driver_count].name, 15, "%s", name);
        driver_table[driver_count].fsd = fsd;
        driver_count++;
    }
}

void vfs_unmount(char* mount_path){
    MountedFileSystem* filesystem_to_unmount = NULL;
    char* mntpath_ptr = NULL;
    // find fs type
    for(int i = 0; i < mount_count; i++){
        if((mntpath_ptr = strstr(mount_path, mount_table[i].mount_path)) != NULL){
            filesystem_to_unmount = &mount_table[i];
        }
    }

    if(mntpath_ptr == NULL){
        printf("[-] Filesystem is not mounted before trying to open!\n");
        return NULL;
    }

    filesystem_to_unmount->driver->fsd->unmount(filesystem_to_unmount->root);

    memset(filesystem_to_unmount, 0, sizeof(MountedFileSystem));
    mount_count--;

}

void vfs_mount(char* mount_path, char* fs_name, void* blob){
   // find driver
   RegisteredDriver* reg_driver = get_driver(fs_name);
   int magic_num = *(int*)blob; // update to use magic num from superblock for driver type
   if (reg_driver == NULL){
        printf("[-] Driver for this filesystem is not registered yet!\n");
        return;
   }

   reg_driver->magic_bytes = magic_num;

   if(mount_count < MAX_MOUNTED_FILESYSTEMS){
        inode* root_node = reg_driver->fsd->mount(blob);
        if (root_node == NULL){
            printf("[-] Driver mount failed!\n");
            return;
        }

        for(int i = 0; i < MAX_MOUNTED_FILESYSTEMS; i++){
            if(mount_table[i].root == NULL){
                mount_table[mount_count].driver = reg_driver;
                snprintf(mount_table[mount_count].mount_path, 63, "%s", mount_path);
                mount_table[mount_count].root = root_node;
                mount_count++;
                break;
            }
        }
   }
}

RegisteredDriver* get_driver(char* fs_name){
   for(int i = 0; i < driver_count; i ++){
        if((strcmp(driver_table[i].name, fs_name)) == 0){
            return &driver_table[i];
        }
   }
   return NULL;
}

file* vfs_open(char* path, int flags){
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

    char* file_path = mntpath_ptr + strlen(curr_filesystem->mount_path)-1;

    printf("FILE PATH: %s\n", file_path);

    inode* node = curr_filesystem->root->fops->lookup(file_path);

    file* fd;
    if(fd_count < MAX_OPEN_FILES){
        fd = node->fops->open(node,flags);
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

int vfs_close(file** fd){

    int id = (*fd)->id;
    free(*fd);
    fd_table[id] = NULL;
    *fd = NULL; 

    fd_count--;
    return 0;
}

MountedFileSystem* get_mounted_filesystem(char *path){
    MountedFileSystem *mfs = find_prefix_mount(path);
    if(mfs == NULL){
        printf("[-] No prefix was found, is this filesystem mounted?");
        return;
    }
    return mfs;
}

MountedFileSystem* find_prefix_mount(char *path){
    for(int i = 0; i < mount_count; i ++){
        if(strstr(mount_table[i].mount_path, path) != NULL){
            return &mount_table[i];
        }
    }
    return NULL;
}



