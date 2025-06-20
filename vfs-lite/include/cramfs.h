// mount -> open -> read -> write -> close -> unmount
#ifndef CRAMFS
#define CRAMFS

#include "vfs_lite.h" 

// on mounting for filesystem
typedef struct {
    int (*mount)();
    int (*unmount)();
} CFSDriver;

typedef struct{
    int (*read) ();
    int (*write) ();
} CFSOperations;

// on open call
typedef struct{
    int mode;
    int position;
    int flags;
    CFSOperations* fops; // we get file ops back from the driver and fill this in with the ptrs to the funcs
} File;

#endif /*CRAMFS*/