build:
	nasm -f elf start.s -o start.o
	gcc -g -m32 -Wall -ansi -c -nostdlib -fno-stack-protector util.c -o util.o
	gcc -g -m32 -Wall -ansi -c -nostdlib -fno-stack-protector encoder.c -o encoder.o
	ld -m elf_i386 start.o encoder.o util.o -o encoder

.PHONEY:
	clean

clean:
	rm *.o encoder
