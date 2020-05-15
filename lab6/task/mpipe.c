#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char **argv){

    int pipefd[2];
    pid_t pid;
    char buf;

    if(pipe(pipefd) == -1){
        perror("pipe");
        exit(1);
    }

    pid = fork();
    if(pid == -1){
        perror("fork");
        exit(1);
    }

    if(pid == 0){
        close(pipefd[0]);

        write(pipefd[1], "Hello", 5);
        
        close(pipefd[1]);
        _exit(0);
    } else {
        close(pipefd[1]);
        
        write(STDOUT_FILENO, "Recive: ", 8);
        while (read(pipefd[0], &buf, 1) > 0)
        {
                write(STDOUT_FILENO, &buf, 1);
        }
        write(STDOUT_FILENO, "\n", 1);

        close(pipefd[0]);
        wait(NULL);
        exit(0);
    }



}