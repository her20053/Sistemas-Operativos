#include <stdio.h>
#include <unistd.h>

int main(){

    int f = fork();

    if(f == 0){
        execl("ej1A", (char*)NULL);
    }
    else{
        printf("Desde inciso B: %d\n", (int)getpid());
        execl("ej1A", (char*)NULL);
    }


     return (0);
}