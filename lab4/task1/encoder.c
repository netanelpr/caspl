#include "util.h"

/*+++++++++++++++ Rapers for debug +++++++++++++++*/

typedef unsigned long ulong;
extern int system_call(ulong arg1, ulong arg2,ulong arg3, ulong arg4);

int system_call_debug_wrapper(ulong arg1, ulong arg2,ulong arg3, ulong arg4);
/*+++++++++++++++ System call func +++++++++++++++*/

#define EOF (-1)

#define STDIN   0
#define STDOUT  1
#define STDERR  2

#define SYS_READ    3
#define SYS_WRITE   4
#define SYS_OPEN    5
#define SYS_CLOSE   6
#define SYS_LSEEK   19
#define SYS_GETDENTS    141

#define O_RDONLY	00000000
#define O_WRONLY	00000001
#define O_RDWR      00000002
#define O_CREAT		00000100
#define O_TRUNC		00001000
#define O_DIRECTORY	00200000

#define SEEK_SET 0

#define S_RWE 511

int readByte(int fd, char *c){
    return system_call_debug_wrapper(SYS_READ, (ulong)fd, (ulong)c, (ulong)1);
}

int lseek_set(int fd, int offset){
    return system_call_debug_wrapper(SYS_LSEEK, (ulong)fd, (ulong)offset, SEEK_SET);
}

int write(int fd, char *c, unsigned int size){
    return system_call_debug_wrapper(SYS_WRITE, (ulong)fd, (ulong)c, (ulong)size);
}

int writeByte(int fd, char *c){
    return write(fd, c, 1);
}

int open(char *file_name, int flag, int mode){
    return system_call_debug_wrapper(SYS_OPEN, (ulong)file_name, (ulong)flag, (ulong)mode);
}

int close(int fd){
    return system_call_debug_wrapper(SYS_CLOSE, (ulong)fd, 0, 0);
}

int getdirent(int fd, char *buf, int size){
    return system_call_debug_wrapper(SYS_GETDENTS, (ulong)fd, (ulong)buf, (ulong)size);
}

int write_no_debug(int fd, char *c, int size){
    return system_call(SYS_WRITE, (ulong)fd, (ulong)c, (ulong)size);
}

int writeByte_no_debug(int fd, char *c){
    return write_no_debug(fd, c, 1);
}

/*+++++++++++++++ assignemt code +++++++++++++++*/

int DEBUG_FLAG = 0;

#define BUFFER_SIZE_DEBUG   12
#define BUFFER_SIZE_NUMBER  12
#define BUFFER              8192
#define NEW_LINE 10
#define NULL 0

typedef struct ent {
    int	    indoe;
	int	    offset;
	short	len;
	char	name[];
} ent;

void print_debug_syscall(ulong arg1, ulong arg2,ulong arg3, ulong arg4, int retVal){

    char tmp_c = '\t';
    write_no_debug(STDERR, "ID: ", 4);
    write_no_debug(STDERR, itoa((int)arg1), BUFFER_SIZE_DEBUG);
    writeByte_no_debug(STDERR, &tmp_c);

    write_no_debug(STDERR, "arg2 : ", 7);
    write_no_debug(STDERR, itoa((int)arg2), BUFFER_SIZE_DEBUG);
    writeByte_no_debug(STDERR, &tmp_c);

    write_no_debug(STDERR, "arg3 : ", 7);
    write_no_debug(STDERR, itoa((int)arg3), BUFFER_SIZE_DEBUG);
    writeByte_no_debug(STDERR, &tmp_c);

    write_no_debug(STDERR, "arg4 : ", 7);
    write_no_debug(STDERR, itoa((int)arg4), BUFFER_SIZE_DEBUG);
    writeByte_no_debug(STDERR, &tmp_c);

    tmp_c = NEW_LINE;
    write_no_debug(STDERR, "Ret: ", 5);
    write_no_debug(STDERR, itoa(retVal), BUFFER_SIZE_DEBUG);
    writeByte_no_debug(STDERR, &tmp_c);
}

int system_call_debug_wrapper(ulong arg1, ulong arg2,ulong arg3, ulong arg4){
    int retVal = 0;

    retVal = system_call(arg1, arg2, arg3, arg4);
    
    if(DEBUG_FLAG){
        print_debug_syscall(arg1, arg2, arg3, arg4, retVal);
    }

    return retVal;
}

#define DIF 32
#define BA 96
#define AZ 123

char upper_case(char c, int fd){
    
    if((c > BA) & (c < AZ)){
        c = c - DIF;
    }

    return c;
}

void print_upper_case(int ifd, int ofd){
    char inputChar = '\0';

    while(readByte(ifd, &inputChar) > 0){
        inputChar = upper_case(inputChar, ofd);
        writeByte(ofd, &inputChar);
    }
}


int main (int argc , char* argv[], char* envp[])
{
    int ifd = STDIN, ofd = STDOUT;
    int i;

    for(i=1; i < argc; i=i+1){
        if(strncmp("-o", argv[i], 2) == 0){
            ofd = open(argv[i]+2, (O_WRONLY | O_CREAT | O_TRUNC), S_RWE);
            continue;
        }

        if(strncmp("-i", argv[i], 2) == 0){
            ifd = open(argv[i]+2, O_RDONLY, 0);
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
    close(ifd);
    close(ofd);
    
    return 0;
}