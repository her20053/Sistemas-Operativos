#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <omp.h>
#include <stdbool.h>


#define FILE_SIZE 81
#define THREAD_COUNT 3

// Declaración del arreglo bidimensional de 9x9

int sudoku[9][9];

// Declaracion de los trheads

pthread_t threads[THREAD_COUNT];

// Bandera para indicar si el Sudoku es válido

int sudoku_valido = 1; 

// Función para verificar si todos los números del 1 al 9 están en una columna
int validarColumna() {

    int resultado = 1; // Inicializamos el resultado a 1
    
    int i;
    int columna;

    omp_set_num_threads(9);
    omp_set_nested(true);

    #pragma omp parallel for private(columna,i) schedule(dynamic)	 
    for (columna = 0; columna < 9; columna++){
        
        int numeros[9] = {0}; // Arreglo para almacenar los números del 1 al 9
        int tid = omp_get_thread_num(); // Identificador del hilo
        printf(" ~ Revisando la columna %d el thread es: %ld\n", columna, (syscall(SYS_gettid) + tid));

        
        for (i = 0; i < 9; i++) {
            int num = sudoku[i][columna];
            if (num < 1 || num > 9 || numeros[num-1] == 1) {
                // El número no está en el rango del 1 al 9 o ya se encontró anteriormente
                resultado = 0;
            }
            numeros[num-1] = 1; // Se marca el número como encontrado
        }
         
    }
    
    // Devolvemos el resultado
    return resultado;
    
}


void *verificarColumnaPorThread(void *arg){

    printf("\n ! En la revision de columnas el siguiente es un thread en ejecucion: %ld\n", syscall(SYS_gettid)); 
    sudoku_valido = validarColumna();
    pthread_exit(0);

}

// Función para verificar si todos los números del 1 al 9 están en una fila
int validarFila() {

    for (int fila = 0; fila < 9; fila++){

        int numeros[9] = {0}; // Arreglo para almacenar los números del 1 al 9
        int i;
        for (i = 0; i < 9; i++) {
            int num = sudoku[fila][i];
            if (num < 1 || num > 9 || numeros[num-1] == 1) {
                // El número no está en el rango del 1 al 9 o ya se encontró anteriormente
                return 0;
            }
            numeros[num-1] = 1; // Se marca el número como encontrado
        }

    }

    // Todos los números del 1 al 9 están en la fila
    return 1; 
}

// Función para verificar si todos los números del 1 al 9 están en un subarreglo de 3x3
int validarSubarreglo(int matriz[9][9], int filaInicio, int columnaInicio) {
    int numeros[9] = {0}; // Arreglo para almacenar los números del 1 al 9
    int i, j;
    for (i = filaInicio; i < filaInicio+3; i++) {
        for (j = columnaInicio; j < columnaInicio+3; j++) {
            int num = matriz[i][j];
            if (num < 1 || num > 9 || numeros[num-1] == 1) {
                // El número no está en el rango del 1 al 9 o ya se encontró anteriormente
                return 0;
            }
            numeros[num-1] = 1; // Se marca el número como encontrado
        }
    }
    return 1; // Todos los números del 1 al 9 están en el subarreglo de 3x3
}

int main(int argc, char **argv) {

    omp_set_num_threads(1); // Se establece el numero de threads a 1

    int fd;
    char *file_data;
    int i, j, k;

    // Primero tenemos que revisar si los argumentos son correctos
    if (argc < 2) {
        // Se utilizo un argumento incorrecto
        fprintf(stderr, "Porfavor utiliza: %s <archivo>\n", argv[0]);
        exit(1);
    }

    // Ahora abrimos el archivo para lectura utilizando la función open
    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        // Hubo un error al abrir el archivo
        perror("open");
        exit(1);
    }

    // Una vez leido el archivo, lo mapeamos a memoria utilizando la función mmap
    file_data = mmap(NULL, FILE_SIZE, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_data == MAP_FAILED) {
        // Hubo un error al mapear el archivo a memoria
        perror("mmap");
        exit(1);
    }

    // Copiamos los datos mappeados a la matriz sudoku (Matriz de 9x9 GLOBAL)
    k = 0;
    for (i = 0; i < 9; i++) {
        for (j = 0; j < 9; j++) {
            sudoku[i][j] = file_data[k] - '0';
            k++;
        }
    }

    // Después de utilizar el archivo, lo desmapeamos de memoria y lo cerramos
    munmap(file_data, FILE_SIZE);
    close(fd);



    // ---------------------------- Empieza creacion del hijo
    
    // Obtenemos el process ID del padre
    pid_t pid;
    pid = getpid();

    // Creamos el proceso hijo
    pid_t pid_hijo;
    pid_hijo = fork();

    if (pid_hijo == 0) {
        // Proceso hijo
        printf(" + Soy el proceso hijo con PID: %d\n", getpid());

        // Obtenemos el numero del proceso padre y lo casteamos a string para meterlo como argumento
        char *arg = malloc(10);
        sprintf(arg, "%d", pid);

        // Ejecutamos el programa "ps" con el argumento del PID del padre
        execlp("ps", "ps", "-p", arg, "-lLf",NULL);
        // Si llegamos a este punto, hubo un error al ejecutar el programa
        perror("execlp");
        exit(1);


    } else {

        // Proceso padre
        
        printf(" + Soy el proceso padre (Esperando a mi primer hijo) con PID: %d\n", getpid());
    
        // Creamos un pthread que realiza la revision de columnas

        if(pthread_create(&threads[0], NULL, verificarColumnaPorThread, NULL) != 0) {
            perror("pthread_create");
            exit(1);
        }

        if (pthread_join(threads[0], NULL) != 0) {
            perror("pthread_join");
            exit(1);
        }

        // Mostramos el thread ID del thread que revisa las columnas
        printf("\n > El thread ID del thread que revisa las columnas es: %ld\n", syscall(SYS_gettid));

        // Ahora tenemos que esperar a que el proceso hijo termine
        int status;
        waitpid(pid_hijo, &status, 0);

        // Si el proceso hijo terminó correctamente, imprimimos el mensaje
        if (WIFEXITED(status)) {
            printf("\n + El proceso hijo terminó correctamente.\n");
        } else {
            printf("\n - El proceso hijo terminó con errores.\n");
        }

        // Validamos las filas
        sudoku_valido = validarFila();


        // Creamos otro hijo para volver a realizar el el execlp

        pid_t pid_hijo2;
        pid_hijo2 = fork();

        if (pid_hijo2 == 0) {
            // Proceso hijo
            printf(" + Soy el proceso hijo con PID: %d\n", getpid());

            // Obtenemos el numero del proceso padre y lo casteamos a string para meterlo como argumento
            char *arg = malloc(10);
            sprintf(arg, "%d", pid);

            // Ejecutamos el programa "ps" con el argumento del PID del padre
            execlp("ps", "ps", "-p", arg, "-lLf",NULL);
            // Si llegamos a este punto, hubo un error al ejecutar el programa
            perror("execlp");
            exit(1);
        }

        else {
            // Proceso padre
            printf(" + Soy el proceso padre (Esperando a mi segundo hijo) con PID: %d\n", getpid());
            // Ahora tenemos que esperar a que el proceso hijo termine
            int status;
            waitpid(pid_hijo2, &status, 0);

            // Si el proceso hijo terminó correctamente, imprimimos el mensaje
            if (WIFEXITED(status)) {
                printf("\n + El proceso hijo terminó correctamente.\n");
            } else {
                printf("\n - El proceso hijo terminó con errores.\n");
            }
        }

    }
    // Finalmente, regresamos 0 para indicar que el programa terminó correctamente
    return 0;
}