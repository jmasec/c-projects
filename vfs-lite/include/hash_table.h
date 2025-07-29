#ifndef HASH_TABLE
#define HASH_TABLE

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL
#define INITIAL_CAPACITY 50

#include <stdint.h>
#include <stddef.h>

typedef struct DirEntry DirEntry;

typedef struct HTEntry{
    const char* key;
    DirEntry* direntry;
}HTEntry;

typedef struct HT{
    HTEntry* entries; // hash slots
    size_t capacity; // total size
    size_t length; // current num of entries
}HT;

static uint64_t hash_key(const char* key);
HT* init_ht();
void ht_destroy(HT* table);
void* ht_get(HT* table, const char* key);
const char* ht_set(HT* table, const char* key, DirEntry* value);
static const char* ht_set_entry(HTEntry* entries, size_t capacity,
    const char* key, DirEntry* value, size_t* plength);


#endif