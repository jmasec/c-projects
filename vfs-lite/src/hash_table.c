#include "hash_table.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Return 64-bit FNV-1a hash for key (NUL-terminated). See description:
// https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
static uint64_t hash_key(const char* key) {
    uint64_t hash = FNV_OFFSET;
    for (const char* p = key; *p; p++) {
        hash ^= (uint64_t)(unsigned char)(*p);
        hash *= FNV_PRIME;
    }
    return hash;
}

HT* init_ht(){
    HT* hash_table = malloc(sizeof(HT));

    if(NULL == hash_table){
        printf("[-] Failed to allocate hash table");
        return NULL;
    }

    hash_table->length = 0;
    hash_table->capacity = INITIAL_CAPACITY;

    hash_table->entries = calloc(hash_table->capacity, sizeof(HTEntry));
    if (hash_table->entries == NULL) {
        free(hash_table);
        return NULL;
    }
    return hash_table;
}

void ht_destroy(HT* table) {
    // First free allocated keys.
    for (int i = 0; i < table->capacity; i++) {
        free((void*)table->entries[i].key);
        free(table->entries[i].direntry);
    }

    // Then free entries array and table itself.
    free(table->entries);
    free(table);
}

void* ht_get(HT* table, const char* key){
    uint64_t hash = hash_key(key);
    size_t index = hash % (uint64_t)table->capacity;

    while(table->entries[index].key != NULL){
        if(strcmp(table->entries[index].key, key) == 0){
            return table->entries[index].direntry;
        }

        index++;
        if (index >= table->capacity) {
            index = 0;
        }
    }
    return NULL;
}

const char* ht_set(HT* table, const char* key, DirEntry* value){
    if(value == NULL){
        return NULL;
    }

    // handle expanding here if we run out of spaces
    if(table->length >= (0.80 * table->capacity)){
        printf("expand");
    }

    return ht_set_entry(table->entries, table->capacity, key, value, &table->length);
}

static const char* ht_set_entry(HTEntry* entries, size_t capacity,
    const char* key, DirEntry* value, size_t* plength){
        uint64_t hash = hash_key(key);
        size_t index = hash % (uint64_t)capacity;

        while(entries[index].key != NULL){
            // already exists
            if(strcmp(entries[index].key, key) == 0){
                entries[index].direntry = value;
                return entries[index].key;
            }

            index++;
            if (index >= capacity) {
                index = 0;
            }
        }

        if(plength != NULL){
            key = strdup(key);
            if(key == NULL){
                return NULL;
            }
            (*plength)++;
        }
        entries[index].key = (char*)key;
        entries[index].direntry = value;
        return key;
}

