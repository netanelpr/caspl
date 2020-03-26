menu_map: menu_map.o
	gcc -m32 $< -o $@

menu_map.o: menu_map.c
	gcc -m32 -g -Wall -c $< -o $@

.PHONY: clean

clean:
	rm *.o menu_map
