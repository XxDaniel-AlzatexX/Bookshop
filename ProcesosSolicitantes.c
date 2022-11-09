#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "BaseDatos.h"

void enviarRequest(int fd, char pipe[10], char tipo, char nombre[30], int isbn,
                   int numEjemplar);
char *crearPipeResp();
int escucharRespuesta(char pipePs[10]);

int main(int argc, char *argv[]) {
  printf("Proceso iniciado\n");

  int indicePipe = 2;
  int indiceFile = 0;
  if (argc > 3)  // Si tiene un file para leer
  {
    if (strcmp(argv[1], "-f") == 0) {
      indiceFile = 2;
    } else if (strcmp(argv[1], "-p") == 0) {
      indicePipe = 2;
    }
    if (strcmp(argv[3], "-f") == 0) {
      indiceFile = 4;
    } else if (strcmp(argv[3], "-p") == 0) {
      indicePipe = 4;
    }
  } else if (argc == 3)  // No tiene un file para leer
  {
    indicePipe = 2;
  }

  int fd, fd2;
  mode_t fifo_mode = S_IRUSR | S_IWUSR;
  fd = -1;
  do {
    printf("Intentando abrir el pipe llamado: %s\n", argv[indicePipe]);
    fd = open(argv[indicePipe], O_WRONLY);
    sleep(1);
  } while (fd == -1);
  printf("Abrio el pipe llamado: %s\n", argv[indicePipe]);
  char msg[100] = "Test mensaje... ---- asdjie";

  int isbn = 0, numEjemplar = -1;
  char nombre[50];
  char pedirNombre[] = "Ingresa el nombre del libro\n:";
  char pedirIsbn[] = "Ingresa el isbn del libro\n:";
  char pedirNumEjemplar[] = "Ingrese el numero del ejemplar\n:";
  char menu[] =
      "\nSeleccione el numero del tipo de operacion a realizar:\n1. "
      "Devolucion\n2. Renovacion\n3. Prestamo\n4. Salir\n: ";

  char pipePs[10];
  printf("Intentando crear pipe de respuesta...\n");
  sprintf(pipePs, "%s", crearPipeResp());
  printf("Pipe de respuesta creado: %s", pipePs);
  printf("\n");
  int op = 0;

  if (indiceFile != 0) {
    char *filename = &(argv[indiceFile][0]);
    FILE *fp = fopen(filename, "r");

    if (fp == NULL) {
      printf("Error:no fue posible abrir el file %s", filename);
      return 1;
    }

    const unsigned MAX_LENGTH = 256;
    char buffer[MAX_LENGTH];

    while (fgets(buffer, MAX_LENGTH, fp)) {
      char tipo[2];
      char nombre[30];
      int isbn;
      printf("\nLinea leida: %s\n", buffer);
      char delim[] = ",";
      char *ptr = strtok(buffer, delim);

      if(ptr != NULL){
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

      enviarRequest(fd, pipePs, tipo[1], nombre, isbn, -1); //Esperabamos tener números de ejemplar por ejemplo para los requests de renovación y devolución, pero no está especificado en el formato del archivo de entrada. Queda por defecto en -1
      fd2 = escucharRespuesta(pipePs);
      if (fd2 != -1) {
        close(fd2);
      }
    }

    fclose(fp);
    unlink(pipePs);
    printf("File leido\n");

    return 0;
  } else {
    do {
      printf("%s", "\n.:MENU:.");
      printf("%s", menu);
      scanf("%d", &op);
      char aux[10];
      switch (op) {
        case 1:  // Devolucion
          printf("%s", pedirNombre);
          gets(aux); //Funcionará de ignore(1);
          gets(nombre);
          printf("%s", pedirIsbn);
          scanf("%d", &isbn);
          printf("%s", pedirNumEjemplar);
          scanf("%d", &numEjemplar);
          enviarRequest(fd, pipePs, 'D', nombre, isbn, numEjemplar);
          fd2 = escucharRespuesta(pipePs);
          break;
        case 2:  // Renovacion
          printf("%s", pedirNombre);
          gets(aux); //Funcionará de ignore(1);
          gets(nombre);
          printf("%s", pedirIsbn);
          scanf("%d", &isbn);
          printf("%s", pedirNumEjemplar);
          scanf("%d", &numEjemplar);
          enviarRequest(fd, pipePs, 'R', nombre, isbn, numEjemplar);
          fd2 = escucharRespuesta(pipePs);
          break;
        case 3:  // Prestamo
          printf("%s", pedirNombre);
          gets(aux); //Funcionará de ignore(1);
          gets(nombre);
          printf("%s", pedirIsbn);
          scanf("%d", &isbn);
          enviarRequest(
              fd, pipePs, 'P', nombre, isbn,
              numEjemplar);  // numEjemplar será -1 por que no se tiene
          fd2 = escucharRespuesta(pipePs);
          break;
        case 4:
          printf("%s", "Saliendo...\n");
          if (fd2 != -1) {
            close(fd2);
          }
          close(fd);
          unlink(pipePs);
          exit(0);
          break;
        default:
          printf("Input invalido. Intente nuevamente.\n");
          break;
      }
    } while (op != 4);
  }

  close(fd);
}

void enviarRequest(int fd, char pipeR[10], char tipo, char nombre[30], int isbn,
                   int numEjemplar) {
  char msg[100];
  sprintf(msg, "%s,%c,%s,%d,%d", pipeR, tipo, nombre, isbn, numEjemplar);
  printf("Enviando request\n");
  if (write(fd, msg, sizeof(msg)) == -1) {
    printf("Error al escribir en la pipe\n");
  }
}

char *crearPipeResp() {
  int existe = -1;
  int count = 0;
  char *pipe;
  mode_t fifo_mode = S_IRUSR | S_IWUSR;
  while (existe == -1) {
    sprintf(pipe, "ps%d", count);
    if (mkfifo(pipe, fifo_mode) == -1) {
      printf("Pipe ps%d ya existe..\n", count);
      perror("mkfifo");
    } else {
      return pipe;
    }
    if (count > 100) {
      printf("Error al crear un pipe de respuesta.");
      exit(1);
    }
    count++;
  }
}

int escucharRespuesta(char pipePs[10]) {
  int fd;
  do {
    printf("Intentando abrir el pipe llamado: %s\n", pipePs);
    fd = open(pipePs, O_RDONLY);
    sleep(1);
  } while (fd == -1);
  char resp[100];
  int bytes;
  bytes = read(fd, resp, sizeof(char[100]));
  if (bytes > 0) {
    printf("Respuesta encontrada: %s\n", resp);
  } else {
    printf("Algo raro paso.. bytes = %d\n", bytes);
  }
  return fd;
}