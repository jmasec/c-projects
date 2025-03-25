#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

#define MEM_SIZE 65536         // 64KB, addresses 0x0000 to 0xFFFF
#define NUM_REG 16
#define STACK_REGION (MEM_SIZE - 4)
#define TEXT_REGION 0
#define DATA_REGION (MEM_SIZE / 2)
#define DATA_REGION_END (DATA_REGION + 5000)


uint8_t * load_program(char *filepath);
void store_in_memory(uint8_t *memory, uint32_t address, uint32_t value);
uint32_t load_from_memory(uint8_t *memory, uint32_t address);

#endif