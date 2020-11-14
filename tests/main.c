#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char ** argv)
{
    printf("Mac Change Time\n");

    do
    {
        printf("AA:BB:CC:DD:EE:FF\n");
        printf("Mac Changed\n");
        printf("00:11:22:33:44:55\n");
    
        printf("Exit? (y/n) ");
        fflush(stdin);
    }
    while(getc(stdin) != 'y');
    
}