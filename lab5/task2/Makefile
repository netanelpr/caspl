CC := gcc -m32
CFLAGS := -Wall

myshell: myshell.o LineParser.o
	$(CC) $(CFLAGS) $^ -o $@

myshell.o : myshell.c LineParser.h
	$(CC) $(CFLAGS) -c $< -o $@

LineParser.o : LineParser.c LineParser.h
	$(CC) $(CFLAGS) -c $< -o $@

looper: looper.c
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean

clean:
	rm *.o myshell