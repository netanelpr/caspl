#include <stdio.h>

int digit_cnt(char *str){
    int c = 0;
    while(*str != 0){
        if(('0' <= *str) & (*str <= '9')){
            ++c;
        }
        str = str + 1;
    }

    return c;
}

/*
    set file to digitc
    set unit size to 1
    read into memory 66 bytes from digitc at 52d
    set file ntsc
    use save into file with 0 577 66
*/

int main(int argc, char **argv){
    printf("%d\n", digit_cnt(argv[1]));
    return 0;
}