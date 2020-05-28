CC := gcc -m32
CFLAGS := -Wall

myshell: task2.o LineParser.o
	$(CC) $(CFLAGS) $^ -o $@

task2.o : task2.c LineParser.h
	$(CC) $(CFLAGS) -c $< -o $@

LineParser.o : LineParser.c LineParser.h
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm *.o myshell