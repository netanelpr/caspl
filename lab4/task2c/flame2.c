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

/* move to util, null is true*/
int match_prefix(char *prefix, int prefix_size, char *str){
    int str_len = strlen(str);

    if(prefix == NULL){
        return 1;
    }

    if(prefix_size > str_len){
        return -1;
    }

    return strncmp(prefix, str, prefix_size) == 0 ? 1 : -1;
} 

int flame2(char *prefix){
    int fd, nread = 0;
    char buf[BUFFER];
    ent *dirent;
    int bpos;

    int write_to = STDIN;
    int prefix_len = 0;

    if(prefix != NULL){
        prefix_len = strlen(prefix);
    }

    fd = open(".", O_RDONLY | O_DIRECTORY, 0);
    if(fd < 0){
        write(STDERR, "Error opening the directory\n", sizeof("Error opening the directory\n"));
        return 0x55;
    }

    write(write_to, "Strat flame\n", sizeof("Strat flame\n"));

    for(;;){
        nread = getdirent(fd, buf, BUFFER);
        if(nread == -1){
          return 1;
        }

        if(nread == 0){
            break;
        }

        for (bpos = 0; bpos < nread;) {
            dirent = (ent*) (buf + bpos);

            if(match_prefix(prefix, prefix_len, dirent->name) > 0){
                write(write_to, dirent->name, strlen(dirent->name));
                writeByte(write_to, "\n");
            }

            bpos = bpos + dirent->len;
        }

    }
    
    close(fd);
    return 0;

}

extern void infection();
extern void infector(char *);

int main (int argc , char* argv[], char* envp[])
{
    int i;
    char * prefix = NULL;

    for(i=1; i < argc; i=i+1){
        if(strncmp("-p", argv[i], 2) == 0){
            prefix = argv[i] + 2;
            continue;
        }

        /*if(strncmp("-a", argv[i], 2) == 0){
            ifd = open(argv[i]+2, O_RDONLY);
            continue;
        }*/

        if(strncmp("-D", argv[i], 2) == 0){
            DEBUG_FLAG = 1;
        } else {
            write(STDERR, "Invalid arg\n", strlen("Invalid arg\n"));
            return 1;
        }
    }

    return flame2(prefix);

}