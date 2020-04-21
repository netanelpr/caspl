#include "util.h"

/*+++++++++++++++ Rapers for debug +++++++++++++++*/

int system_call_open_raper(int op, char *str, unsigned int mode);
int system_call_raper_rw(int op, int fd, char *str, int size);
int system_call_raper_lseek(int op, int fd, int offset, unsigned int mode);

/*+++++++++++++++ System call func +++++++++++++++*/

#define EOF (-1)

#define STDIN   0
#define STDOUT  1
#define STDERR  2

#define SYS_READ    3
#define SYS_WRITE   4
#define SYS_OPEN    5
#define SYS_LSEEK   19

#define O_RDONLY	00000000
#define O_WRONLY	00000001
#define O_RDWR      00000002
#define O_CREAT		00000100
#define O_TRUNC		00001000

# define SEEK_SET 0

int readByte(int fd, char *c){
    return system_call_raper_rw(SYS_READ, fd, c, 1);
}

int lseek_set(int fd, int offset){
    return system_call_raper_lseek(SYS_LSEEK, fd, offset, SEEK_SET);
}

int write(int fd, char *c, unsigned int size){
    return system_call_raper_rw(SYS_WRITE, fd, c, size);
}

int writeByte(int fd, char *c){
    return write(fd, c, 1);
}

int open(char *file_name, unsigned int mode){
    return system_call_open_raper(SYS_OPEN, file_name, mode);
}

int write_no_debug(int fd, char *c, unsigned int size){
    return system_call(SYS_WRITE, fd, c, size);
}

int writeByte_no_debug(int fd, char *c){
    return write_no_debug(fd, c, 1);
}

/*+++++++++++++++ assignemt code +++++++++++++++*/

int DEBUG_FLAG = 0;

#define BUFFER_SIZE 12
#define NEW_LINE 10

void print_debug_syscall(int op, int retVal){

    char tmp_c = '\t';
    write_no_debug(STDERR, "ID: ", 4);
    write_no_debug(STDERR, itoa(op), BUFFER_SIZE);
    writeByte_no_debug(STDERR, &tmp_c);

    tmp_c = NEW_LINE;
    write_no_debug(STDERR, "Ret: ", 5);
    write_no_debug(STDERR, itoa(retVal), BUFFER_SIZE);
    writeByte_no_debug(STDERR, &tmp_c);
}

/* use for open */
int system_call_open_raper(int op, char *str, unsigned int mode){
    int retVal = 0;

    retVal = system_call(op, str, mode);
    
    if(DEBUG_FLAG){
        print_debug_syscall(op, retVal);
    }

    return retVal;
}

int system_call_raper_rw(int op, int fd, char *str, int size){
    int retVal = 0;

    retVal = system_call(op, fd, str, size);
    
    if(DEBUG_FLAG){
        print_debug_syscall(op, retVal);
    }

    return retVal;
}

int system_call_raper_lseek(int op, int fd, int offset, unsigned int mode){
    int retVal = 0;

    retVal = system_call(op, fd, offset, mode);
    
    if(DEBUG_FLAG){
        print_debug_syscall(op, retVal);
    }

    return retVal;
}

#define DIF 32
#define BA 96
#define AZ 123

char print_debug_and_return_upper_case_char(char c, int fd){
    
    char inputChar = c;
    if((c > BA) & (c < AZ)){
        c = c - DIF;
    }

    /*if(DEBUG_FLAG){
        if(inputChar == NEW_LINE){
            write(STDERR, "\n", sizeof("\n"));
        } else {
            char tmp_c = '\t';

            write(STDERR, itoa(inputChar), BUFFER_SIZE);
            writeByte(STDERR, &tmp_c);

            tmp_c = NEW_LINE;
            write(STDERR, itoa(c), BUFFER_SIZE);
            writeByte(STDERR, &tmp_c);
        }
    }*/

    return c;
}

void print_upper_case(int ifd, int ofd){
    char inputChar = '\0';

    while(readByte(ifd, &inputChar) > 0){
        inputChar = print_debug_and_return_upper_case_char(inputChar, ofd);
        writeByte(ofd, &inputChar);
    }
}


int main (int argc , char* argv[], char* envp[])
{
    int ifd = STDIN, ofd = STDOUT;
    int i;

    for(i=1; i < argc; i=i+1){
        if(strncmp("-o", argv[i], 2) == 0){
            ofd = open(argv[i]+2, (O_WRONLY | O_CREAT | O_TRUNC));
            continue;
        }

        if(strncmp("-i", argv[i], 2) == 0){
            ifd = open(argv[i]+2, O_RDONLY);
            continue;
        }

        if(strncmp("-D", argv[i], 2) == 0){
            DEBUG_FLAG = 1;
        } else {
            write(STDERR, "Invalid arg\n", strlen("Invalid arg\n"));
            return 1;
        }
    }

    print_upper_case(ifd, ofd);
    
    return 0;
}