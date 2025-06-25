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

// drivers can call this, and it fills the vfs array of registered drivers in the vfs.h file
// have to call extern func from drivers to send back the mount and unmount functions
void vfs_register_driver(const char* name, struct FileSystemDriver* fsd) {
    if (driver_count < MAX_DRIVERS) {
        //strncpy(driver_table[driver_count].name, name, 15);
        snprintf(driver_table[driver_count].name, 16, "%s", name);
        driver_table[driver_count].fsd = fsd;
        driver_count++;
    }
}

void vfs_mount(char* mount_path){
    // parse for name in mount path to match name for driver, but really
    // this is where we would need to read the blob at this mount path and
    // firgure out the filesystem
    // tokenize the file path
    // grab the driver thats registered with a name thats in the path
    // call the drivers mount
    // set up mount_table[mount_count]
    // inode we get from drivers mount, we got the driver, mount path
    
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

char *tokenize_path(char *){
    // tokenize path for mount, this would change into parsing blob to find
    // filesystem type later
}

file* vfs_open(char* path, int flags){
    // call vfs_lookup till I find the right file
    // call the inode open I get back for that file
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



