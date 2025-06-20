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

inode* vfs_lookup(char *path, inode* node){

}

char *tokenize_path(char *){
    // tokenize path
}



