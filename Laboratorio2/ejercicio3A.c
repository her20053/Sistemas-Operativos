#include <stdio.h>
#include <unistd.h>

int main()
{
    int pid = fork();

    if (pid == 0)
    {
        printf("Soy el proceso hijo.\n");
    }
    else if (pid > 0)
    {
        while (1)
        {
            // Ciclo infinito del padre
        }
    }

    return 0;
}