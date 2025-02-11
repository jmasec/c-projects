#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int test_var = 42;  // Global variable to inspect

void allocate_memory() {
    int i = 0;
    printf("%d", i);
    int *heap_var = (int *)malloc(sizeof(int));
    *heap_var = 1337;  // Assign value for debugging
    printf("Heap variable allocated at: %p\n", (void *)heap_var);
    
    while (1) {  // Infinite loop to keep the process running for debugging
        sleep(5);
    }

    free(heap_var);  // (Won't reach here, but included for completeness)
}

int main() {
    printf("Process ID: %d\n", getpid());  // Print PID for attaching with ptrace
    allocate_memory();
    return 0;
}
