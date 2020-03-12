#include<stdio.h>

void task_1a(int argc, char **argv){

	int input = -1;
	input = fgetc(stdin);

	while(input > -1){
		
		if(input > 96 & input < 123){
			input = input - 32;
		}
		fputc(input, stdout);
		input = fgetc(stdin);
	}
}

int main(int argc, char **argv){
	task_1a(argc, argv);
	return 0;
}
