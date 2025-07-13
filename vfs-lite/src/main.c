#include <stdio.h>    
#include <stdlib.h> 
#include <string.h>
#include <fcntl.h>
#include "vfs_lite.h"
#include "cramfs.h"

int main(){
    // this is where cramfs_register is called
    cramfs_register();

    printf("Just registered: %s\n", driver_table[driver_count - 1].name);

    printf("Just registered magic: %x\n", driver_table[driver_count - 1].magic_bytes);

    printf("Mount Func: %x, %x\n", driver_table[driver_count - 1].fsd->mount, driver_table[driver_count - 1].fsd->unmount);

    void* blob = cramfs_build_blob();

    FileSystemSource s = {
        .blob = blob,
        .type = SOURCE_TYPE_BLOB
    };

    vfs_mount("/mnt/cramfs/", &s);

    printf("Mounted FileSystem: %s\n", mount_table[0].mount_path);
    printf("Driver name: %s\n", mount_table[0].driver->name);
    printf("Magic: %x\n", mount_table[0].driver->magic_bytes);

    file* fd = vfs_open("/mnt/cramfs/hello.txt", O_RDONLY);

    void* buf = malloc(40);

    size_t ret = vfs_read(fd, buf, 10);

    for (int i = 0; i < ret; i++) {
        printf("char: %c \n", ((char*)buf)[i]);
    }

    vfs_close(&fd);

    void* buf2 = malloc(40);

    vfs_read(fd, buf2, 10);

    free(buf);
    free(buf2);

    vfs_unmount("/mnt/cramfs/");

    file* fd2 = vfs_open("/mnt/cramfs/hello.txt", O_RDONLY);

    int block_fd = open("minifs.img", O_RDWR);
    if (block_fd < 0) {
        perror("Failed to open disk image");
        return 1;
    }

    FileSystemSource s2 = {
        .fd = block_fd,
        .type = SOURCE_TYPE_BLOCK
    };

    vfs_mount("/mnt/minifs", &s2);

}