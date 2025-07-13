#include <stdio.h>    
#include <stdlib.h> 
#include <string.h>
#include "blockfs.h"

void blockfs_register(){    
    vfs_register_driver(DRIVER_NAME, &blockfs_fsd);
}