#include <linux/limits.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#include <errno.h>

#include "LineParser.h"

#define INPUT_SIZE 2<<10

void execute(cmdLine *pCmdLine){
    pid_t pid;

    if(pCmdLine->argCount == 0){
        return;
    }

    pid = fork();

    if(pid == -1){
        perror("fork error");
    }

    if(pid == 0){
        if(execvp(pCmdLine->arguments[0], pCmdLine->arguments) != 0){
            perror("exec error");
            _exit(0);
        }
    } else {
        if(pCmdLine->blocking == 1){
            int status;
            waitpid(pid, &status, 0);
        }
    }
}

void run_cmd_lines(cmdLine *pCmdLine ){
    while(pCmdLine){
        execute(pCmdLine);
        pCmdLine = pCmdLine->next;
    }
}

void replace_nl_to_nt(char *str){
    char * schr = strchr(str, '\n');

    if(schr != NULL){
        schr[0] = 0;
    }
}

int setcwd(char *path_str){
    if(getcwd(path_str, PATH_MAX) == NULL){
        perror("getcwd");
        return -1;
    }
    return 1;
}

int check_cd_cmd(char *input, char *path_str){
    if(strncmp(input, "cd\n", 3) == 0){
        return 1;
    }
    
    if(strncmp(input, "cd ", 3) == 0){
        replace_nl_to_nt(input);

        if(chdir((input + 3)) == 0){
            if(setcwd(path_str) < 0){
                return 0;
            }
        } else {
            perror("change dir");
            return 0;
        }

        return 1;
    }

    return -1;
}

void run_shell(){
    char *path_str = (char*)calloc(PATH_MAX, 1);
    char *input = (char*)calloc(INPUT_SIZE, 1);
    cmdLine *pCmdLine = NULL;

    if(setcwd(path_str) < 0){
        return;
    }


    for(;;){

        printf("%s> ", path_str);
        if(fgets(input, INPUT_SIZE, stdin) != NULL){
            int is_cd = -1;
            if(strcmp(input, "q\n") == 0){
                break;
            }
            
            is_cd = check_cd_cmd(input, path_str);
            if(is_cd > 0){
                continue;
            }
            if(is_cd == 0){
                break;
            }

            pCmdLine = parseCmdLines(input);
            run_cmd_lines(pCmdLine);

        } else {

        }

        freeCmdLines(pCmdLine);
    }

    free(path_str); free(input);
}

int main(int argc, char** argv){
    run_shell();
    return 0;
}