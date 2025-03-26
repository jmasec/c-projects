#include <stdio.h>    
#include <stdlib.h> 
#include <stdint.h>
#include <string.h>
#include "include/mini_vm.h"
#include "include/arm_instructions.h"
#include "include/memory_util.h"

/*
- for handling if there is condition to update, if condition == 0, then we leave it, else we update CSPR update_CSPR(). 
I guess we can just or the bitmask from when we process an instruction? WE ONLY HAVE TO DO THIS FOR ADD(S) for example not regular instructions

- How do handle address translation, since realistically programs will give their own
addresses that I need to translate to an area, I hold offsets into memory 
*/

void init_registers(VMState *vms)
{
    for(int i = 0; i < NUM_REG; i++)
    {
        vms->registers[i] = 0;
    }

    vms->SP = STACK_REGION;
    vms->PC = TEXT_REGION;
}

VMState* init_vm()
{
    VMState* vms = (VMState*)malloc(sizeof(VMState));
    if(vms == NULL)
    {
        perror("Failed to allocate VMState");
        return NULL;
    }
    vms->CPSR = 0;
    vms->memory = load_program("/home/ubuntu/c_projects/mini_vm/tests/test_prog.bin");
    if(vms->memory == NULL)
    {
        perror("Error reading in binary, critical");
        return NULL;
    }

    init_registers(vms);

    return vms;
}

int run_vm()
{
    // read and execute instructions
    VMState* vms  = init_vm();
    for(;;)
    {
        vms->current_instruction = *(uint32_t*)(vms->memory + vms->PC);
        if(vms->PC == MEM_SIZE)
        {
            return 0;
        }
        else if(0) // there is code to return, jump somewhere or branch to LR
        {
            // need to handle of code wants to jump somewhere other than return
            vms->PC = vms->LR;
        }
        else // normal instruction
        {
            vms->PC += 4; // set PC to next
        }
        
    }
}

void update_control_register(VMState* vms, int32_t result, int negative_operand, int carried)
{   
    printf("RESULT: %d\n", result);
    if(result == 0)
    {
        vms->CPSR |= FLAG_Z;
    }
    else
    {
        vms->CPSR &= ~FLAG_Z;
    }

    if(result < 0)
    {
        vms->CPSR |= FLAG_N;
        if (negative_operand)
        {
            vms->CPSR |= FLAG_V;
        }
    }
    else
    {
        vms->CPSR &= ~FLAG_N;
    }

    if(carried)
    {
        vms->CPSR |= FLAG_C;
    }
}



void execute_instruction(const uint32_t curr_instruction, VMState* vms)
{
    // I could have this send the actual values to be computed and then I return result from exeucte, do I return a result struct?
    // Like conditions to be updated, etc?
    instruction_type_t instr_type = instruction_type(curr_instruction);
    // ok so now we have the instruction type and opcode (ex. so we know data processing and ADD )
    if(instr_type.type == 0) // data processing
    {
        data_processing_t* dp = build_data_process_struct(curr_instruction);

        uint32_t effective_op1 = vms->registers[dp->first_op_rn];
        uint32_t effective_op2;
        uint32_t result;
        if (dp->is_immediate) 
        {
            effective_op2 = dp->second_op;
        } else 
        {
            effective_op2 = vms->registers[dp->second_op];
        }

        printf("Dest Reg: %d , Val: %x\n", dp->destination_op_rd, vms->registers[dp->destination_op_rd]);
        printf("First Reg: %d, Val: %x\n", dp->first_op_rn, vms->registers[dp->first_op_rn]);
        printf("Second Op: %d, Val: %x\n", dp->second_op, vms->registers[dp->second_op]);

        switch(instr_type.opcode) // type of data processing
        {
            case 0:
                printf("AND Instruction\n");
                result = execute_and_instruction(dp, effective_op1, effective_op2);
                vms->registers[dp->destination_op_rd] = result;
                break;
            case 1: // EOR
                printf("EOR Instruction\n");
                result = execute_eor_instruction(dp, effective_op1, effective_op2);
                vms->registers[dp->destination_op_rd] = result;
                printf("Dest Reg: %d , Val: %x\n", dp->destination_op_rd, vms->registers[dp->destination_op_rd]);
                break;
            case 2:
                printf("SUB Instruction\n");
                result = execute_sub_instruction(dp, effective_op1, effective_op2);
                vms->registers[dp->destination_op_rd] = result;
                break;
            case 4: // ADD instruction
                printf("ADD Instruction\n");
                result = execute_add_instruction(dp, effective_op1, effective_op2);
                vms->registers[dp->destination_op_rd] = result;
                break;
            case 10: // CMP instruction
                printf("CMP Instruction\n");
                int is_negative;
                int carried;
                if((int32_t)effective_op1 < 0 || (int32_t)effective_op2 < 0)
                {
                    is_negative = 1;
                }
                if ((int32_t)effective_op1 >= (int32_t)effective_op2)
                {
                    carried = 1;
                }
                int32_t cmp_result = execute_cmp_instruction(dp, effective_op1, effective_op2);
                update_control_register(vms, cmp_result, is_negative, carried);
                break;
            case 12: // ORR
                printf("ORR Instruction\n");
                result = execute_orr_instruction(dp, effective_op1, effective_op2);
                vms->registers[dp->destination_op_rd] = result;
                break;
            case 13: // mov doesnt need a function
                vms->registers[dp->destination_op_rd] = effective_op2;
                break;
            default:
                printf("Unknown Instruction\n");
                break;
        }
        free(dp);
    }
    else if(instr_type.type == 1)
    {
        load_store_t* ls = build_load_store_struct(curr_instruction);

        uint32_t effective_op1 = vms->registers[ls->first_op_rn]; // R1
        uint32_t first_op = vms->registers[ls->destination_op_rd]; //R0
        uint32_t offset; // this the #4 or Register based on immediate
        uint32_t result;
        // ok need to fix this, the immediate is pertaining to this for str/ld
        // so the offset is always gonna be there and the op2 will be offset of reg
        if (!ls->is_immediate) 
        {
            offset = ls->offset;
        } else 
        {
            offset = vms->registers[ls->offset];
        }

        printf("Dest Reg: %d , Val: %x\n", ls->destination_op_rd, vms->registers[ls->destination_op_rd]);
        printf("First Reg: %d, Val: %x\n", ls->first_op_rn, vms->registers[ls->first_op_rn]);
        printf("Second Op: %d, Val: %x\n", ls->offset, vms->registers[ls->offset]);

        switch(instr_type.opcode)
        {
            case 0: // Store
                printf("Store Instruction\n");
                store_in_memory(vms->memory, effective_op1+offset, first_op);
                break;
            case 1: // Load
                printf("Load Instruction\n");
                result = load_from_memory(vms->memory, effective_op1+offset);
                printf("Result: %x\n", result);
                vms->registers[ls->destination_op_rd] = result;
                break;
            default:
                printf("Unknown Instruction\n");
                break;
        }
        free(ls);
    }
    else if(instr_type.type == 5)
    {
        branch_t* bp = build_branch_struct(curr_instruction);

        bp->immediate = (sign_extend_24bit(bp->immediate) << 2)  + vms->PC;

        // handle the imm24 shift, Target = PC + 8 + updated_immediate
    
        // ok so we need to check what kind of jump maybe? with the cases
        // we just have the address to jump to
        // if L bit is set, then its BL branch, else we need to check the
        // condition to find out the type of branch

        // B, BEQ, BNE, need to check CPSR to see if we execute as well
        // BGE, BLT, BGT, need to set these branches up too
        // EQ = Z set is equal
        // NE = Z clear is not equal
        // GE = N equals V is greater or equal
        // LT = N not equal to V is greater or equal
        // GT = Z clear AND (N equals V) greater than
        // LE = Z set OR (N not equal to V) less than or equal
        switch(instr_type.opcode)
        {
            case 0: // BEQ
                if((vms->CPSR >> 30)&0x1)
                {
                    // set PC
                }
                break;
            case 1: // BNE
                if((vms->CPSR >> 30)&0x1)
                {
                    // set PC
                }
                break;
            case 10: // BGE
                if((vms->CPSR >> 30)&0x1)
                {
                    // set PC
                }
                break;
            case 11: // BLT
                if((vms->CPSR >> 30)&0x1)
                {
                    // set PC
                }
                break;
            case 12: // BGT
                if((vms->CPSR >> 30)&0x1)
                {
                    // set PC
                }
                break;
            case 13: // BLE
                if((vms->CPSR >> 30)&0x1)
                {
                    // set PC
                }
                break;
            case 14: // B or BL based on L bit
                if((vms->CPSR >> 30)&0x1)
                {
                    // set PC
                }
                break;
            default:
                printf("Unknown Instruction\n");
                break;
        }
    }
}


int main(int argc, char *argv[]) {
    printf("Mini VM started.\n");

    VMState* vms = init_vm();

    //vms->CPSR |= FLAG_Z;     // Set Zero
    //vms->CPSR &= ~FLAG_Z;    // Clear Zero
    //vms->CPSR & FLAG_Z;      // check
    vms->R1 = 0x10;
    vms->R2 = 0x1;
    //vms->R0 = 0x1;
    //vms->R2 = 0x45;
    uint32_t b = 0b11100001010100010000000000000010; // CMP R1, R2
    //0b11100101100000010000000000000100; // STR R0, [R1, #4]
    //0b11100101100100010000000000000100; // LDR R0, [R1, #4]
    //0b11100010001000010000000000111100 EOR R0, R1, 0x3c
    //0b11100000001000010000000000000010 EOR R0, R1, R2
    //0b11100011100000010000000000111100 ORR R0, R1, #0x3c
    //0b11100001100000010000000000000010; ORR R0, R1, R2
    //0b11100000010000010000000000000010; SUB R0, R1, R2
    //0b11100010010000010000000011001100; SUB R0, R1, #cc
    //0b11100011101000000010000000111100; MOV R2, #0x3c
    //0b11100001101000000000000000000001; MOV R0, R1
    //0b11100010000000010000000000111100; AND R0, R1, 0x3C
    //0b11100000000000010000000000000010; AND Reg
    //0b11100010000000010000000011111111; AND Immediate
    //0b11100010100000010010000000000101; ADD 
    //0b11100000100000010010000000110011; ADD Immediate

    execute_instruction(b, vms);

    printf("FLAG V: %d, FLAG C: %d, FLAG Z: %d, FLAG N: %d\n", (vms->CPSR >> 28)&0x1, (vms->CPSR >> 29)&0x1, (vms->CPSR >> 30)&0x1, (vms->CPSR >> 31)&0x1);

    // vms->R0 = 0x0;
    // uint32_t a = 0b11100101100100010000000000000100; // LDR R0, [R1, #4]

    //execute_instruction(a, vms);
    
    printf("Free Memory\n");
    free(vms->memory);
    free(vms);
    return 0;
}