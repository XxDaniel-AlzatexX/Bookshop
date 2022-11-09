#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include "BaseDatos.h"

int main(int argc, char** argv) {

  if (fork() == 0) {
    printf("Proceso auxiliar inciado. Listo para recibir comandos...\n");
    // hilo2 que recibe comandos s (salir) y r (reporte)
    exit(0);
  }
  else {
    printf("Proceso inciado: %d args\n", argc);
    int fd, fd1, pid, n, bytes;
    mode_t fifo_mode = S_IRUSR | S_IWUSR;
    unlink(argv[2]);
    if (mkfifo (argv[2], fifo_mode) == -1) {
       perror("mkfifo");
    }
    fd = open(argv[2], O_RDONLY);
    printf("Abrio el pipe llamado: %s\n", argv[2]);

    do {
      char in[100];
      bytes = read(fd, in,sizeof(char[100]));  // Cada request sera una string de 100 caracteres que quedara con el formato: "NombrePipeRespuesta.Tipo.Nombre.ISBN.NumeroEjemplar"
                                               //Si es un request de tipo P (préstamo) no se tiene el número de ejemplar.. de resto si
      if (bytes == -1) {
        perror("proceso lector:");
        exit(1);
      }
      else if (bytes > 0) {
        char nombre[50];
        char pipePs[10];
        char tipo[2];
        int isbn, numEjemplar;
        printf("Mensaje recibido: %s\n", in);
        char delim[]= ",";
        char *ptr = strtok(in,delim);
        sprintf(pipePs, ptr);
        if(ptr != NULL){
          ptr = strtok(NULL, delim);
          sprintf(tipo, ptr);
        }
        if(ptr != NULL){
          ptr = strtok(NULL, delim);
          sprintf(nombre, ptr);
        }
        if(ptr != NULL){
          ptr = strtok(NULL, delim);
          isbn = atoi(ptr);
        }
        if(ptr != NULL){
          ptr = strtok(NULL, delim);
          numEjemplar = atoi(ptr);
        } else{numEjemplar = -1;}s
        //Esto siguiente se hace usualmente en el hilo auxioliar 1 (dependiendo del tipo de request)
        do{
          printf("Intentando abrir el pipe llamado: %s\n", pipePs);
          fd1 = open(pipePs, O_WRONLY);
          sleep(1);
        }while(fd1 == -1);
        char resp[100] = "Request recibido...";                  //Formato: Si es de tipo R: "R.A/D.fecha"   Si es de tipo D:"D.A/D"  Si es de tipo P: "P.A/D.fecha" (A/D -> aceptado/denegado)  

        if (write(fd1, resp, sizeof(resp)) == -1) {
          printf("Error al escribir en la pipe");
        }
        printf("Se escribió en el pipe de respuesta\n");
      }
      wait(1);
    } while (1);
    close(fd);

    unlink(argv[2]);
  }

  exit(0);
}