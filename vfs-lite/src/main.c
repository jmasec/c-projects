#include <stdio.h>    
#include <stdlib.h> 
#include <string.h>
#include <fcntl.h> 
#include <unistd.h> 

#include "vfs_lite.h"
#include "cramfs.h"
#include "blockfs.h"

int main(){
    // this is where cramfs_register is called
    cramfs_register();

    printf("Just registered: %s\n", driver_table[driver_count - 1].name);

    printf("Just registered magic: %x\n", driver_table[driver_count - 1].magic_bytes);

    printf("Mount Func: %x, %x\n", driver_table[driver_count - 1].fsd->mount, driver_table[driver_count - 1].fsd->unmount);

    void* blob = cramfs_build_blob();

    vfs_mount("/mnt/cramfs/", CRAMFS_DRIVER_NAME, blob);

    printf("Mounted FileSystem: %s\n", mount_table[0].mount_path);
    printf("Driver name: %s\n", mount_table[0].driver->name);
    printf("Magic: %x\n", mount_table[0].driver->magic_bytes);

    File* fd = vfs_open("/mnt/cramfs/docs/readme.md", O_RDONLY);

    if(fd == NULL){
        printf("Fail to open file");
        return -1;
    }

    char buf[50];

    size_t ret = vfs_read(fd, buf, 6);

    for (int i = 0; i < ret; i++) {
        printf("char: %c \n", ((char*)buf)[i]);
    }

    ret = vfs_read(fd, buf, 5);

    for (int i = 0; i < ret; i++) {
        printf("b char: %c \n", ((char*)buf)[i]);
    }

    vfs_close(&fd);

    vfs_read(fd, buf, 10);


    File* fd2 = vfs_open("/mnt/cramfs/hello.txt", O_RDONLY);


    if(fd2 == NULL){
        printf("Fail to open file");
        return -1;
    }
    

    size_t ret2 = vfs_read(fd2, buf, 6);

    for (int i = 0; i < ret2; i++) {
        printf("char: %c \n", ((char*)buf)[i]);
    }

    File* fd3 = vfs_open("/mnt/cramfs/docs/readme.md", O_RDONLY);

    if(fd3 == NULL){
        printf("Fail to open file");
        return -1;
    }

    size_t ret3 = vfs_read(fd3, buf, 6);

    for (int i = 0; i < ret3; i++) {
        printf("char: %c \n", ((char*)buf)[i]);
    }

    vfs_unmount("/mnt/cramfs/");

    File* fd4 = vfs_open("/mnt/cramfs/docs/readme.md", O_RDONLY);

    blockfs_register();

    printf("Just registered: %s\n", driver_table[driver_count - 1].name);

    printf("Just registered magic: %x\n", driver_table[driver_count - 1].magic_bytes);

    printf("Mount Func: %x, %x\n", driver_table[driver_count - 1].fsd->mount, driver_table[driver_count - 1].fsd->unmount);

    int disk_fd;

    const char* disk_image_path = "minifs_large.img";

    // the fd would need to work different in an actual kernel implementation
    disk_fd = open(disk_image_path, O_RDWR);

    if(disk_fd == -1){
        perror("Error opening disk image");
        return -1;
    }

    vfs_mount("/mnt/blockfs/", BLOCKFS_DRIVER_NAME, &disk_fd);

}