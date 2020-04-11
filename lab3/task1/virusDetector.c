#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BITS_IN_BYTE 8


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
        free(virus_list);
        virus_list = tmp;
    }
}

typedef struct fun_desc {
  char *name;
  void (*fun)();
}fun_desc;

void loadS(){
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
    printf("1");


    virus *vir = NULL;
    while((vir = readVirus(sig)) != NULL){
        link_ptr = list_append(link_ptr, vir);
    }

    free(fileName);
    free(sig);
}

void printS(){
    list_print(link_ptr, stdout);
}

void e(){
    exit(1);
}

void display_menu(fun_desc function_desc[], int length){
    printf("Choose an option:\n");
    for(int i=0; i < length; i=i+1){
        printf("%d) %s\n",i, function_desc[i].name);
    }
}

void menu(){

    fun_desc function_desc[] = {{"Load signatures", loadS}, {"Print signatures", printS},
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
     menu();
}