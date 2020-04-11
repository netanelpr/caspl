#include <stdio.h>

int main(int argc, char **argv){
    char *fileName = NULL;
    FILE *fileToRead = NULL;
    unsigned char read = '\0';

    if(argc != 2){
        return 1;
    }

    fileName = argv[1];
    fileToRead = fopen(fileName, "r");

    if(fileToRead == NULL){
        fprintf(stderr, "Error opening the file");
        return 1;
    }

    while(read != EOF){
        int raed = fread(&read, 1, 1, fileToRead);

        if(raed != 1){
            return 1;
        }

        printf("%02x ", read);
    }

    printf("\n");
}

