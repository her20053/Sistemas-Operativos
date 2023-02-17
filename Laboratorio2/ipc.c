#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>

size_t mem_size = sizeof(char) * 100;
const char* mem_name = "my_shared_memory";

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Uso: %s n x\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int n = atoi(argv[1]);
    char x = argv[2][0];

    // Crear un pipe anónimo para la comunicación entre los procesos padre e hijo
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Crear el espacio de memoria compartida
    const size_t size = sizeof(char) * n;
    int shm_fd = shm_open("/ipc_shm", O_CREAT | O_RDWR, 0666);
    int mem_fd = shm_open(mem_name, O_RDWR, 0);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    if (ftruncate(shm_fd, size) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    // Obtener el puntero al espacio de memoria compartida
    char* ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // Comunicarse con la otra instancia del programa
    int other_shm_fd = -1;
    if (ptr[0] != '\0') {
        printf("Se encontró una instancia previa del programa\n");

        // Obtener el file descriptor del espacio de memoria compartida de la otra instancia
        other_shm_fd = atoi(ptr);
        if (close(shm_fd) == -1) {
            perror("close");
        }

        // Leer los datos del espacio de memoria compartida de la otra instancia
        ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, other_shm_fd, 0);
        if (ptr == MAP_FAILED) {
            perror("mmap");
            exit(EXIT_FAILURE);
        }

        // Esperar a que la otra instancia termine de escribir
        if (wait(NULL) == -1) {
            perror("wait");
            exit(EXIT_FAILURE);
        }
    } else {
        printf("Creando espacio de memoria compartida\n");
        sprintf(ptr, "%d", shm_fd);
    }

    // Crear el proceso hijo
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Proceso hijo: esperar a recibir la señal de notificación del proceso padre
        close(pipefd[1]);
        int iteration = 0;
        while (iteration < n) {
            char c;
            if (read(pipefd[0], &c, sizeof(char)) == -1) {
                perror("read");
                exit(EXIT_FAILURE);
            }
            if (c == x) {
                ptr[iteration] = c;
                iteration++;
            }
        }
        if (close(pipefd[0]) == -1) {
            perror("close");
        }
        exit(EXIT_SUCCESS);
    } else {
        // Proceso padre: enviar señales de notificación al proceso hijo
        close(pipefd[0]);
        for (int i = 0; i < n; i++) {
            if (i % n == 0) {
                printf("Proceso padre: iteración divisible entre %d, enviando señal a proceso hijo\n", n);
                if (write(pipefd[1], x, 2) == -1) {
                    perror("write");
                    exit(EXIT_FAILURE);
                }
            }
        }
        close(pipefd[1]);

        // Esperar al proceso hijo
        int status;
        if (wait(&status) == -1) {
            perror("wait");
            exit(EXIT_FAILURE);
        }

        // Desplegar contenido de la memoria compartida
        char* mem_ptr = (char*) mmap(NULL, mem_size, PROT_READ, MAP_SHARED, mem_fd, 0);
        if (mem_ptr == MAP_FAILED) {
            perror("mmap");
            exit(EXIT_FAILURE);
        }
        printf("Contenido de la memoria compartida:\n%s\n", mem_ptr);

        // Liberar recursos
        if (munmap(mem_ptr, mem_size) == -1) {
            perror("munmap");
            exit(EXIT_FAILURE);
        }
        close(mem_fd);
        if (unlink(mem_name) == -1) {
            perror("unlink");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
               
