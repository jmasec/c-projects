#include <stdio.h>    
#include <stdlib.h> 
#include <string.h>
#include "vfs_lite.h" 

file fd_table[MAX_OPEN_FILES];
RegisteredDriver driver_table[MAX_DRIVERS];
MountedFileSystem mount_table[MAX_MOUNTED_FILESYSTEMS];

int driver_count = 0;
int mount_count = 0;
int fd_count = 0;

int vfs_read(file* fd, void* buf, size_t size){
    int ret;
    if(fd->node->size < size){
        ret = fd->node->fops->read(fd, buf, fd->node->size);
    }
    else{
        ret = fd->node->fops->read(fd, buf, size);
    }
    if (ret > 0){
        fd->file_postion = ret; // how much would of been read
    }
    return ret;
}

// drivers can call this, and it fills the vfs array of registered drivers in the vfs.h file
// have to call extern func from drivers to send back the mount and unmount functions
void vfs_register_driver(const char* name, struct FileSystemDriver* fsd) {
    if (driver_count < MAX_DRIVERS) {
        snprintf(driver_table[driver_count].name, 16, "%s", name);
        driver_table[driver_count].fsd = fsd;
        driver_count++;
    }
}

void vfs_mount(char* mount_path, char* fs_name, void* blob){
   // find driver
   RegisteredDriver* reg_driver = get_driver(fs_name);
   if (reg_driver == NULL){
        printf("[-] Driver for this filesystem is not registered yet!");
        return;
   }

   if(mount_count < MAX_MOUNTED_FILESYSTEMS){
        inode* root_node = reg_driver->fsd->mount(blob);

        mount_table[mount_count].driver = reg_driver;
        snprintf(mount_table[mount_count].mount_path, 64, "%s", mount_path);
        mount_table[mount_count].root = root_node;
        mount_count++;
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
    int fs_index = -1;
    inode* node = NULL;

    // find fs type
    for(int i = 0; i < mount_count; i++){
        if(strstr(mount_table[i].mount_path, path) != NULL){
            fs_index = i;
        }
    }

    if(fs_index < 0){
        printf("[-] Filesystem is not mounted before trying to open!");
        return NULL;
    }

    char* token = strtok(path, DELIMINATOR);

    node = mount_table[fs_index].root->fops->lookup(node, token);

    token = strtok(NULL, DELIMINATOR);
    
    while (token != NULL){
        node = node->fops->lookup(node,token);
        token = strtok(NULL, DELIMINATOR);
    }

    file* fd = NULL;
    if(fd_count < MAX_OPEN_FILES){
        fd = node->fops->open(node,flags);
        if (fd == NULL){
            printf("[-] Failed to open the file, driver open died");
            return NULL;
        }
        fd_table[fd_count] = *fd;
        fd_count++;
    }

    return fd;
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



