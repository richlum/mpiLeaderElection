#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXSIZE 256

int main(int argc, char* argv){


char buf [MAXSIZE];
int total = 0;

while (gets(buf)){
    total+= atoi(buf);
}

printf("Total = %d\n" ,total);

}    
