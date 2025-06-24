#include <stdio.h>    
#include <stdlib.h> 
#include <string.h>
#include "cramfs.h"

void cramfs_register(){    
    vfs_register_driver(DRIVER_NAME, &cramfs_fsd);
}

void* cramfs_build_blob(){
    char* blob = (char*)malloc(4096);
    char* write_ptr = blob;
}


