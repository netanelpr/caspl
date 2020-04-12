#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BITS_IN_BYTE 8
#define KB (1 << 10)

char *buffer = NULL;
char *fileName = NULL;

typedef struct virus {
    unsigned short SigSize;
    char virusName[16];
    char* sig;
} virus;

typedef struct link link;
 
struct link {
    link *nextVirus;
    virus *vir;
};

link* link_ptr = NULL;

int setSigSize(virus *vir, FILE *file){
    unsigned char c1 = '\0', c2 = '\0';

    if((fread(&c1, 1, 1, file) != 1) | (fread(&c2, 1, 1, file) != 1)){
        return -1;
    }

    vir->SigSize = (c2 << BITS_IN_BYTE) + c1;
    return 1;
}

int setName(virus * vir, FILE *file){
    char *c = (char*)malloc(16);

    if(fread(c, 1, 16, file) != 16){
        free(c);
        return -1;
    }
    memcpy(vir->virusName, c, 16);
    free(c);
    return 1;
}

int setSeg(virus * vir, FILE *file){
    char * c = (char*)malloc(vir->SigSize);
    if(c == NULL){
        free(c);
        return -1;
    }

    if(fread(c, 1, vir->SigSize, file) != vir->SigSize){
        free(c);
        return -1;
    }

    vir->sig = c;
    return 1;
}

virus* readVirus(FILE* file){
    virus *vir = (virus*)malloc(sizeof(virus));

    if(vir == NULL){
        return NULL;
    }
    if(setSigSize(vir, file) < 0){
        free(vir);
        return NULL;
    }
    if(setName(vir, file) < 0){
        free(vir);
        return NULL;
    }
    if(setSeg(vir, file) < 0){
        free(vir);
        return NULL;
    }
    return vir;
}

void printVirus(virus* virus, FILE* output){
    fprintf(output, "Virus name : %s\n", virus->virusName);
    fprintf(output, "Virus size : %d\n", virus->SigSize);

    fprintf(output, "signature: \n");
    for(unsigned short i=1; i <= virus->SigSize; i=i+1){
        fprintf(output, "%02x ", (unsigned char)virus->sig[i-1]);

        if(i % 20 == 0){
            fprintf(output, "\n");
        }
    }
    fprintf(output, "\n");
}

/* Print the data of every link in list to the given stream. Each item followed by a newline character. */
void list_print(link *virus_list, FILE* output){
    while(virus_list != NULL){
        printVirus(virus_list->vir, output);
        virus_list = virus_list->nextVirus;
    }
}
     
/* Add a new link with the given data to the list 
        (either at the end or the beginning, depending on what your TA tells you),
        and return a pointer to the list (i.e., the first link in the list).
        If the list is null - create a new entry and return a pointer to the entry. */
link* list_append(link* virus_list, virus* data){
    link* data_link =(link*)malloc(sizeof(link));
    if(data_link == NULL){
        return NULL;
    }
    data_link->nextVirus = virus_list;
    data_link->vir = data;

    return data_link;
}
 
 /* Free the memory allocated by the list. */
void list_free(link *virus_list){
    while(virus_list != NULL){
        link *tmp = virus_list->nextVirus;
        if(virus_list->vir != NULL){
            free(virus_list->vir->sig);
            free(virus_list->vir);
        }
        free(virus_list);
        virus_list = tmp;
    }
}

void detect_virus(char *buffer, unsigned int size, link *virus_list){
    if(virus_list == NULL){
        return;
    }

    while(virus_list != NULL){
        unsigned short at_sig = 0;

        for(unsigned int at_buffer = 0; at_buffer < size; at_buffer = at_buffer +1){
            if(at_sig == virus_list->vir->SigSize -1){
                printf("At : %d\n",at_buffer - at_sig);
                printVirus(virus_list->vir, stdout);
                at_sig = 0;
            } else {
                if(virus_list->vir->sig[at_sig] == buffer[at_buffer]){
                    at_sig = at_sig + 1;
                } else {
                    at_sig = 0;
                }
            }
        }
        virus_list = virus_list->nextVirus;

    }
}

typedef struct fun_desc {
  char *name;
  void (*fun)();
}fun_desc;

void loadS(){
    list_free(link_ptr);
    link_ptr = NULL;

    char* fileName = (char*)malloc(128);
    FILE *sig = NULL;

    if(fileName == NULL){
        return;
    }

    fgets(fileName, 128, stdin);
    fileName[strlen(fileName)-1] = '\0';

    sig = fopen(fileName, "r");
    if(sig == NULL){
        return;
    }

    virus *vir = NULL;
    while((vir = readVirus(sig)) != NULL){
        link_ptr = list_append(link_ptr, vir);
    }
    
    fclose(sig);
    free(fileName);
}

void printS(){
    list_print(link_ptr, stdout);
}

void e(){
    if(buffer != NULL){
        free(buffer);
    }

    if(link_ptr != NULL){
        list_free(link_ptr);
    }
    exit(1);
}

void detect(){
    if(fileName == NULL){
        fprintf(stderr, "Didnt load a file to scan");
        return;
    }

    FILE *file = fopen(fileName, "r");
    if(file == NULL){
        fprintf(stderr, "Error openning the file");
        return;
    }

    buffer = (char*)malloc(10*KB);
    size_t numbraed = fread(buffer, 1, 10*KB, file);

    detect_virus(buffer, numbraed, link_ptr);
    free(buffer);
    buffer = NULL;
    fclose(file);
}

void display_menu(fun_desc function_desc[], int length){
    printf("Choose an option:\n");
    for(int i=0; i < length; i=i+1){
        printf("%d) %s\n",i, function_desc[i].name);
    }
}

void kill_virus(char *fileName, int signitureOffset, int signitureSize){
    FILE *file = fopen(fileName, "r+");
    if(file == NULL){
        fprintf(stderr, "Error openning the file");
        return;
    }

    fseek(file, signitureOffset, SEEK_SET);
    char c[1] = {0x90};
    for(int i=0; i<signitureSize; i=i+1){
        fwrite(c, 1,1, file);
    }
    fclose(file);

}

void removeV(){
    char* signitureOffset = (char*)malloc(128);
    char* signitureSize = (char*)malloc(128);
    int sO, siS;
    if((signitureOffset == NULL) | (signitureSize == NULL)){
        return;
    }

    fgets(signitureOffset, 128, stdin);
    signitureOffset[strlen(signitureOffset)-1] = '\0';


    fgets(signitureSize, 128, stdin);
    signitureSize[strlen(signitureSize)-1] = '\0';

    sO = atoi(signitureOffset);
    siS = atoi(signitureSize);
    kill_virus(fileName, sO, siS);

    free(signitureSize);
    free(signitureOffset);
}

void menu(){

    fun_desc function_desc[] = {{"Load signatures", loadS}, {"Print signatures", printS},
                            {"Detect viruses", detect}, {"Fix file", removeV},
                            {"Quit", e}, {NULL, NULL}};
    
    int menu_size = (int)(sizeof(function_desc)/sizeof(fun_desc) - 1);
    char option_input[menu_size+1];

    while(1){
        display_menu(function_desc, menu_size);
        printf("Option:");
        fgets(option_input, 3, stdin);
        int option = atoi(option_input);
        if((option > -1) & (option < menu_size)){
            printf("Within bounds\n");
            function_desc[option].fun();
            printf("DONE.\n");
        }
    }
}

int main(int argc, char **argv){
    if(argc > 1){
        fileName = argv[1];
    }

     menu();
}