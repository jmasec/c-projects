#include <stdio.h>    
#include <stdlib.h> 
#include <string.h>
#include "include/vfs_lite.h" 

// drivers can call this, and it fills the vfs array of registered drivers in the vfs.h file
// have to call extern func from drivers to send back the mount and unmount functions
void vfs_register_driver(const char* name, struct FileSystemDriver* fsd) {
    if (driver_count < MAX_DRIVERS) {
        strncpy(driver_table[driver_count].name, name, 15);
        driver_table[driver_count].fsd = fsd;
        driver_count++;
    }
}

void vfs_mount(char* mount_path){
    MountedFileSystem* mfs = malloc(sizeof(MountedFileSystem));

}

inode* vfs_lookup(char *path, inode* node){
    // maybe recursion here
    MountedFileSystem* mfs = NULL;
    mfs = get_mounted_filesystem(path);

    // whats the base case

    // then we keep looking through inodes, call itself with recursion for the
    // next inode we find
}

char *tokenize_path(char *){
    // tokenize path
}

void vfs_open(char* path, int flags){
    
}

MountedFileSystem* get_mounted_filesystem(char *path){
    MountedFileSystem* mfs;
    if(mfs = find_prefix_mount(path) == NULL){
        printf("[-] No prefix was found, is this filesystem mounted?");
        return NULL;
    }
    return mfs;
}

MountedFileSystem* find_prefix_mount(char *path){
    MountedFileSystem* mfs;

    for(int i = 0; i < mount_count; i ++){
        if(strstr(mount_table[i]->mount_path, path) != NULL){
            mfs = mount_table[i];
            break;
        }
    }
    return mfs;
}

char* find_valid_driver(char * path){
    // find mem or ext2 in the path and return it 
}



