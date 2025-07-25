// disk.h
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

// typedef struct {
//     uint32_t magic;
//     uint32_t block_size;
//     uint32_t inode_count;
//     uint32_t inode_table_block;
//     uint32_t dirent_start_block;
// } Superblock;

// typedef struct {
//     uint8_t type;               // 0 = file, 1 = dir
//     uint32_t size;
//     uint32_t direct_block;      // block with file data or dirents
//     char name[28];              // null-terminated
// } MinifsInode;

// typedef struct {
//     uint32_t inode_number;
//     char name[28]; // name of file or dir in this entry
// } MinifsDirent;
