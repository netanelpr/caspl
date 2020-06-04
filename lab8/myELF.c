#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <elf.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>


#define DECIMAL_DISPLAY 0
#define HEX_DISPLAY 1

#define VM_S 0x08048000

int DEBUG_MODE = 0;

//+++++++++++ elf +++++++++++//

Elf32_Ehdr* get_elf_header(FILE *file){
    Elf32_Ehdr *header = (Elf32_Ehdr*)malloc(sizeof(Elf32_Ehdr));

    fseek(file, 0, SEEK_SET);
    fread((char *)header, sizeof(Elf32_Ehdr), 1, file);
    
    return header;
}

void print_header_encoding(char encoding){
    if(encoding == ELFDATANONE){
        printf("Data encoding : \t\tInvalid\n");
        return;
    }

    if(encoding == ELFDATA2LSB){
        printf("Data encoding : \t\t2's complement, little endian\n");
        return;
    }

    if(encoding == ELFDATA2MSB){
        printf("Data encoding : \t\t2's complement, big endian\n");
        return;
    }

}

void print_elf_header(Elf32_Ehdr *header){
    printf("Megic number: \t\t %c%c%c\n", header->e_ident[1], header->e_ident[2], header->e_ident[3]);
    print_header_encoding(header->e_ident[EI_DATA]);
    printf("Entry point: \t\t %x\n", header->e_entry);
    printf("Section table offset: \t\t %d\n", header->e_shoff);
    printf("Section table entries: \t\t %d\n", header->e_shnum);
    printf("Section table size: \t\t %d\n", header->e_shentsize);
    printf("Program header table offset: \t %d\n", header->e_phoff);
    printf("Program header entries: \t %d\n", header->e_phnum);
    printf("Program header size: \t\t %d\n", header->e_phentsize);

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

void print_section_t(Elf32_Shdr *header){
    uint32_t t = header->sh_type;
    const char *t_name[] = {"NULL", "PROGBITS", "SYMTAB", "STRTAB", "RELA", "HASH", "DYNAMIC",
                    "NOTE", "NOBITS", "REL", "SHLIB", "DYNSYM"};
    if(t > 11){
        printf("Add T");
        return;
    }

    printf("%s", t_name[t]);
}

void print_section_headers(Elf32_Shdr *header, uint32_t section_t_size, char *shst){
    uint32_t index = 0;

    for(; index < section_t_size; ++index){
        printf("[%u]\t",index);
        printf("%s ", (shst + header->sh_name));
        printf("%x ", header->sh_addr);
        printf("%x ", header->sh_offset);
        printf("%x ", header->sh_size);
        print_section_t(header);
        printf("\n");

        ++header;
    }

}

typedef struct{
    Elf32_Sym *symble_table;
    uint32_t size;
    char *strtab;
} Symble_table;


void fill_symble_table_struct(Symble_table *st, void *s_map_ptr, Elf32_Shdr *header, uint32_t section_t_size, char *shst){
    uint32_t index = 0;

    for(; index < section_t_size; ++index){
        if(header->sh_type == SHT_SYMTAB){
            st->symble_table = s_map_ptr + header->sh_offset;
            printf("%x %x\n",header->sh_size, sizeof(Elf32_Sym));
            st->size = header->sh_size / sizeof(Elf32_Sym);
            break;
        }
        ++header;
    }

    for(index = 0; index < section_t_size; ++index){
        if(header->sh_type == SHT_STRTAB){
            if(strcmp(".strtab", (shst + header->sh_name)) == 0){
                st->strtab = s_map_ptr + header->sh_offset;
                break;
            }
        }
        ++header;
    }
}

void print_symbol_table(Symble_table *st, Elf32_Shdr *header, char *shst){
    uint32_t index = 0;
    Elf32_Sym *symble_t = st->symble_table;
    Elf32_Shdr *s_header;

    for(; index < st->size; ++index){
        s_header = header + symble_t->st_shndx;

        printf("[%u]\t",index);
        printf("%x ", symble_t->st_value);
        printf("%d ", symble_t->st_shndx);
        if((symble_t->st_shndx == SHN_UNDEF) | (symble_t->st_shndx == SHN_ABS)){
            printf("N ");
        } else {
            printf("%s ", (shst + s_header->sh_name));
        }
        printf("%s ", (st->strtab + symble_t->st_name));
        printf("\n");

        ++symble_t;
    }   
}

//+++++++++++ hexeditplus +++++++++++//

void debug_string_print(char debug_mode, char *str){
    if(debug_mode != 0){
    fprintf(stderr, "DEBUG: %s\n", str);
    }
}

typedef struct fun_desc {
  char *name;
  void (*fun)();
} fun_desc;

typedef struct {
    void *map_ptr;
    long size;
    int fd;
} MapData;

void map_file_read_private(MapData* map_d, char *file_name){
    void *map_ptr = NULL;
    struct stat sb;
    int fd;

    fd = open(file_name, O_RDONLY);
    if(fd == -1){
        map_d->map_ptr = NULL;
        return;
    }

    if (fstat(fd, &sb) == -1){
        map_d->map_ptr = NULL;
        close(fd);
        return;
    }

    map_ptr = mmap(0, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map_ptr == MAP_FAILED){
        map_d->map_ptr = NULL;
        close(fd);
        return;
    }

    fprintf(stderr,"%p\n", map_ptr);
    map_d->map_ptr = map_ptr;
    map_d->size = sb.st_size;
    map_d->fd = fd;

}


void toggle_debug_mode(){
    if(DEBUG_MODE == 0){
        DEBUG_MODE = 1;
        printf("Debug flag now on\n");
    } else {
        DEBUG_MODE = 0;
        printf("Debug flag now off\n"); 
    }
}


void examine_elf_file(){
    Elf32_Ehdr *header;
    char file_name[128];
    int fd;

    fgets(file_name, 128, stdin);
    sscanf(file_name, "%s\n", file_name);

    fd = open(file_name, O_RDONLY);
    if(fd == -1){
        perror("Open file");
        return;
    }

    if(DEBUG_MODE != 0){
        fprintf(stderr, "DEBUG: Set file name to %d\n", DEBUG_MODE);
    }

    header = mmap(0, sizeof(Elf32_Addr), PROT_READ, MAP_PRIVATE, fd, 0);
    if (header == MAP_FAILED){
        close(fd);
        perror("mmap");
        return;
    }

    print_elf_header(header);

    munmap(header, sizeof(Elf32_Addr));
    close(fd);

    return;
}

void print_section_names(){
    void *file_map = NULL;
    Elf32_Ehdr *header;
    Elf32_Shdr *section_header, *section_shst_enrty;
    MapData map_d;
    char *shst = NULL;
    char file_name[128];

    fgets(file_name, 128, stdin);
    sscanf(file_name, "%s\n", file_name);

    map_file_read_private(&map_d, file_name);
    if(map_d.map_ptr == NULL){
        perror("Mmap");
        return;
    }

    file_map = map_d.map_ptr;
    header = file_map;

    section_header = file_map + header->e_shoff;
    section_shst_enrty = section_header + header->e_shstrndx;
    shst = file_map + section_shst_enrty->sh_offset;

    print_section_headers(section_header, header->e_shnum, shst);

    munmap(file_map, map_d.size);
    close(map_d.fd);

    return;
} 

void print_symbols(){
    void *file_map = NULL;
    Elf32_Ehdr *header;
    Elf32_Shdr *section_header, *section_shst_enrty;
    Symble_table symbol_table;
    MapData map_d;
    char *shst = NULL;
    char file_name[128];

    fgets(file_name, 128, stdin);
    sscanf(file_name, "%s\n", file_name);

    map_file_read_private(&map_d, file_name);
    if(map_d.map_ptr == NULL){
        perror("Mmap");
        return;
    }

    file_map = map_d.map_ptr;
    header = file_map;

    section_header = file_map + header->e_shoff;
    section_shst_enrty = section_header + header->e_shstrndx;
    shst = file_map + section_shst_enrty->sh_offset;

    fill_symble_table_struct(&symbol_table, file_map, section_header, header->e_shnum, shst);

    print_symbol_table(&symbol_table, section_header, shst);

    munmap(file_map, map_d.size);
    close(map_d.fd);

    return;
}

void quit(){
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
    fun_desc function_desc[] = {{"Toggle Debug Mode", toggle_debug_mode}, {"Examine ELF File", examine_elf_file},
                                {"Print Section Names", print_section_names}, {"Print Symbols", print_symbols},
                                {"Quit", quit}, {NULL, NULL}};
    
    int menu_size = (int)(sizeof(function_desc)/sizeof(fun_desc) - 1);
    char option_input[menu_size+1];

    while(1){
        display_menu(function_desc, menu_size);
        printf("Option:");
        fgets(option_input, menu_size, stdin);
        sscanf(option_input, "%d\n", &option);
        if((option > -1) & (option < menu_size)){
            function_desc[option].fun();
        }
    }
}

int main(int argc, char **argv){
    menu();
    /*Elf32_Shdr *shdr = get_section_of_address("abc", 0x080483b0);
    if(shdr == NULL){
        printf("Return null\n");
        return 0;
    }
    printf("%x\n", shdr->sh_addr);*/
}