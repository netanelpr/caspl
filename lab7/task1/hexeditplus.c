#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
  char debug_mode;
  char file_name[128];
  int unit_size;
  unsigned char mem_buf[10000];
  size_t mem_count;
} state;

void freeStateStruct(state *st){
    /*if(st->file_name != NULL){
        free(st->file_name);
    }

    if(st->mem_buf !=NULL){
        free(st->mem_buf);
    }*/

    free(st);
}

void debug_print(state *st){
    if(st->debug_mode != 0){
        fprintf(stderr, "DEBUG:\n\tunitSize: %d\n\tfilename: %s\n\tmemory count: %d\n",
            st->unit_size, st->file_name, st->mem_count);
    }
}

void debug_string_print(char debug_mode, char *str){
    if(debug_mode != 0){
    fprintf(stderr, "DEBUG: %s\n", str);
    }
}

typedef struct fun_desc {
  char *name;
  void (*fun)(state *);
} fun_desc;


void toggle_debug_mode(state *st){
    if(st->debug_mode == 0){
        st->debug_mode = 1;
        printf("Debug flag now on\n");
    } else {
        st->debug_mode = 0;
        printf("Debug flag now off\n"); 
    }
}


void set_file_name(state *st){
    char file_name[128];
    fgets(file_name, 128, stdin);
    sscanf(file_name, "%s\n", st->file_name);

    if(st->debug_mode != 0){
        fprintf(stderr, "DEBUG: Set file name to %s\n", st->file_name);
    }

}

void set_unit_size(state *st){
    char file_name[16];
    int input_num;
    fgets(file_name, 16, stdin);
    sscanf(file_name, "%d\n", &input_num);

    if((input_num == 1) | (input_num == 2) | (input_num == 4)){
        st->unit_size = input_num;
        
        if(st->debug_mode != 0){
            fprintf(stderr, "DEBUG: Set unit size to %d\n", st->unit_size);
        }
    }

    
}

void quit(state *st){
    freeStateStruct(st);
    exit(0);
}

void display_menu(fun_desc function_desc[], int length){
    printf("Choose action:\n");
    for(int i=0; i < length; i=i+1){
        printf("%d) %s\n",i, function_desc[i].name);
    }
}

void menu(){
    int option = 0;
    state *st;
    fun_desc function_desc[] = {{"Toggle Debug Mode", toggle_debug_mode}, {"Set File Name", set_file_name},
                                {"Set Unit Size", set_unit_size},
                                {"Quit", quit}, {NULL, NULL}};
    
    int menu_size = (int)(sizeof(function_desc)/sizeof(fun_desc) - 1);
    char option_input[menu_size+1];

    st = (state*)calloc(sizeof(state), 1);
    if(st == NULL){
        fprintf(stderr, "Error at allocating memory\n");
        return;
    }

    while(1){
        display_menu(function_desc, menu_size);
        printf("Option:");
        fgets(option_input, menu_size, stdin);
        sscanf(option_input, "%d\n", &option);
        if((option > -1) & (option < menu_size)){
            debug_print(st);
            function_desc[option].fun(st);
        }
    }
}
 
int main(int argc, char **argv){
    menu();
}