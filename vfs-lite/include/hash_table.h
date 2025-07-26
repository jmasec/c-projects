#ifndef HASH_TABLE
#define HASH_TABLE

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

#include <stdint.h>
#include <stddef.h>

typedef struct DirEntry DirEntry;

static uint64_t hash_key(const char* key);

typedef struct HTEntry{
    const char* key;
    DirEntry* direntry;
}HTEntry;

typedef struct HT{
    HTEntry* entries; // hash slots
    size_t capacity; // total size
    size_t length; // current num of entries
}HT;

#endif