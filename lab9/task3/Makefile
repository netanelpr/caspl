
task1:
	nasm -f elf skeleton.s -o skeleton.o
	ld -m elf_i386 skeleton.o -o ver

build_files:
	cp ELFexec2short ELFexec
	./ver
	mv ELFexec ELFexec2
	cp ELFexec2short ELFexec
.PHONY: clean

clean:
	rm *.o