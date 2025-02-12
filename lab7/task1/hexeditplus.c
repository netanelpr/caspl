#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <elf.h>

#define DECIMAL_DISPLAY 0
#define HEX_DISPLAY 1

#define VM_S 0x08048000

typedef struct {
  char debug_mode;
  char file_name[128];
  int unit_size;
  unsigned char mem_buf[10000];
  size_t mem_count;
  char dispaly_mode;
} state;

//+++++++++++ elf +++++++++++//

Elf32_Ehdr* get_elf_header(FILE *file){
    Elf32_Ehdr *header = (Elf32_Ehdr*)malloc(sizeof(Elf32_Ehdr));

    fseek(file, 0, SEEK_SET);
    fread((char *)header, sizeof(Elf32_Ehdr), 1, file);
    
    return header;
}

Elf32_Shdr* get_section_of_address(char *file_name, uint address){
    FILE *file;
    int section_table_size, index;
    Elf32_Word sec_offset, section_size;
    Elf32_Ehdr *header;
    Elf32_Shdr *shdr = (Elf32_Shdr*)malloc(sizeof(Elf32_Shdr));
    
    if(shdr == NULL){
        return NULL;
    }

    file = fopen(file_name, "r");
    if(file == NULL){
        perror("Open file");
        free(shdr);
        return NULL;
    }

    header = get_elf_header(file);
    section_table_size = header->e_shnum;
    sec_offset = header->e_shoff;
    fseek(file, sec_offset, SEEK_SET);
    for(index = 0; index < section_table_size; index = index + 1){

            fread((char *)shdr, sizeof(Elf32_Shdr), 1, file);

            section_size = shdr->sh_size;
            if(shdr->sh_addr <= address){
                if(address <= (shdr->sh_addr + section_size)){
                    fclose(file);
                    return shdr;
                }
            }
    }

    free(shdr);
    fclose(file);
    return NULL;

}

//+++++++++++ hexeditplus +++++++++++//

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

void print_dispaly_mode(char display_mode){
    if(display_mode == DECIMAL_DISPLAY){
        printf("Decimal\n");
    } else {
        printf("Hexadecimal\n");
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

void get_location_and_len(size_t *location, size_t *len){
    char input[128];
    printf("Please enter <location> <length>\n");
    fgets(input, 128, stdin);
    sscanf(input, "%x %d", location, len);
}

void load_into_memory(state *st){
    FILE *file;
    size_t location, len;

    if(st->file_name == NULL){
        fprintf(stderr, "The file name is not set");
        return;
    }

    file = fopen(st->file_name, "r");
    if(file == NULL){
        perror("Open file");
        return;
    }

    get_location_and_len(&location, &len);
    if(st->debug_mode != 0){
        fprintf(stderr, "DEBUG: load int memory\n\tfilename:%s\n\tlocation: %zu\n\tlen: %zu\n", 
                st->file_name, location, len);
    }

    fseek(file, location, SEEK_SET);
    fread(st->mem_buf, st->unit_size, len, file);
    st->mem_count = len;
    printf("Loaded %zu units into memory\n", len);

    fclose(file);
}

void toggle_display_mode(state *st){
    if(st->dispaly_mode == DECIMAL_DISPLAY){
        st->dispaly_mode = HEX_DISPLAY;
        printf("Display flag now on, hexadecimal representation\n");
    } else {
        st->dispaly_mode = DECIMAL_DISPLAY;
        printf("Display flag now off, decimal representation\n");  
    }
}

char* unit_to_format(int unit) {
    static char* formats[] = {"%#hhx\n", "%#hx\n", "No such unit", "%#x\n"};
    return formats[unit-1];
}  

void print_units(FILE* output, char* buffer, int count, int unit_size, char display_mode) {
    char* end = buffer + unit_size * count;
    while (buffer < end) {
        //print ints
        int var = *((int*)(buffer));
        if(display_mode == DECIMAL_DISPLAY){
            fprintf(output, "%d\n", var);
        } else {
            fprintf(output, unit_to_format(unit_size), var);
        }
        buffer += unit_size;
    }
}


void get_len_and_addr(int *len, size_t *addr){
    char input[128];
    fgets(input, 128, stdin);
    sscanf(input, "%x %d", addr, len);
}

void dispaly_memory_from_state(state *st, int len){    
    print_dispaly_mode(st->dispaly_mode);
    printf("==========\n");
    print_units(stdout, (char *)st->mem_buf, len, st->unit_size, st->dispaly_mode);
}

void dispaly_memory_from_the_file(state *st, int len, size_t addr){   
    FILE *file;
    Elf32_Shdr *shdr;
    char *buff = NULL;

    shdr = get_section_of_address(st->file_name, addr);
    if(shdr == NULL){
        debug_string_print(st->debug_mode, "Invalid address");
        return;
    }

    file = fopen(st->file_name, "r");
    if(file == NULL){
        perror("Open file");
        free(shdr);
        return;
    }

    fseek(file, shdr->sh_offset, SEEK_SET);
    buff = (char *)malloc(len * st->unit_size);
    if(buff == NULL){
        perror("Malloc");
        return;
    }
    fread(buff, len, st->unit_size, file);

    print_dispaly_mode(st->dispaly_mode);
    printf("==========\n");
    print_units(stdout, buff, len, st->unit_size, st->dispaly_mode);

    free(buff);
    free(shdr);
    fclose(file);
}

void memory_display(state *st){
    int len = 0;
    size_t addr = 0;

    if(st->file_name == NULL){
        fprintf(stderr, "The file name is not set");
        return;
    }

    printf("Enter address and length\n");
    get_len_and_addr(&len, &addr);
    if(addr == 0){
        dispaly_memory_from_state(st, len);
    } else {
        dispaly_memory_from_the_file(st, len, addr);
    }
}

void write_units(FILE* output, char* buffer, int count, int unit_size) {
    fwrite(buffer, unit_size, count, output);
}

void get_save_into_file_data(size_t *s_addr, size_t *t_location, int *len){
    char input[128];

    printf("Please enter <source-address> <target-location> <length>\n");
    fgets(input, 128, stdin);
    sscanf(input, "%x %x %d", s_addr, t_location, len);
}

void save_into_file(state *st){
    int len = 0;
    size_t s_addr = 0, t_location = 0;
    char *write_buff;
    FILE *st_file, *hexeditplus_file;

    if(st->file_name == NULL){
        fprintf(stderr, "The file name is not set");
        return;
    }
    st_file = fopen(st->file_name, "r+");

    get_save_into_file_data(&s_addr, &t_location, &len);
    if(s_addr == 0){
        write_buff = (char *)st->mem_buf;
    } else {
        write_buff = (char*)s_addr;
        //write_buff = (char *)malloc(len * st->unit_size);

        /*hexeditplus_file = fopen(st->file_name, "r");
        if(hexeditplus_file == NULL){
            perror("Open file");
            return;
        }

        fseek(hexeditplus_file, s_addr-1, SEEK_SET);
        fread(write_buff, st->unit_size, len, hexeditplus_file);
        fclose(hexeditplus_file);*/

    }

    fseek(st_file, t_location, SEEK_SET);
    write_units(st_file, write_buff, len, st->unit_size);

    if(write_buff != (char *)st->mem_buf){
        free(write_buff);
    }
    fclose(st_file);
}

void get_memory_modify_data(size_t *addr, size_t *val){
    char input[128];

    printf("Please enter <location> <val>\n");
    fgets(input, 128, stdin);
    sscanf(input, "%x %x", addr, val);
}

void memory_modify(state *st){
    size_t addr, val;

    get_memory_modify_data(&addr, &val);

    if(addr > st->mem_count){
        printf("Invalid address");
        return;
    }
    printf("print: %x %x\n",addr, val);
    printf(unit_to_format(st->unit_size), val);
    printf("\n",addr, val);
    memcpy((char*)(st->mem_buf + addr), &val, st->unit_size);
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
                                {"Set Unit Size", set_unit_size}, {"Load Into Memory", load_into_memory},
                                {"toggle Display Mode", toggle_display_mode}, {"Memory Display", memory_display},
                                {"Save Into File", save_into_file}, {"Memory Modify", memory_modify},
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
 
/*
2. Entry point of the program is 08048166 which is in the _start, at the call of to deoo_
    that is a loop for printing A
    set the address to the stating poist of _start 08048140
    load memory of enrty point offset 18 unit size 4 replace it with 08048140 and save it into file

3. Main is in the .text section 080483b0 and the offset in file is 0003b0, 
    main address is 08048464 and size of 175 bytes
    the offset of main in the file is 08048464-080483b0 + 0003b0 = 0xb4 + 0003b0 = 000464
    b. the 3 start byte are 0x55 = push ebp, 0x89 0xe5 = mov ebp, esp
    c. load 1 byte into memory set the file to abc
        then modify it to be 0xc3 = ret
        set with save into file 0 000464 1

*/

int main(int argc, char **argv){
    menu();
    /*Elf32_Shdr *shdr = get_section_of_address("abc", 0x080483b0);
    if(shdr == NULL){
        printf("Return null\n");
        return 0;
    }
    printf("%x\n", shdr->sh_addr);*/
}