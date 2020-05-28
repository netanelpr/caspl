#include <linux/limits.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

#include <errno.h>

#define INPUT_END 0
#define OUTPUT_END 1

int DEBUG = -1;

void print_debug(char *str){
    if(DEBUG == 0){
        fprintf(stderr, "%s\n", str);
    }
}

void print_debug_arg(char *str, int pid){
    if(DEBUG == 0){
        fprintf(stderr, "%s %d\n", str, pid);
    }
}

int fork_debug(){
    int pid;
    print_debug("(parent_process>forking…)");
    pid = fork();
    print_debug_arg("(parent_process>created process with id: %d)", pid);
    return pid;
}

void execute(){
    int pipefd[2];
    pid_t pid1, pid2;
    char *ls[] = {"ls", "-l", NULL};
    char *tail[] = {"tail", "-n", "2", NULL};
    int wstatus;

    if(pipe(pipefd) == -1){
        perror("pipe");
        exit(1);
    }

    pid1 = fork_debug();
    if(pid1 == -1){
        perror("fork");
        exit(1);
    }

    if(pid1 == 0){
        print_debug("(child1>redirecting stdout to the write end of the pipe…)");
        close(STDOUT_FILENO);
        dup(pipefd[OUTPUT_END]);
        close(pipefd[OUTPUT_END]);
        print_debug("(child1>going to execute cmd: …)");
        execvp(ls[0], ls);
        _exit(0);
    } else {
        print_debug("(parent_process>closing the write end of the pipe…)");
        close(pipefd[OUTPUT_END]);

        pid2 = fork_debug();
        if(pid2 == -1){
            perror("fork");
            exit(1);
        }

        if(pid2 == 0){
            print_debug("(child2>redirecting stdin to the read end of the pipe…)");
            close(STDIN_FILENO);
            dup(pipefd[INPUT_END]);
            close(pipefd[INPUT_END]);
            print_debug("(child2>going to execute cmd: …)");
            execvp("tail", tail);
            _exit(0);
        } else {
            print_debug("(parent_process>closing the read end of the pipe…)");
            close(pipefd[INPUT_END]);
        }
        print_debug("(parent_process>waiting for child processes to terminate…)");
        waitpid(pid1, &wstatus, 0);
        print_debug("(parent_process>waiting for child processes to terminate…)");
        waitpid(pid1, &wstatus, 0);
        print_debug("(parent_process>exiting…)");
    }
}

int main(int argc, char **argv){

    if(argc == 2){
        if(strncmp("-d", argv[1], 2)==0){
            DEBUG = 0;
        }
    }
    execute();
    return 1;
}