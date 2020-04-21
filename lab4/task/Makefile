build:
	nasm -f elf start.s -o start.o
	gcc -g -m32 -Wall -ansi -c -nostdlib -fno-stack-protector util.c -o util.o
	gcc -g -m32 -Wall -ansi -c -nostdlib -fno-stack-protector patch.c -o patch.o
	ld -m elf_i386 start.o patch.o util.o -o patch

build_main:
	nasm -f elf start.s -o start.o
	gcc -g -m32 -Wall -ansi -c -nostdlib -fno-stack-protector util.c -o util.o
	gcc -g -m32 -Wall -ansi -c -nostdlib -fno-stack-protector main.c -o main.o
	ld -m elf_i386 start.o main.o util.o -o task0

.PHONEY:
	clean

clean:
	rm *.o task0
