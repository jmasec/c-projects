#ifndef MINI_VM_H
#define MINI_VM_H

#include <stdint.h>

#define NUM_REG 16
#define PC registers[15] // offset into mem for the next instruction
#define LR registers[14] // offset 
#define SP registers[13] // offset to stack
#define R12 registers[12]
#define R11 registers[11]
#define R10 registers[10] 
#define R9 registers[9]
#define R8 registers[8]
#define R7 registers[7] 
#define R6 registers[6]
#define R5 registers[5]
#define R4 registers[4] 
#define R3 registers[3]
#define R2 registers[2]
#define R1 registers[1] 
#define R0 registers[0]


#define FLAG_N (1 << 31)
#define FLAG_Z (1 << 30)
#define FLAG_C (1 << 29)
#define FLAG_V (1 << 28)


typedef struct
{
    uint32_t registers[NUM_REG];
    uint8_t *memory;
    uint32_t CPSR;
    uint32_t current_instruction;
} VMState;

void init_registers(VMState *vms);
VMState* init_vm();
int run_vm();
void update_control_register(VMState* vms, int32_t result, int negative_operand, int carried);
void execute_instruction(const uint32_t curr_instruction, VMState* vms);

#endif