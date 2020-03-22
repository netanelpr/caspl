CC = gcc
CFLAGS = -g -Wall -m32

all: main

main: main.o numbers.o add.o my_echo.o
	gcc -m32 -g -Wall -o main main.o numbers.o add.o
	gcc -m32 -g -Wall -o my_echo my_echo.o

# Depends on the source and header files
main.o: 
numbers.o: 
#task
my_echo.o:
 
add.o: add.s
	nasm -g -f elf -w+all -o add.o add.s


#tell make that "clean" is not a file name!
.PHONY: clean cleano

#Clean the build directory
clean: 
	rm -f *.o main my_echo
cleano: 
	rm -f *.o
