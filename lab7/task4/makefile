hexeditplus: hexeditplus.o
	gcc -m32 $< -o $@

hexeditplus.o: hexeditplus.c
	gcc -m32 -g -Wall -c $< -o $@

digitc: digitc.c
	gcc -m32 -fno-pie $< -o $@

.PHONY: clean

clean:
	rm *.o hexeditplus
