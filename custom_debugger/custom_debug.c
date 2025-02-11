#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stddef.h>
#include <sys/user.h>

//long ptrace(enum __ptrace_request op, pid_t pid,void *addr, void *data);
// TODO: Update this to take in an executable to trace like gdb does as a command line arg fed to the prog

unsigned int getRegisterFromLookup(char *reg, struct user_regs_struct target_reg);

typedef struct {
    const char *name;
    size_t offset;
} RegisterLookup;

// Define the lookup table ok so its an array of structs that hold the register name and the offset of it
const RegisterLookup register_lookup[] = {
    { "rip", offsetof(struct user_regs_struct, rip) }, { "rax", offsetof(struct user_regs_struct, rax) }, { "rcx", offsetof(struct user_regs_struct, rcx) },
    { "rdx", offsetof(struct user_regs_struct, rdx) }, { "rsi", offsetof(struct user_regs_struct, rsi) }, { "rdi", offsetof(struct user_regs_struct, rdi) },
    { "eflags", offsetof(struct user_regs_struct, eflags) }, { "cs", offsetof(struct user_regs_struct, cs) }, { "rsp", offsetof(struct user_regs_struct, rsp) },
    { "ss", offsetof(struct user_regs_struct, ss) }, { "r15", offsetof(struct user_regs_struct, r15) }, { "r14", offsetof(struct user_regs_struct, r14) }, 
    { "r13", offsetof(struct user_regs_struct, r13) }, { "r12", offsetof(struct user_regs_struct, r12) }, { "r11", offsetof(struct user_regs_struct, r11) },
    { "rbp", offsetof(struct user_regs_struct, rbp) }, { "rbx", offsetof(struct user_regs_struct, rbx) }, { "r10", offsetof(struct user_regs_struct, r10) },
    { "r9", offsetof(struct user_regs_struct, r9) }, { "r8", offsetof(struct user_regs_struct, r8) }, { "NULL", 0},
};


int findProcessPID(char * processName){

    char pgrepCommand[100] = "pgrep ";
    FILE *fp;
    int len = strlen(processName);\
    char buffer[10];

    strncat(pgrepCommand, processName, len);
    
    fp = popen(pgrepCommand, "r");
    if (fp == NULL){
        perror("popen failed");
        return 1;
    }

    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        return atoi(buffer);
    }

    return -1;
}

void attachProcess(int pid){
    if(ptrace(PTRACE_ATTACH, pid, NULL, NULL) < 0){
        perror("PTRACE [ATTACH]");
        exit(1);
    }
    waitpid(pid, NULL, 0);
}

void detachProcess(int pid){
    if(ptrace(PTRACE_DETACH, pid, 0, NULL) < 0){
        perror("PTRACE [DETACH]");
        exit(1);
    }

}

void getProcessRegValues(char *reg, int pid){
    
    struct user_regs_struct target_regs;
    ptrace(PTRACE_GETREGS, pid, NULL, &target_regs);
    if (reg == NULL){
        // get all reg values and print
        printf("R8: %lx, R9: %lx, R10: %lx, R11: %lx, R12: %lx, R13: %lx, R14: %lx, R15: %lx\n", target_regs.r8, target_regs.r9, target_regs.r10, target_regs.r11, target_regs.r12, target_regs.r13, target_regs.r14, target_regs.r15);
        printf("RSP: %lx, RBP: %lx, RDI: %lx, RSI: %lx, RDX: %lx, RCX: %lx, RBX: %lx, RAX: %lx\n", target_regs.rsp, target_regs.rbp, target_regs.rdi, target_regs.rsi, target_regs.rdx, target_regs.rcx, target_regs.rbx, target_regs.rax);
    }
    else{
        // print single reg
        int reg_val = getRegisterFromLookup(reg, target_regs);
        if(reg_val){
            printf("0x%lx\n", reg_val);
        }
        else{
            printf("Register does not exist\n");
        }
    }
    
}

unsigned int getRegisterFromLookup(char *reg, struct user_regs_struct target_reg){
    unsigned int count = 0;
    while(strcmp(register_lookup[count].name, "NULL") != 0){
        if(strcmp(register_lookup[count].name, reg) == 0){
            return *(unsigned long *)((char *)&target_reg + register_lookup[count].offset);
        }
        count++;   
    }
    return NULL; // probably not great practice since the return should be int
}

// remember, we can use registers to get the stack, so thats variables, return addresses, call stack etc
// parse /proc/<pid>/maps
    // we can use symbol resolution to map addresses to function names
    // we also can use this to get valid memory ranges so we dont segfault

void updateRegisterValues(char *reg, unsigned int value){

}

void inspectMemory(unsigned int addr){

}

void setBreakPoint(unsigned int addr){
    
}

void removeBreakPoint(){

}

void singleStepExecution(){

}

void resumeExecution(){

}

void signalHandle(){

}

void displayCallStack(){

}

void showDisassembly(){

} 






int main(){
    int pid = findProcessPID("python");

    if (pid < 0){
        perror("Bad PID");
        exit(1);
    }
    else{
        printf("PID:%d\n",pid);
    }

    attachProcess(pid);

    getProcessRegValues("rdx", pid);

    detachProcess(pid);

    return 0;
}
