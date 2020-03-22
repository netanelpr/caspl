#include<stdio.h>
#include<string.h>

int DEBUG_FLAG = 0;
FILE *OSTREAM = 0;
FILE *ISTREAM = 0;

void printUpperCaseChar(char c){

    char inputChar = c;

    if((c > 96) & (c < 123)){
        c = c - 32;
    }

    if(DEBUG_FLAG){
        if(inputChar == 10){
            fprintf(stderr, "\n");
        } else {
            fprintf(stderr, "%d\t%d\n", inputChar, c);
        }
    }

    fputc(c, OSTREAM);
}

void printUpperCase(){
    char inputChar = fgetc(ISTREAM);

    while(inputChar > -1){
        printUpperCaseChar(inputChar);
        inputChar = fgetc(ISTREAM);
    }
}

void printEncryptedKey(int enc_flag, char *key, int len){

    char defult_key = (enc_flag)*32;
    if(len == 0){
        key = &defult_key;
        len = 1;
    }

    int at_key_index = 0;
    char inputChar = fgetc(ISTREAM);

    while(inputChar > -1){
        if(inputChar != 10){
            int enc_value = enc_flag*(key[at_key_index]-48);
            char entered_input = inputChar;

            inputChar = inputChar + enc_value;
            if(DEBUG_FLAG){
                if(entered_input == 10){
                    fprintf(stderr, "\n");
                } else {
                    fprintf(stderr, "%d\t%d\n", entered_input, inputChar);
                }
            }
            fputc(inputChar,OSTREAM);

            if(at_key_index == len-1){
                at_key_index = 0;
            } else {
                at_key_index = at_key_index + 1;
            }
        } else {
            at_key_index = 0;
            if(DEBUG_FLAG){
                fprintf(stderr, "\n");
            }
            fputc(inputChar,OSTREAM);
        }
        inputChar = fgetc(ISTREAM);
    }
}

int main(int argc, char **argv){

    int enc_flag = 0;
    char *enc_digits = 0;
    int enc_len = 0;

    OSTREAM = stdout;
    ISTREAM = stdin;

    for(int i=1; i < argc; i=i+1){
        if(strlen(argv[i]) < 2){
            return 1;
        }

        if(strncmp("-o", argv[i], 2) == 0){
            OSTREAM = fopen(argv[i]+2, "w");
            continue;
        }

        if(strncmp("-i", argv[i], 2) == 0){
            ISTREAM = fopen(argv[i]+2, "r");
            continue;
        }

        if(strncmp("-D", argv[i], 2) == 0){
            DEBUG_FLAG = 1;
        } else {
            if(strncmp("-e", argv[i], 2) == 0){
                enc_flag = -1;
                enc_digits = argv[i] + 2;
                enc_len = strlen(enc_digits);
                continue;
            }
            if(strncmp("+e", argv[i], 2) == 0){
                enc_flag = 1;
                enc_digits = argv[i] + 2;
                enc_len = strlen(enc_digits);
            } else {
                fprintf(stderr, "Invalid paramater %s\n", argv[i]);
                fflush(stderr);
                return 1;
            }
        }
    }

    if(enc_flag == 0){
        printUpperCase();
    } else {
        printEncryptedKey(enc_flag, enc_digits, enc_len);
    }
    return 0;
}