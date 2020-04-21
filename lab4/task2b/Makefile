build:
	nasm -f elf start.s -o start.o
	gcc -g -m32 -Wall -ansi -c -nostdlib -fno-stack-protector util.c -o util.o
	gcc -g -m32 -Wall -ansi -c -nostdlib -fno-stack-protector flame2.c -o flame2.o
	ld -m elf_i386 start.o flame2.o util.o -o flame2

.PHONEY:
	clean

clean:
	rm *.o flame2
