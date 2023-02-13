#include <stdio.h>
#include <unistd.h>

int main()
{
    int pid = fork();

    if (pid == 0)
    {
        for (int i = 1; i <= 5000000; i++)
        {
            printf("%d\n", i);
        }
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