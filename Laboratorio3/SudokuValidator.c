#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#define FILE_SIZE 81
int sudoku[9][9];

// Función para verificar si todos los números del 1 al 9 están en una columna
int validarColumna(int matriz[9][9], int columna) {
    int numeros[9] = {0}; // Arreglo para almacenar los números del 1 al 9
    int i;
    for (i = 0; i < 9; i++) {
        int num = matriz[i][columna];
        if (num < 1 || num > 9 || numeros[num-1] == 1) {
            // El número no está en el rango del 1 al 9 o ya se encontró anteriormente
            return 0;
        }
        numeros[num-1] = 1; // Se marca el número como encontrado
    }
    return 1; // Todos los números del 1 al 9 están en la columna
}

// Función para verificar si todos los números del 1 al 9 están en una fila
int validarFila(int matriz[9][9], int fila) {
    int numeros[9] = {0}; // Arreglo para almacenar los números del 1 al 9
    int i;
    for (i = 0; i < 9; i++) {
        int num = matriz[fila][i];
        if (num < 1 || num > 9 || numeros[num-1] == 1) {
            // El número no está en el rango del 1 al 9 o ya se encontró anteriormente
            return 0;
        }
        numeros[num-1] = 1; // Se marca el número como encontrado
    }
    return 1; // Todos los números del 1 al 9 están en la fila
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

    // Bandera para indicar si el Sudoku es válido
    int valido = 1; 

    for (i = 0; i < 9; i++) {
        // Se valida cada fila
        if (!validarFila(sudoku, i)) { 
            valido = 0;
            break;
        }
        // Se valida cada columna
        if (!validarColumna(sudoku, i)) { 
            valido = 0;
            break;
        }
    }
    if (valido) {
        // Si se validaron las filas y columnas, se validan los subarreglos de 3x3
        // Se validan los subarreglos de 3x3
        for (i = 0; i < 9; i+=3) {
            for (j = 0; j < 9; j+=3) {
                if (!validarSubarreglo(sudoku, i, j)) {
                    valido = 0;
                    break;
                }
            }
            if (!valido) {
                break;
            }
        }
    }
    

    // Para este punto, la bandera "valido" indica si el Sudoku es válido o no
    if (valido) {
        printf("El Sudoku es valido.\n");
    } else {
        printf("El Sudoku no es valido.\n");
    }

    // Después de utilizar el archivo, lo desmapeamos de memoria y lo cerramos
    munmap(file_data, FILE_SIZE);
    close(fd);

    // Finalmente, regresamos 0 para indicar que el programa terminó correctamente
    return 0;
}