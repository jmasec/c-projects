#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "include/memory_util.h"

// so I could make the load mem here, which takes a file path, then we malloc
// do the memcpy and return a pointer to the malloced mem that we copied bytes over into 
// but we also need to put it into the text sections

uint8_t * load_program(char *filepath)
{
    unsigned char buffer[1024];
    uint8_t *memory = (uint8_t *)malloc(MEM_SIZE);
    if(memory == NULL)
    {
        perror("malloc failed");
        return NULL;
    }

    size_t bytesRead;
    unsigned int offset = TEXT_REGION;

    FILE *file = fopen(filepath, "rb");
    if(file == NULL)
    {
        perror("Error opening file");
        return NULL;
    }

    while((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0)
    {
        memcpy(memory+offset, buffer, bytesRead);
        offset += bytesRead;
    }

    fclose(file);

    return memory;
}

uint32_t load_from_memory(uint8_t *memory, uint32_t address)
{
    // just store it in memory directly, up to prog to be in the right segment
    // I think this is right, because assuming that our mem block is some rand
    // address - rand address. Its still 0x0000 - 0xFFFF, so we will just use it 
    // as a sort of address into the memory. The user would see it at that range
    return memory[address+DATA_REGION];
}

void store_in_memory(uint8_t *memory, uint32_t address, uint32_t value)
{
    // just read whatever is in that memory address, dont worry about how much, prog handles that
    // we read in value and return it
    memcpy(&memory[address+DATA_REGION], &value, sizeof(value));
}