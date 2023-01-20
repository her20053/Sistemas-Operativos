#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char* argv[]){

    int estFile = open(argv[1], O_RDONLY);

    if(estFile == -1){
        perror("Error al abrir el archivo\n");
        // printf("Error al abrir el archivo\n");
        return 1;
    }

    int estFileDestino = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if(estFileDestino == -1){
        // printf("Error al abrir el archivo de destino\n");
        perror("Error al abrir el archivo de destino\n");
        return 1;
    }

    char buffer[1024];

    ssize_t n;

    while((n = read(estFile, buffer, 1024)) > 0){
        if(write(estFileDestino, buffer, n) != n){
            perror("Error al escribir el archivo\n");
            close(estFile);
            close(estFileDestino);
            return 1;
            // printf("Error al escribir el archivo");
        }
    }

    if(n == -1){
        perror("Error al leer el archivo de origen\n");
        close(estFile);
        close(estFileDestino);
        return 1;
    }

    close(estFile);
    close(estFileDestino);


    return 0;

}