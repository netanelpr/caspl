#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Gets a char c and returns its encrypted form by adding 3 to its value. 
If c is not between 0x20 and 0x7E it is returned unchanged */
char encrypt(char c){
    if((c > 0x1f) & (c <= 0x7E)){
        c = c + 3;
    }
    return c;
}

/* Gets a char c and returns its decrypted form by reducing 3 to its value. 
If c is not between 0x20 and 0x7E it is returned unchanged */
char decrypt(char c){
    if((c > 0x1f) & (c <= 0x7E)){
        c = c - 3;
    }
    return c;
}

/* dprt prints the value of c in a decimal representation followed by a 
new line, and returns c unchanged. */
char dprt(char c){
    printf("%d\n", c);
    return c;
}

/* If c is a number between 0x20 and 0x7E, cprt prints the character of ASCII value c followed 
by a new line. Otherwise, cprt prints the dot ('.') character. After printing, cprt returns 
the value of c unchanged. */
char cprt(char c){
    if((c > 0x1f) & (c <= 0x7E)){
        printf("%c\n", c);
        return c;
    }

    printf(".\n");
    return c;
}

/* Ignores c, reads and returns a character from stdin using fgetc. */
char my_get(char c){
    return fgetc(stdin);
}

/* Gets a char c,  and if the char is 'q' , ends the program with exit code 0. Otherwise returns c. */
char quit(char c){
    if(c == 'q'){
        exit(0);
    }

    return c;
}

char censor(char c) {
  if(c == '!')
    return '.';
  else
    return c;
}
 
char* map(char *array, int array_length, char (*f) (char)){
  char* mapped_array = (char*)(malloc(array_length*sizeof(char)));
  
  for(int i=0; i < array_length; i=i+1){
      mapped_array[i] = (*f)(array[i]);
  }
  
  return mapped_array;
}
 
int main(int argc, char **argv){
  /*char arr[] = {'a','b', '!'};
  char * menu_map = map(arr, 3, censor);
    for(int i=0; i < 3; i=i+1){
      printf("%c\n",menu_map[i]);
  }*/

    int base_len = 5;
    char arr1[base_len];
    char* arr2 = map(arr1, base_len, my_get);
    char* arr3 = map(arr2, base_len, encrypt);
    char* arr4 = map(arr3, base_len, dprt);
    char* arr5 = map(arr4, base_len, decrypt);
    char* arr6 = map(arr5, base_len, cprt);
    free(arr2);
    free(arr3);
    free(arr4);
    free(arr5);
    free(arr6);
}