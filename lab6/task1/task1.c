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

#include "LineParser.h"

#define INPUT_SIZE 2<<10

//+++++++++++++ proccess +++++++++++++//

#define TERMINATED  -1
#define RUNNING 1
#define SUSPENDED 0

typedef struct process{
    cmdLine* cmd;                         /* the parsed command line*/
    pid_t pid; 		                  /* the process id that is running the command*/
    int status;                           /* status of the process: RUNNING/SUSPENDED/TERMINATED */
    struct process *next;	                  /* next process in chain */
} process;

void set_proc_status(char * str, int status){
    if(status == 1){
        strcpy(str, "RUNNING\0");
        return;
    }

    if(status == 0){
        strcpy(str, "SUSPENDED\0");
        return;
    }

    if(status == -1){
        strcpy(str, "TERMINATED\0");
        return;
    }

}

void addProcess(process** process_list, cmdLine* cmd, pid_t pid){
    process *proc = (process *)malloc(sizeof(process));

    proc->cmd = cmd;
    proc->pid = pid;
    proc->status = RUNNING;

    proc->next = *process_list;
    *process_list = proc;
}

void updateProcessList(process **process_list){
    int status;
    process *proc = *process_list;
    pid_t pid;

    while(proc != NULL){
         pid = waitpid(proc->pid, &status, WUNTRACED| 8 | WNOHANG);
         printf("%d \n",status);
         if (pid == -1) {  /* child ended                 */
            proc->status = TERMINATED;
            proc = proc->next;
            continue;
        }

        if(WIFSTOPPED(status)){
            proc->status = SUSPENDED;
        } else if (WIFEXITED(status)) {
            proc->status = TERMINATED;
        } else if (WIFSIGNALED(status)) {
            if(WTERMSIG(status) == SIGINT){
                proc->status = TERMINATED;
            }
        } else {
            proc->status = RUNNING;
        }

        proc = proc->next;
    }
}

void removeTerProc(process** process_list){
    process *proc = *process_list;
    process *prev;

    while(proc != NULL){
         if (proc->status == TERMINATED) {
             *process_list = proc->next;
            
            freeCmdLines(proc->cmd);
            free(proc);

            proc = *process_list;        
         } else{
             prev = proc;
             proc = proc->next;
             break;
         }
    }

    while(proc != NULL){
         if (proc->status == TERMINATED) {
             prev->next = proc->next;
            
            freeCmdLines(proc->cmd);
            free(proc);

            proc = prev->next;
         } else{
             prev = proc;
             proc = proc->next;
         }
    }
}

void printProcessList(process** process_list){

    process *proc = *process_list;
    char status[11];
    int index = 0;

    status[11] = '\0';

    updateProcessList(process_list);

    printf("INDEX\tPID\tSTATUS\t\tCMD AND ARGS\n");
    while(proc != NULL){
        int sindex = 0;
        set_proc_status(status, proc->status);
        printf("%d\t%d\t%s\t\t",index, proc->pid, status);

        while(proc->cmd->arguments[sindex] != NULL){
            printf("%s ",proc->cmd->arguments[sindex++]);
        }
        printf("\n");

        index = index + 1;
        proc = proc->next;
    }

    removeTerProc(process_list);

}

void freeProcessList(process* process_list){
    process *tmp;
    while(process_list != NULL){
        tmp = process_list->next;
        freeCmdLines(process_list->cmd);
        process_list = tmp;
    }
}

void updateProcessStatus(process* process_list, int pid, int status){
    while(process_list != NULL){
         if (pid == process_list->pid) {
             process_list->status = status;
             return;
         }
         process_list = process_list->next;
    }
}

int is_in_the_proc_list(process* process_list, int pid){
    while(process_list != NULL){
         if (pid == process_list->pid) {
             return 1;
         }
         process_list = process_list->next;
    }
    return 0;
}

void execute(cmdLine *pCmdLine, process** proc_list){
    pid_t pid;
    int ifd, ofd;

    if(pCmdLine->argCount == 0){
        return;
    }

    pid = fork();

    if(pid == -1){
        perror("fork error");
        return;
    }

    if(pid == 0){
        if(pCmdLine->outputRedirect != NULL){
            close(STDOUT_FILENO);
            ofd = open(pCmdLine->outputRedirect, O_WRONLY | O_CREAT, 0644);
            if(ofd == -1){
                perror("open");
                _exit(1);
            }
        }

        if(pCmdLine->inputRedirect != NULL){
            close(STDIN_FILENO);
            ifd = open(pCmdLine->inputRedirect, O_RDONLY, 0);
            if(ifd == -1){
                perror("open");
                _exit(1);
            }
        }

        if(execvp(pCmdLine->arguments[0], pCmdLine->arguments) != 0){
            perror("exec error");
            _exit(0);
        }
    } else {
        addProcess(proc_list, pCmdLine, pid);
        if(pCmdLine->blocking == 1){
            int status;
            waitpid(pid, &status, 0);
        }
    }
}

void run_cmd_lines(cmdLine *pCmdLine, process** proc_list){
    while(pCmdLine){
        execute(pCmdLine, proc_list);
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

//+++++++++++++ var list +++++++++++++//

typedef struct varlist{
    char *name;
    char *val;
    struct varlist *next;
} varlist;

void addToVarList(varlist **list, char *name, char *val){
    varlist *vl;
    if((name == NULL) | (val == NULL)){
        return;
    }

    vl = *list;
    while(vl != NULL){
        if(strcmp(name, vl->name)){
            free(vl->val);
            vl->val = (char *)malloc(strlen(val));
            if(vl-> val == NULL){
                perror("malloc");
                exit(1);
            }
            strcpy(vl->val, val);
            return;
        }
    }

    vl = (varlist *)malloc(sizeof(varlist));
    if(vl == NULL){
        perror("vl");
        exit(1);
    }

    vl->name = (char *)calloc(strlen(name)+1, 1);
    vl->val = (char *)calloc(strlen(val)+1, 1);
    if((vl-> val == NULL) | (vl->name == NULL)){
        perror("malloc");
        exit(1);
    }
    strcpy(vl->name, name);
    strcpy(vl->val, val);

    vl->next = *list;
    *list = vl;
}

void freeVarlist(varlist *list){
    while(list != NULL){
        free(list->name);
        free(list->val);
        list = list->next;
    }
}

void print_var_list(varlist *list){
    while(list != NULL){
        printf("name: %s\tvalue: %s\n", list->name, list->val);
        list = list->next;
    }
}

//+++++++++++++ shell +++++++++++++//

void set_var_shell(varlist *varl, cmdLine *pCmdLine){

}


void run_shell(){
    char *path_str = (char*)calloc(PATH_MAX, 1);
    char *input = (char*)calloc(INPUT_SIZE, 1);
    cmdLine *pCmdLine = NULL;
    process *proc = NULL;
    varlist *varl = NULL;

    if(setcwd(path_str) < 0){
        return;
    }

    for(;;){
        printf("%s> ", path_str);
        if(fgets(input, INPUT_SIZE, stdin) != NULL){
            pCmdLine = parseCmdLines(input);
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

            if(strcmp(input, "proc\n") == 0){
                printProcessList(&proc);
                continue;
            }

            if(strncmp(input, "suspend", 7) == 0){
                int pid = 0;
                sscanf(input+8, "%d\n", &pid);
                if(pid > 0 && (is_in_the_proc_list(proc, pid) == 1)){
                    printf("send suspend to %d",pid);
                    kill(pid, SIGTSTP);
                }
                continue;
            }

            if(strncmp(input, "kill", 4) == 0){
                int pid = 0;
                sscanf(input+5, "%d\n", &pid);
                if(pid > 0 && (is_in_the_proc_list(proc, pid) == 1)){
                    kill(pid, SIGINT);
                }
                continue;
            }

            if(strncmp(input, "wake", 4) == 0){
                int pid = 0;
                sscanf(input+5, "%d\n", &pid);
                if(pid > 0 && (is_in_the_proc_list(proc, pid) == 1)){
                    kill(pid, SIGCONT);
                }
                continue;
            }
            if(pCmdLine->argCount == 3){
                if(strcmp(pCmdLine->arguments[0], "set") == 0){
                    addToVarList(&varl, pCmdLine->arguments[1],
                                pCmdLine->arguments[2]);
                    continue;
                }
            }

            if(pCmdLine->argCount == 1){
                if(strcmp(pCmdLine->arguments[0], "vars") == 0){
                    print_var_list(varl);
                    continue;
                }
            }

            run_cmd_lines(pCmdLine, &proc);

        } else {

        }
    }
    
    free(path_str); free(input);
    freeProcessList(proc);
    freeVarlist(varl);
}

int main(int argc, char** argv){
    run_shell();
    return 0;
}