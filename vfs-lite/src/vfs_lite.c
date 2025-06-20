#include <stdio.h>    
#include <stdlib.h> 
#include <string.h>
#include "include/vfs_lite.h" 

// drivers can call this, and it fills the vfs array of registered drivers in the vfs.h file
void vfs_register_driver(const char* name, struct FileSystemOps* fops) {
    if (driver_count < MAX_DRIVERS) {
        strncpy(driver_table[driver_count].name, name, 15);
        driver_table[driver_count].fops = fops;
        driver_count++;
    }
}

