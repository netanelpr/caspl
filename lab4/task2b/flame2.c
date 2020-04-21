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
#define SYS_GETDENTS    141

#define O_RDONLY	00000000
#define O_WRONLY	00000001
#define O_RDWR      00000002
#define O_CREAT		00000100
#define O_TRUNC		00001000
#define O_DIRECTORY	00200000

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

void print_debug_syscall(int op, int retVal){

    char tmp_c = '\t';
    write_no_debug(STDERR, "ID: ", 4);
    write_no_debug(STDERR, itoa(op), BUFFER_SIZE_DEBUG);
    writeByte_no_debug(STDERR, &tmp_c);

    tmp_c = NEW_LINE;
    write_no_debug(STDERR, "Ret: ", 5);
    write_no_debug(STDERR, itoa(retVal), BUFFER_SIZE_DEBUG);
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

int system_call_raper_getdirent(int fd, char *buf, unsigned int size){
    int retVal = 0;

    retVal = system_call(SYS_GETDENTS, fd, buf, size);
    
    if(DEBUG_FLAG){
        print_debug_syscall(SYS_GETDENTS, retVal);
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

    fd = system_call_open_raper(SYS_OPEN, ".", O_RDONLY | O_DIRECTORY);
    if(fd < 0){
        write(STDERR, "Error opening the directory\n", sizeof("Error opening the directory\n"));
        return 0x55;
    }

    write(write_to, "Strat flame\n", sizeof("Strat flame\n"));

    for(;;){
        nread = system_call_raper_getdirent(fd, buf, BUFFER);
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

    return 0;

}

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