#include <stdio.h>    
#include <stdlib.h> 
#include <stdint.h>
#include <string.h>
#include "include/arm_instructions.h"

int is_valid_reg()
{
    // at least 16 check, then if special reg are used in wrong instructions
    // we would need to check this before executing the instruction
}

load_store_t* build_load_store_struct(uint32_t curr_instruction)
{
    load_store_t* ls = (load_store_t*)malloc(sizeof(load_store_t));
    ls->cond                = (curr_instruction >> 28) & 0xF;   // 4 bits
    ls->is_immediate        = (curr_instruction >> 25) & 0x1;   // 1 bit
    ls->p                   = (curr_instruction >> 24) & 0x1;   // 1 bit
    ls->u                   = (curr_instruction >> 23) & 0x1;   // 1 bit
    ls->is_byte_transfer    = (curr_instruction >> 22) & 0x1;   // 1 bit
    ls->write_back          = (curr_instruction >> 21) & 0x1;   // 1 bit
    ls->is_load             = (curr_instruction >> 20) & 0x1;   // 1 bit
    ls->first_op_rn         = (curr_instruction >> 16) & 0xF;   // 4 bits
    ls->destination_op_rd   = (curr_instruction >> 12) & 0xF;   // 4 bits
    ls->offset              = (curr_instruction >> 0)  & 0xFFF; // 12 bits

    return ls;
}

data_processing_t* build_data_process_struct(uint32_t curr_instruction)
{
    data_processing_t* dp = (data_processing_t*)malloc(sizeof(data_processing_t));
    dp->cond                = (curr_instruction >> 28) & 0xF;
    dp->opcode              = (curr_instruction >> 21) & 0xF;
    dp->is_immediate        = (curr_instruction >> 25) & 0xF;
    dp->set_condition       = (curr_instruction >> 20) & 0xF;
    dp->first_op_rn         = (curr_instruction >> 16) & 0xF;
    dp->destination_op_rd   = (curr_instruction >> 12) & 0xF;
    dp->second_op           = (curr_instruction >> 0) & 0xFF; 

    return dp;
}

branch_t* build_branch_struct(uint32_t curr_instruction)
{
    branch_t* bp = (branch_t*)malloc(sizeof(branch_t));
    bp->cond                = (curr_instruction >> 28) & 0xF; // 4 bit
    bp->l                   = (curr_instruction >> 24) & 0x1; // 1 bit
    bp->offset              = (curr_instruction >> 0) & 0x00FFFFFF; // 24 bit

    return bp;
}

instruction_type_t instruction_type(uint32_t curr_instruction)
{
    // check for a branch to start, set the type to that, and opcode to link or not
    // then lets add conditon to the the type and actually handle it since we need it
    instruction_type_t instr;
    if ((curr_instruction >> 26) & 0b111 == 5) // check for branch first
    {
        instr.type = 0b101;
        instr.opcode = 0; // no use for opcode here

        printf("Instruction Type: %x\n", instr.type);
        printf("Opcode: %x\n", instr.opcode);

        return instr;
    }
    else
    {
        instr.type = (curr_instruction >> 26) & 0b111;
    }

    if(instr.type == 1)
    {
        instr.opcode = (curr_instruction >> 20) & 0x1;
    }
    else
    {
        instr.opcode = (curr_instruction >> 21) & 0xF;
    }
    
    printf("Instruction Type: %x\n", instr.type);
    printf("Opcode: %x\n", instr.opcode);
    
    return instr;
}

uint32_t execute_and_instruction(data_processing_t* data_processing, uint32_t first_op, uint32_t second_op)
{
    uint32_t exe_result;
    exe_result = first_op & second_op;

    return exe_result;
}

uint32_t execute_sub_instruction(data_processing_t* data_processing, uint32_t first_op, uint32_t second_op)
{
    uint32_t exe_result;
    exe_result = first_op - second_op;

    return exe_result;
}

uint32_t execute_add_instruction(data_processing_t* data_processing, uint32_t first_op, uint32_t second_op)
{
    uint32_t exe_result;
    exe_result = first_op + second_op;

    return exe_result;
}

uint32_t execute_orr_instruction(data_processing_t* data_processing, uint32_t first_op, uint32_t second_op)
{
    uint32_t exe_result;
    exe_result = first_op | second_op;

    return exe_result;
}

uint32_t execute_eor_instruction(data_processing_t* data_processing, uint32_t first_op, uint32_t second_op)
{
    uint32_t exe_result;
    exe_result = first_op ^ second_op;

    return exe_result;
}

