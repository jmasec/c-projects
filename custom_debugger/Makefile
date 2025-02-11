CC = gcc
CFLAGS = -g -Wall -Wextra -Werror

# Define targets
all: custom_debug test_prog

# Compile the debugger
custom_debug: custom_debug.c
	$(CC) $(CFLAGS) -o custom_debug custom_debug.c

# Compile the test program
test_prog: test_prog.c
	$(CC) $(CFLAGS) -o test_prog test_prog.c

# Clean up compiled binaries
clean:
	rm -f custom_debug test_prog
