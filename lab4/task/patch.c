#include "util.h"

#define EOF (-1)

#define STDOUT 1
#define STDERR 2

#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_LSEEK 19

#define O_RDWR 0x00000002

# define SEEK_SET 0

int readByte(int fd, char *c){
    return system_call(SYS_READ, fd, c, 1);
}

int writeByte(int fd, char *c){
    return system_call(SYS_WRITE, fd, c, 1);
}

int lseek_set(int fd, int offset){
    return system_call(SYS_LSEEK, fd, offset, SEEK_SET);
}

int main (int argc , char* argv[], char* envp[])
{
    int fd = -1;
    char shiraStr[] = "Shira";
    char raed_char = '\0';
    int for_index = 0, index = 0, at_index_shiraStr = 0;

    if(argc != 3){
        system_call(SYS_WRITE, STDERR, "Invalid number of args\n", strlen("Invalid number of args\n"));
        return 1;
    }

    fd = system_call(SYS_OPEN, argv[1], O_RDWR);
    if(fd < 3){
        system_call(SYS_WRITE, STDERR, "Error at opennnig the file\n", strlen("Error at opennnig the file\n"));
        return 1;
    }


    while(readByte(fd, &raed_char) != EOF){
        if(at_index_shiraStr == strlen(shiraStr)){
            index = index - strlen(shiraStr);
            if(lseek_set(fd, index) == index){
                    for(; for_index < strlen(argv[2]); for_index=for_index+1){
                        writeByte(fd, &argv[2][for_index]);
                    }
            }
            break;
        }

        if(raed_char == shiraStr[at_index_shiraStr]){
            at_index_shiraStr = at_index_shiraStr + 1;
        } else {
            at_index_shiraStr = 0;
        }

        index = index + 1;
    }


    return 0;
}