#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
  pid_t pid = fork();

  if (pid == -1) {
    // Error al hacer el fork
    perror("Error al hacer el fork");
    exit(EXIT_FAILURE);
  } else if (pid == 0) {
    // Proceso hijo
    char arg1[10], arg2[2];
    sprintf(arg1, "%d", 20);
    sprintf(arg2, "%c", 'b');
    execl("./ipc", "ipc", arg1, arg2, NULL);
    perror("Error al ejecutar el programa ipc en el proceso hijo");
    exit(EXIT_FAILURE);
  } else {
    // Proceso padre
    char arg1[10], arg2[2];
    sprintf(arg1, "%d", 30);
    sprintf(arg2, "%c", 'c');
    execl("./ipc", "ipc", arg1, arg2, NULL);
    perror("Error al ejecutar el programa ipc en el proceso padre");
    exit(EXIT_FAILURE);
  }

  // Esperar a que los procesos hijo y padre terminen
  int status;
  wait(&status);
  wait(&status);

  return 0;
}
