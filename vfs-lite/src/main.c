#include <stdio.h>    
#include <stdlib.h> 
#include <string.h>
#include "vfs_lite.h"
#include "cramfs.h"

int main(){
    // this is where cramfs_register is called
    cramfs_register();

    printf("Just registered: %s\n", driver_table[driver_count - 1].name);

    printf("Mount Func: %x, %x\n", driver_table[driver_count - 1].fsd->mount, driver_table[driver_count - 1].fsd->unmount);

    void* blob = cramfs_build_blob();

    vfs_mount("/mnt/cramfs/", "cramfs", blob);

    printf("Mounted FileSystem: %s\n", mount_table[0].mount_path);
    printf("Driver name: %s\n", mount_table[0].driver->name);
    printf("Magic: %x\n", mount_table[0].driver->magic_bytes);
}