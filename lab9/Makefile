
task1:
	nasm -f elf skeleton.s -o skeleton.o
	ld -m elf_i386 skeleton.o -o ver

.PHONY: clean

clean:
	rm *.o