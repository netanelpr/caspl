#include<stdio.h>
#include<string.h>

int main(int argc, char **argv){
	for(int i=1; i < argc - 1; i=i+1){
		printf("%s ", argv[i]);
	}
	if(argc > 1){
		printf("%s\n", argv[argc-1]);
	}

	return 0;
}
