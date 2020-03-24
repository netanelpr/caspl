/* $Id: count-words.c 858 2010-02-21 10:26:22Z tolpin $ */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

/* return string "word" if the count is 1 or "words" otherwise */
char *words(int count) {
  char *words1 = "Hello";
  printf("%p\n",words1);
  char *words = (char*)malloc(6*sizeof(char));
  strcpy(words, "words");
  if(count==1)
    words[strlen(words)-1] = '\0';
  return words;
}

/* print a message reportint the number of words */
int print_word_count(char **argv) {
  int count = 0;
  char **a = argv;
  while(*(a++))
    ++count;
  printf("The sentence contains %d %s.\n", count, words(count));
  fgetc(stdin);
  return count;
}

/* print the number of words in the command line and return the number as the exit code */
int main(int argc, char **argv) {
  printf("%d\n",getpid());
  return print_word_count(argv+1);
}

   
