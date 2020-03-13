#format is target-name: target dependencies
#{-tab-}actions

# All Targets
all: main

# Tool invocations
# Executable "hello" depends on the files hello.o and run.o.
main: main.o asm.o
	gcc -m32 -g -Wall -o main main.o asm.o

# Depends on the source and header files
main.o: mainAssignmentw.c
	gcc -g -Wall -m32 -c -o main.o mainAssignmentw.c 
 
asm.o: asmAssignmentw.s
	nasm -g -f elf -w+all -o asm.o asmAssignmentw.s


#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean: 
	rm -f *.o main

