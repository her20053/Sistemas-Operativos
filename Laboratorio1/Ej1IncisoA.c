#include <stdio.h>
#include <unistd.h>

int main(){

    printf("Hola mundo!\n");
    printf("Desde inciso A: %d\n", (int)getpid());
    return (0);

}