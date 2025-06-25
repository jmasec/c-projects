#include <stdio.h>    
#include <stdlib.h> 
#include <string.h>
#include "include/vfs_lite.h" 

file fd_table[MAX_OPEN_FILES];
RegisteredDriver driver_table[MAX_DRIVERS];
MountedFileSystem mount_table[MAX_MOUNTED_FILESYSTEMS];

int driver_count = 0;
int mount_count = 0;
int fd_count = 0;

// drivers can call this, and it fills the vfs array of registered drivers in the vfs.h file
// have to call extern func from drivers to send back the mount and unmount functions
void vfs_register_driver(const char* name, struct FileSystemDriver* fsd) {
    if (driver_count < MAX_DRIVERS) {
        snprintf(driver_table[driver_count].name, 16, "%s", name);
        driver_table[driver_count].fsd = fsd;
        driver_count++;
    }
}

int vfs_mount(char* mount_path, char* fs_name, void* blob){
   // find driver
   RegisteredDriver* reg_driver = get_driver(fs_name);
   if (reg_driver == NULL){
        printf("[-] Driver for this filesystem is not registered yet!");
        return -1;
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

inode* vfs_lookup(char *path, inode* node){
    // this func walks a file tree looking for a file/dir and returns the inode
    // lookup for a inode is in it fops, so you need to keep using the next lookup
    // maybe recursion here
    // whats the base case

    // then we keep looking through inodes, call itself with recursion for the
    // next inode we find
    // this is called within anywhere we need to find a file or directory for an
    // operation
}

char *tokenize_path(char *path){
    // tokenize path for mount, this would change into parsing blob to find
    // filesystem type later
    char* token = strtok(path, "/");

    return token;
}

file* vfs_open(char* path, int flags){
    // call vfs_lookup till I find the right file
    // call the inode open I get back for that file

    // tokenize
    char* token = tokenize_path(path);

    int fs_index = -1;
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

    // mount_table[fs_index].root->fops->lookup();
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



