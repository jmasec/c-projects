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

unsigned long getRegisterFromLookup(char *reg, struct user_regs_struct target_reg);
int findProcessPID(char * processName);
void attachProcess(int pid);
void detachProcess(int pid);
struct user_regs_struct getProcessRegValues(int pid);
void updateRegisterValues(struct user_regs_struct regs, int pid, unsigned long value);

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

struct user_regs_struct getProcessRegValues(int pid){
    
    struct user_regs_struct target_regs;
    ptrace(PTRACE_GETREGS, pid, NULL, &target_regs);

    // get all reg values and print
    printf("R8: %llx, R9: %llx, R10: %llx, R11: %llx, R12: %llx, R13: %llx, R14: %llx, R15: %llx\n", target_regs.r8, target_regs.r9, target_regs.r10, target_regs.r11, target_regs.r12, target_regs.r13, target_regs.r14, target_regs.r15);
    printf("RSP: %llx, RBP: %llx, RDI: %llx, RSI: %llx, RDX: %llx, RCX: %llx, RBX: %llx, RAX: %llx\n", target_regs.rsp, target_regs.rbp, target_regs.rdi, target_regs.rsi, target_regs.rdx, target_regs.rcx, target_regs.rbx, target_regs.rax);
    return target_regs;
    
}

unsigned long getRegisterFromLookup(char *reg, struct user_regs_struct target_reg){
    unsigned int count = 0;
    while(strcmp(register_lookup[count].name, "NULL") != 0){
        if(strcmp(register_lookup[count].name, reg) == 0){
            return *(unsigned long *)((char *)&target_reg + register_lookup[count].offset);
        }
        count++;   
    }
    return -1; // probably not great practice since the return should be int
}

// remember, we can use registers to get the stack, so thats variables, return addresses, call stack etc
// parse /proc/<pid>/maps
    // we can use symbol resolution to map addresses to function names
    // we also can use this to get valid memory ranges so we dont segfault

void updateRegisterValues(struct user_regs_struct regs, int pid, unsigned long value){
    regs.r15 = value;
    if(ptrace(PTRACE_SETREGS, pid, 0, &regs) < 0){
        perror("PTRACE [SETREGS]");
        exit(1);
    }
}



int main(){
    int pid = findProcessPID("test_prog");

    if (pid < 0){
        perror("Bad PID");
        exit(1);
    }
    else{
        printf("PID:%d\n",pid);
    }

    attachProcess(pid);

    struct user_regs_struct target_regs = getProcessRegValues(pid);

    unsigned long reg_val = getRegisterFromLookup("rsp", target_regs);

    printf("%lx", reg_val);

    updateRegisterValues(target_regs, pid, 999);


    detachProcess(pid);

    return 0;
}
