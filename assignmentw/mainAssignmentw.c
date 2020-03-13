#include <stdio.h>
#define MAX_LEN 100			/* maximal input string size */

extern int do_Str(char*);

int main(int argc, char** argv) {

  char str_buf[MAX_LEN];   
  int counter = 0;
  
  fgets(str_buf, MAX_LEN, stdin);	/* get user input string */ 
  counter = do_Str (str_buf);		/* call your assembly function */
  printf("%s%d",str_buf,counter);	/* print result string and counter */
  
  return 0;
}