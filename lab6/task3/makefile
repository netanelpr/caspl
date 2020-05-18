CC := gcc -m32
CFLAGS := -Wall

mypipeline: task3.o
	$(CC) $(CFLAGS) $^ -o $@

task3.o : task3.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm *.o mypipeline