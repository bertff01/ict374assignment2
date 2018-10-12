/* 
 *  name;		memory.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

extern char **environ;

int global_x = 10;  				// initialised global variable
int global_y;       				// un-initialised global variable
char global_array1[] = "Hello, world!"; 	// initialised global array 
char global_array2[10];  			// un-initialised global array
char *global_pointer1 = "bye!";  		// global pointer to a string literal 
char *global_pointer2;  			// un-initialised global pointer 
float global_float = 100.1;			// initialised global variable
double global_double;				// un-initialised global variable

int f2(int x)
{
    char * f2_p;
    int f2_x = 21;

    f2_p = malloc(1000);         // dynamically allocated memory
 
    // print out the address of x
    printf("address of x %p\n",&x);
    // print out the addresses of f2_p, and f2_x
    printf("address of f2p %p\n",&f2_p);
    printf("address of f2_x %p\n",&f2_x);
    // print out the starting address of the dynamically allocated memory
    printf("address of dynamically allocated memory %p\n",f2_p);
   
    f2_x = 10;
    return f2_x;
}

void f1(int x1, int x2, float x3, char x4, double x5, int x6)
{
    int f1_x = 10;
    int f1_y;
    char *f1_p1 = "This is inside f1";   // pointer to another string literal 
    char *f1_p2;

    f1_p2 = malloc(100);         // dynamically allocated memory

    // print out the addresses of x1, x2, x3, x4, x5, x6i
    printf("x1 %p x2 %p x3 %p x4 %p x5 %p x6 %p\n",&x1,&x2,&x3,&x4,&x5,&x6);
    // print out the addresses of f1_x, f1_y, f1_p1, f1_p2
    printf("f1_x %p f1_y %p f1_p1 %p f1_p2 %p",&f1_x,&f1_y,&f1_p1,&f1_p2);
    // print out the address of the string literal "This is inside f1"
    printf("string literal f1_p1 %p",f1_p1);
    printf("allocated memory f1_p2 %p",f1_p2);

    f1_y = f2(10);
    return;
}

int main(int argc, char *argv[])
{
     
    printf("My OS bit size: %lu\n", sizeof(void *) * 8);

    // print out the addresses of argc, argv
    printf("argc %p argv %p\n",&argc,&argv);
    // print out the starting address and end address of the command line arguments of this process
    
    printf("argv[0] %p end of argv %p\n",argv[0],&argv[argc-1][strlen(argv[argc-1])]);
    // print out the starting address and end address of the environment of this process
    // print out the starting addresses of function main, f1, and f2
    // print out the addresses of global_x, global_y, global_array1, global_array2, global_pointer1,
    //           global_pointer2, global_float, global_double
    // print out the addresses of string literals "Hello, world!" and "bye" 


    // call function f1 with suitable arguments such as 12, -5, 33.7, 'A', 1.896e-10, 100 
    f1( 12,-5,22.1,'A',1.896e-10,100 );

    exit(0);
}


