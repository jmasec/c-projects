// mount -> open -> read -> close -> unmount
// no write for cramfs
#ifndef CRAMFS
#define CRAMFS

#include "vfs_lite.h" 
#include "driver.h"

FileSystemDriver* cramfs_register(); // fill out the 
void* cramfs_build_blob(); // later
void cramfs_parse_blob(); // later
void cramfs_open();
void cramfs_close();
void cramfs_read();

#endif /*CRAMFS*/