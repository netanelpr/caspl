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
        if(execvp(pCmdLine->arguments[0], pCmdLine->arguments) == 0){
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

void run_shell(){

    char *path_str = (char*)calloc(PATH_MAX, 1);
    char *input = (char*)calloc(INPUT_SIZE, 1);
    cmdLine *pCmdLine = NULL;

    if(getcwd(path_str, PATH_MAX) == NULL){
        return;
    }


    for(;;){
        printf("%s> ", path_str);
        if(fgets(input, INPUT_SIZE, stdin) != NULL){
            if(strcmp(input, "q\n") == 0){
                break;
            }

            if(strncmp(input, "cd ", 3) == 0){
                strchr(input, '\n')[0] = 0;
                if(chdir((input + 3)) == 0){
                    if(getcwd(path_str, PATH_MAX) == NULL){
                        return;
                    }
                } else {
                    fprintf(stderr, "%s\n", strerror(errno));
                }
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