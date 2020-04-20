#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BITS_IN_BYTE 8
#define KB (1 << 10)

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

void v_free(virus *v){
    if(v != NULL){
        if(v->sig != NULL){
            free(v->sig);
        }
        free(v);
    }
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
        v_free(virus_list->vir);
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
        int sigSize = virus_list->vir->SigSize;
        for(unsigned int at_buffer = 0; at_buffer < size; at_buffer = at_buffer + 1){
            if(at_sig == sigSize -1){
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
  link* (*fun)(link*);
}fun_desc;

link* loadS(link* list){

    char* fileName;
    FILE *sig_file = NULL;
    virus *vir = NULL;

    if(list != NULL){
        list_free(list);
        list = NULL;
    }

    fileName = (char*)malloc(128);
    if(fileName == NULL){
        return NULL;
    }

    fgets(fileName, 128, stdin);
    fileName[strlen(fileName)-1] = '\0';

    sig_file = fopen(fileName, "r");
    if(sig_file == NULL){
        return NULL;
    }

    while((vir = readVirus(sig_file)) != NULL){
        list = list_append(list, vir);
    }
    
    fclose(sig_file);
    free(fileName);

    return list;
}

link* printS(link* list){
    list_print(list, stdout);
    return list;
}

link* e(link* list){
    if(list != NULL){
        list_free(list);
    }
    exit(1);

    return list;
}

link* detect(link* list){
    char *buffer;

    if(fileName == NULL){
        fprintf(stderr, "Didnt load a file to scan");
        return NULL;
    }

    FILE *file = fopen(fileName, "r");
    if(file == NULL){
        fprintf(stderr, "Error openning the file");
        return NULL;
    }

    buffer = (char*)malloc(10*KB);
    size_t numbraed = fread(buffer, 1, 10*KB, file);

    detect_virus(buffer, numbraed, list);

    free(buffer);
    buffer = NULL;
    fclose(file);

    return list;
}

void display_menu(fun_desc function_desc[], int length){
    printf("Choose an option:\n");
    for(int i=0; i < length; i=i+1){
        printf("%d) %s\n",i, function_desc[i].name);
    }
}


void menu(){

    fun_desc function_desc[] = {{"Load signatures", loadS}, {"Print signatures", printS},
                            {"Detect viruses", detect}, {"Quit", e}, {NULL, NULL}};

    link *list = NULL;
    
    int menu_size = (int)(sizeof(function_desc)/sizeof(fun_desc) - 1);
    char option_input[menu_size+1];

    while(1){
        int option;
        display_menu(function_desc, menu_size);
        printf("Option:");

        fgets(option_input, 3, stdin);
        sscanf(option_input ,"%d", &option);

        if((option > -1) & (option < menu_size)){
            printf("Within bounds\n");
            list = function_desc[option].fun(list);
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