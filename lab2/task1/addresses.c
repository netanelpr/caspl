#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

/*
The stack ffba8000-ffbc9000
The heap 57529000-5754b000
*/

int addr5;
int addr6;

int foo();
void point_at(void *p);
void foo1();
void foo2();

int main (int argc, char** argv){
    printf("%d",getpid());
    int addr2;
    int addr3;
    char* yos="ree";
    int * addr4 = (int*)(malloc(50));
    printf("- &addr2: %p\n",&addr2);
    printf("- &addr3: %p\n",&addr3);
    printf("- foo: %p\n",foo);
    printf("- &addr5: %p\n",&addr5);
    
	point_at(&addr5);
	
    printf("- &addr6: %p\n",&addr6);
    printf("- yos: %p\n",yos);
    printf("- addr4: %p\n",addr4);
    printf("- &addr4: %p\n",&addr4);
    
    printf("- &foo1: %p\n" ,&foo1);
    printf("- &foo1: %p\n" ,&foo2);
    printf("- &foo2 - &foo1: %ld\n" ,&foo2 - &foo1);

    foo1();
    foo2();

    fgetc(stdin);
    return 0;
}

int foo(){
    return -1;
}

void point_at(void *p){
    int local;
	static int addr0 = 2;
    static int addr1;


    long dist1 = (size_t)&addr6 - (size_t)p;
    long dist2 = (size_t)&local - (size_t)p;
    long dist3 = (size_t)&foo - (size_t)p;
    
    printf("dist of global var to static, both in the data section.\ndist1: (size_t)&addr6 - (size_t)p: %ld\n",dist1);
    printf("dist of local in the stack to the global var.\ndist2: (size_t)&local - (size_t)p: %ld\n",dist2);
    printf("dist of the code section to the data section.\ndist3: (size_t)&foo - (size_t)p:  %ld\n",dist3);
	
	printf("- addr0: %p\n", & addr0);
    printf("- addr1: %p\n",&addr1);
}

void foo1 (){   
    /*
    All array in the stack if there size is small
    array are *arr
    The + operator adds sizeof()
    */ 
    int iarray[3];
    float farray[3];
    double darray[3];
    char carray[3];

    printf("int array: [0]%x, [1]%x\n", (unsigned int)iarray, (unsigned int)(iarray+1));
    printf("float array: [0]%x, [1]%x\n", (unsigned int)farray, (unsigned int)(farray+1));
    printf("double array: [0]%x, [1]%x\n", (unsigned int)darray, (unsigned int)(darray+1));
    printf("char array: [0]%x, [1]%x\n", (unsigned int)carray, (unsigned int)(carray+1));
}

void foo2 (){    
    int iarray[] = {1,2,3};
    char carray[] = {'a','b','c'};
    int* iarrayPtr = iarray;
    char* carrayPtr = carray; 
    int *p;

    printf("int*: [0]%x, [1]%x\n", (unsigned int)iarrayPtr, (unsigned int)iarrayPtr+sizeof(int));
    printf("char*: [0]%x, [1]%x\n", (unsigned int)carrayPtr, (unsigned int)carrayPtr+sizeof(carrayPtr));
    printf("int* in the stack uninitialze: [0]%x, [1]%x, data:%d\n", (unsigned int)p, (unsigned int)p+sizeof(p), *p);
}
