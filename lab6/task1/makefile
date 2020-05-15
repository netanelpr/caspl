CC := gcc -m32
CFLAGS := -Wall

myshell: task1.o LineParser.o
	$(CC) $(CFLAGS) $^ -o $@

task1.o : task1.c LineParser.h
	$(CC) $(CFLAGS) -c $< -o $@

LineParser.o : LineParser.c LineParser.h
	$(CC) $(CFLAGS) -c $< -o $@

looper: looper.c
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean

clean:
	rm *.o myshell