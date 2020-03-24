count: count-words.o
	gcc -m32 $< -o $@

count-words.o: count-words.c
	gcc -m32 -g -Wall -c $< -o $@

.PHONY: clean

clean:
	rm *.o count
