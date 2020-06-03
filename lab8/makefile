myELF: myELF.o
	gcc -m32 $< -o $@

myELF.o: myELF.c
	gcc -m32 -g -Wall -c $< -o $@

.PHONY: clean

clean:
	rm *.o myELF
