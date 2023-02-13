#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
   clock_t start, end;
   pid_t pid1, pid2, pid3;
   int i;
   double elapsed_time;

   start = clock();

   pid1 = fork();

   if (pid1 == 0) {
      pid2 = fork();
      if (pid2 == 0) {
         pid3 = fork();
         if (pid3 == 0) {
            for (i = 0; i < 1000000; i++) {
               printf("%d\n", i);
            }
         } else {
            for (i = 0; i < 1000000; i++) {
               printf("%d\n", i);
            }
            wait(NULL);
         }
      } else {
         for (i = 0; i < 1000000; i++) {
            printf("%d\n", i);
         }
         wait(NULL);
      }
   } 
   else {
      for (i = 0; i < 1000000; i++) {
         printf("%d\n", i);
      }
      wait(NULL);

      end = clock();

    elapsed_time = (double)(end - start) / CLOCKS_PER_SEC;

    printf("El tiempo de ejecución es: %lf segundos\n", elapsed_time);

    return 0;
   }

   
}
