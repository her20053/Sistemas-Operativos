#include <stdio.h>
#include <time.h>

int main() {
   clock_t start, end;
   int i, j, k;
   double elapsed_time;

   start = clock();

    for (i = 0; i < 1000000; i++) {
      printf("%d\n", i);
    }
    for (j = 0; j < 1000000; j++) {
      printf("%d\n", j);
    }
    for (k = 0; k < 1000000; k++) {
      printf("%d\n", k);  
    }

   end = clock();

   elapsed_time = (double)(end - start) / CLOCKS_PER_SEC;

   printf("El tiempo de ejecución es: %lf segundos\n", elapsed_time);

   return 0;
}