#include<stdio.h>
#include<string.h>
#include<stdlib.h>

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


void task_1b(int argc, char **argv){

	int debug_flag = 0;
	for(int i=1; i < argc; i=i+1){
		if(!strcmp("-D", argv[i])){
			debug_flag = 1;
		} else {
			printf("Invalid paramater\n");
			fflush(stdout);
			return;
		}
	}


        int c_input = -1;

	int len = 128, at_index = 0;
	char *input = (char *)malloc(sizeof(char)*len);

        c_input = fgetc(stdin);

        while(c_input > -1){
		if(c_input != 10){
			int c_input1 = c_input;

                	if(c_input > 96 & c_input < 123){
                	        c_input = c_input - 32;
                	}

			if(at_index == len){
				len = len * 2;
				char *tmp_input = (char *) malloc(sizeof(char)*len);
				strcpy(tmp_input, input);
				free(input);
				input = tmp_input;
			}
			input[at_index] = c_input;
			at_index = at_index + 1;

			if(debug_flag){
                		fprintf(stderr, "%d\t%d\n", c_input1, c_input);
			}
		} else {
			printf("%s\n", input);
			fflush(stdout);
			for(int i=0; i < at_index; i=i+1){
				input[i] = 0;
			}
			at_index = 0;
		}
                c_input = fgetc(stdin);

        }
}

void task_1c(int argc, char **argv){
	
	int enc = 0;
	int key_len = 0;
	char *key = 0;
	for(int i=1; i < argc; i=i+1){
		int len;
		if((len = strlen(argv[i])) > 2){
			if(argv[i][0] == '-' & argv[i][1] == 'e'){
				enc = -1;
				key_len = len - 2;
				key = (argv[i] + 2);
			} else {
				if(argv[i][0] == '+' & argv[i][1] == 'e'){
					enc = 1;
					key_len = len - 2;
					key = (argv[i] + 2);
				} else {
					fprintf(stderr, "Invalid paramater");
					return;	
				}
			}
		} else {
			fprintf(stderr, "Invalid paramater");
			return;
		}
	}

	int input = -1;
	int at_key_index = 0;

	input = fgetc(stdin);

	while(input > -1){
	
		if(input != 10){
			char key_char = 0;
			if(key != 0){
				key_char = (enc)*(key[at_key_index] - 48);
			} else {
				if(input > 96 & input < 123){
					key_char = -32;				
				}
			}

			input = input + key_char;
			fputc(input, stdout);
			
			if(at_key_index == key_len - 1){
				at_key_index = 0;		
			} else {
				at_key_index = at_key_index + 1;
			}
		} else {
			at_key_index = 0;
			fputc(input, stdout);
		}

		input = fgetc(stdin);
	}
}


void task_1d(int argc, char **argv){
	
	int enc = 0;
	int key_len = 0, fileName_len = 0;
	char *key = 0, *fileName = 0;
	FILE *writeTo = stdout;
	
	for(int i=1; i < argc; i=i+1){
		int len;
		if((len = strlen(argv[i])) > 2){
			if(argv[i][0] == '-' & argv[i][1] == 'o'){
				fileName_len = strlen(argv[i]) - 2;
				fileName = (argv[i] + 2);
				continue;
			}

			if(argv[i][0] == '-' & argv[i][1] == 'e'){
				enc = -1;
				key_len = len - 2;
				key = (argv[i] + 2);
			} else {
				if(argv[i][0] == '+' & argv[i][1] == 'e'){
					enc = 1;
					key_len = len - 2;
					key = (argv[i] + 2);
				} else {
					fprintf(stderr, "Invalid paramater");
					return;	
				}
			}
		} else {
			fprintf(stderr, "Invalid paramater");
			return;
		}
	}

	if(fileName != 0){
		writeTo = fopen(fileName, "w");
	}

	int input = -1;
	int at_key_index = 0;

	input = fgetc(stdin);

	while(input > -1){
	
		if(input != 10){
			char key_char = 0;
			if(key != 0){
				key_char = (enc)*(key[at_key_index] - 48);
			} else {
				if(input > 96 & input < 123){
					key_char = -32;				
				}
			}

			input = input + key_char;
			fputc(input, writeTo);
			
			if(at_key_index == key_len - 1){
				at_key_index = 0;		
			} else {
				at_key_index = at_key_index + 1;
			}
		} else {
			at_key_index = 0;
			fputc(input, writeTo);
		}

		input = fgetc(stdin);
	}
}

void task_2(int argc, char **argv){
	
	int enc = 0;
	int key_len = 0, fileName_len = 0;
	char *key = 0, *fileName = 0;
	FILE *read_from = stdin;
	
	for(int i=1; i < argc; i=i+1){
		int len;
		if((len = strlen(argv[i])) > 2){
			if(argv[i][0] == '-' & argv[i][1] == 'i'){
				fileName_len = strlen(argv[i]) - 2;
				fileName = (argv[i] + 2);
				continue;
			}

			if(argv[i][0] == '-' & argv[i][1] == 'e'){
				enc = -1;
				key_len = len - 2;
				key = (argv[i] + 2);
			} else {
				if(argv[i][0] == '+' & argv[i][1] == 'e'){
					enc = 1;
					key_len = len - 2;
					key = (argv[i] + 2);
				} else {
					fprintf(stderr, "Invalid paramater");
					return;	
				}
			}
		} else {
			fprintf(stderr, "Invalid paramater");
			return;
		}
	}

	if(fileName != 0){
		read_from = fopen(fileName, "r");
	}

	int input = -1;
	int at_key_index = 0;

	input = fgetc(read_from);

	while(input > -1){
	
		if(input != 10){
			char key_char = 0;
			if(key != 0){
				key_char = (enc)*(key[at_key_index] - 48);
			} else {
				if(input > 96 & input < 123){
					key_char = -32;				
				}
			}

			input = input + key_char;
			fputc(input, stdout);
			
			if(at_key_index == key_len - 1){
				at_key_index = 0;		
			} else {
				at_key_index = at_key_index + 1;
			}
		} else {
			at_key_index = 0;
			fputc(input, stdout);
		}

		input = fgetc(read_from);
	}
}

int main(int argc, char **argv){
//	task_1a(argc, argv);
//	task_1b(argc, argv);
//	task_1c(argc, argv);
//	task_1d(argc, argv);
	task_2(argc, argv);
	return 0;
}
