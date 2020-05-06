#include <stdio.h>
#include <unistd.h>
#include <signal.h> 
#include <stdlib.h>

void handle_sigtstp(int sig){
	printf("%d recived SIGTSPT\n",getpid());
}

void handle_sigint(int sig){
	printf("%d recived SIGINT\n",getpid());
	exit(1);
}

void handle_sigcont(int sig){
	printf("%d recived SIGCONT\n",getpid());
}

int main(int argc, char **argv){ 

	signal(SIGTSTP, handle_sigtstp);
	signal(SIGINT, handle_sigint);
	signal(SIGCONT, handle_sigcont);

	while(1) {
		sleep(2);
	}

	return 0;
}