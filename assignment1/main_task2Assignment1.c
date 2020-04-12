#include <stdio.h>
#define	MAX_LEN 34			/* maximal input string size */
					/* enough to get 32-bit string + '\n' + null terminator */
extern int convertor(char* buf);

int main(int argc, char** argv)
{
  char buf[MAX_LEN ];
 
  fgets(buf, MAX_LEN, stdin);		/* get user input string */ 
  convertor(buf);			/* call your assembly function */

  return 0;
}