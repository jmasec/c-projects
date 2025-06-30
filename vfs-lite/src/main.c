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

    cramfs_parse_blob(blob);

    print_tree(root);

}