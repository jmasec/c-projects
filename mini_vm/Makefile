# Compiler for C Code
CC=gcc

# Compiler for ARM Assembly
AS=arm-linux-gnueabihf-as
LD=arm-linux-gnueabihf-ld
OBJCOPY=arm-linux-gnueabihf-objcopy

# Directories
SRC_DIR=src
INCLUDE_DIR=include
TEST_DIR=tests

# Source and Header Files
C_SOURCES=$(SRC_DIR)/mini_vm.c $(SRC_DIR)/arm_instructions.c $(SRC_DIR)/memory_util.c
HEADERS=$(INCLUDE_DIR)/mini_vm.h $(INCLUDE_DIR)/arm_instructions.h $(INCLUDE_DIR)/memory_util.h
VM_BIN=mini_vm

# ARM Assembly Test Files
ASM_SRC=$(TEST_DIR)/test_prog.s
ASM_OBJ=$(TEST_DIR)/test_prog.o
ASM_BIN=$(TEST_DIR)/test_prog
RAW_BIN=$(TEST_DIR)/test_prog.bin

# Compiler Flags
CFLAGS=-I. -g # -Wall -Wextra -Werror
LDFLAGS=

# Default target: Compile everything
all: $(VM_BIN) $(RAW_BIN)

# Compile the VM
$(VM_BIN): $(C_SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o $(VM_BIN) $(C_SOURCES)


# Assemble the ARM test program
$(ASM_OBJ): $(ASM_SRC)
	$(AS) -o $(ASM_OBJ) $(ASM_SRC)

# Link the ARM binary
$(ASM_BIN): $(ASM_OBJ)
	$(LD) -o $(ASM_BIN) $(ASM_OBJ)

# Extract raw machine code
$(RAW_BIN): $(ASM_BIN)
	$(OBJCOPY) -O binary $(ASM_BIN) $(RAW_BIN)

# Clean all generated files
clean:
	rm -f $(VM_BIN) $(ASM_OBJ) $(ASM_BIN) $(RAW_BIN)
