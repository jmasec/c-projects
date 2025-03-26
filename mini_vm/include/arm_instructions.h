#ifndef ARM_INSTRUCTIONS_H
#define ARM_INSTRUCTIONS_H

#include <stdint.h>

/*
Instructions Supported Currently:
ADD, AND, MOV, SUB, ORR, EOR, LDR, STR
B, BEQ, BNE, BGE, BLT, BGT (In progress)
ADDS, SUBS (Future)
*/

#define NUM_REG 16

typedef struct
{
    uint8_t cond;
    uint8_t is_immediate; // immediate(0) or register(1) 25
    uint8_t p; // post/pre indexing flag
    uint8_t u; // add/subtract offsets
    uint8_t is_byte_transfer; // (1) byte and (0) word
    uint8_t write_back; // 0 (no) 1 (yes)
    uint8_t is_load; // load (1) or store (0)
    uint8_t first_op_rn; // first operand reg 16-19
    uint8_t destination_op_rd; // dest reg 12-15
    uint16_t offset; // immediate val or second reg 0-11
} load_store_t; // from bit 16-23 can I do all possible combinations as enum?

typedef struct
{
    uint8_t cond;
    uint8_t opcode;
    uint8_t is_immediate;
    uint8_t set_condition; 
    uint8_t first_op_rn; 
    uint8_t destination_op_rd; 
    uint16_t second_op;
} data_processing_t; // not handling the shift in the second op

typedef struct
{
    uint8_t cond;
    uint8_t l; // link flag (1 for function calls)
    uint32_t immediate; // signed jump offset
} branch_t;

typedef struct
{
    uint32_t type;
    uint32_t opcode;
} instruction_type_t;


data_processing_t* build_data_process_struct(uint32_t curr_instruction);
load_store_t* build_load_store_struct(uint32_t curr_instruction);
branch_t* build_branch_struct(uint32_t curr_instruction);
instruction_type_t instruction_type(uint32_t curr_instruction);
uint32_t execute_and_instruction(data_processing_t* data_processing, uint32_t first_op, uint32_t second_op);
int32_t execute_sub_instruction(data_processing_t* data_processing, uint32_t first_op, uint32_t second_op);
uint32_t execute_add_instruction(data_processing_t* data_processing, uint32_t first_op, uint32_t second_op);
uint32_t execute_orr_instruction(data_processing_t* data_processing, uint32_t first_op, uint32_t second_op);
uint32_t execute_eor_instruction(data_processing_t* data_processing, uint32_t first_op, uint32_t second_op);
int is_valid_reg();
int32_t sign_extend_24bit(uint32_t imm24);
int32_t execute_cmp_instruction(data_processing_t* data_processing, uint32_t first_op, uint32_t second_op);

#endif