#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "BaseDatos.h"

void ingresarArchivo(biblio *bibli, char *argv[1]);
void cambioEstatus(biblio *bibli, char nombre[], int isbn, char status[]);
void imprimirReporte(biblio *bibli);
bool buscarLibro(biblio *bibli, int auxIsbn,, char auxNombre[]);
char renovarFecha(char fechaAux[]);

int main (int argc, char *argv[1])
{
  int fd, pid, n, bytes;
  Libro est;
  mode_t fifo_mode = S_IRUSR | S_IWUSR;

  if (mkfifo (argv[1], fifo_mode) == -1) {
     perror("mkfifo");
     exit(1);
  }
  fd = open (argv[1], O_WRONLY);
  printf ("Abrio el pipe\n");
  est.numeroLibro = 50;

  if(write(fd, &est, siezeof(Libro)) == -1)
  {
    prntf("Error al escribir en la pipe");
  }

    biblio bibli[100];
    int isbn = 0, op1 = 0, op2 = 0, i = 0;
    char nombre[30];
    char pedirNombre[] = "Ingresa el nombre del libro";
    char pedirIsbn[] = "Ingresa el isbn del libro";
    char menu1[] = "\nSelecciona el numero del tipo de operacion a realizar:\n1. Ingresar desde archivo\n2. Ingresar manualmente\n3. Imprimir reporte\n4. Salir\n: ";
    char menu2[] = "\nSeleccione el numero del tipo de operacion a realizar:\n1. Devolucion\n2. Renovacion\n3. Prestamo\n4. Salir\n: ";

    FILE *fp;
    fp = fopen(argv[1], "r");
    time_t t = time(NULL);
    struct tm tiempoLocal = *localtime(&t);
    char fecha[70];
    char *formato = "%d-%m-%Y";
    strftime(fecha, sizeof fecha, formato, &tiempoLocal);

    while(!feof(fp))
    {
        fscanf(fp, "%[^,],%d,%d\n", bibli[i].nombre, &bibli[i].ISBN, &bibli[i].numeroEjemplares);
        for(int j=0; j<&bibli[i].numeroEjemplares)
        {
            fscanf(fp, "%d, %s\n", bibli[i].libro[j].numeroLibro, bibli[i].libro[j].status);
            strcpy(bibli[i].libro[j].fecha, fecha);
        }
        i++;
    }

    do
    {
        printf("%s", "\n.:MENU PRINCIPAL:.");
        printf("%s", menu1);
        scanf("%d", &op1);
        switch(op1)
        {
        case 1:
            ingresarArchivo(biblio *bibli, //falta mandarle el archivo que leera);
            break;
        case 2:
            do
            {
                printf("%s", "\n.:SUB MENU:.");
                printf("%s", menu2);
                scanf("%d", &op2);
                switch (op2)
                {
                case 1:
                    printf("%s", pedirNombre);
                    scanf("%s", &nombre);
                    printf("%s", pedirIsbn);
                    scanf("%d", &isbn);
                    cambioStatus(bibli, nombre, isbn, "D");
                    break;
                case 2:
                    printf("%s", pedirNombre);
                    scanf("%s", &nombre);
                    printf("%s", pedirIsbn);
                    scanf("%d", &isbn);
                    cambioStatus(bibli, nombre, isbn, "R");
                    break;
                case 3:
                    printf("%s", pedirNombre);
                    scanf("%s", &nombre);
                    printf("%s", pedirIsbn);
                    scanf("%d", &isbn);
                    cambioStatus(bibli, nombre, isbn, "P");
                    break;
                case 4:
                    printf("%s", "Redirigiendote al menu principal...\n");
                    sleep(2);
                    op2=5;
                    break;
                default:
                    printf("Input invalido. Intente nuevamente.\n");
                    break;
                }
            }
            while(op2 < 5);
            break;
        case 3:
            imprimirReporte(bibli);
            break;
        case 4:
            printf("%s", "Muchas gracias por utilizar nuestro programa, vuelve pronto :)\n");
            op1=5;
            break;
        default:
            printf("Input invalido. Intente nuevamente.\n");
            break;
        }
    }
    while(op1 != 5);
}

void ingresarArchivo(biblio *bibli, char *argv[1])
{
    char auxStatus[50];
    char auxNombre[50];
    int auxIsbn;
    FILE *fp;
    fp = fopen(argv[1], "r");

    while(!feof(fp))
    {
        fscanf(fp, "%s,%[^,],%d\n", auxStatus, auxNombre, auxIsbn);
        if(buscarLibro(bibli, auxIsbn, auxNombre)==true)
        {
            cambioStatus(bibli, auxNombre, auxIsbn, auxStatus);
        }
        else
        {
            printf("%s", "se intento acceder a un libro que no existe");
        }
    }
}

void cambioStatus(biblio *bibli, char nombre[], int isbn, char status[])
{
    for(int i=0; i<sizeof(bibli); i++)
    {
        if(buscarLibro(bibli, nombre, isbn)==true)
        {
            if(strcmp(status, "D")==0)
            {
                for(int j=0; j<sizeof(bibli.libro); j++)
                {
                    if(strcmp(bibli[i].libro[j].status, "P")==0)
                    {
                        strcpy(bibli[i].libro[j].status, "D");
                        printf("%s", "el libro ha sido devuelto con exito");
                    }
                }
            }
            else if(strcmp(status, "R")==0)
            {
                for(int j=0; j<sizeof(bibli.libro); j++)
                {
                    if(strcmp(bibli[i].libro[j].status, "P")==0)
                    {
                        strcpy(bibli[i].libro[j].fecha, renovarFecha(bibli[i].libro[j].fecha));
                        printf("%s", "el libro ha sido renovado con exito, tienes una semana más para entregarlo");
                    }
                }
            }
            else if(strcmp(status, "P")==0)
            {
                for(int j=0; j<sizeof(bibli.libro); j++)
                {
                    if(strcmp(bibli[i].libro[j].status, "D")==0)
                    {
                        strcpy(bibli[i].libro[j].status, "P");
                        printf("%s", "el libro ha sido prestado con exito");
                    }
                }
            }
        }
        else
        {
            printf("%s", "el libro al que intentas acceder no existe");
        }
    }
}

void imprimirReporte(biblio *bibli)
{
    for(int i=0; i<sizeof(bibli); i++)
    {
        for(int j=0; j<sizeof(bibli.libro); i++)
        {
            printf("%s,%[^,],%d,%d,%s\n",  bibli[j].libro[i].status, bibli[j].nombre, bibli[j].ISBN, bibli[j].libro[i].numeroLibro, bibli[j].libro[i].fecha);
        }
    }
}

bool buscarLibro(biblio *bibli, int auxIsbn, char auxNombre[])
{
    for(int i=0; i<sizeof(bibli); i++)
    {
        if((strcmp(bibli[i].ISBN, auxIsbn)==0)&&(strcmp(bibli[i].nombre, auxNombre)==0))
        {
            return true;
        }
    }
    return false;
}

char renovarFecha(char fechaAux[])
{
    int dia = 0, mes = 0, anio = 0, cont = 0;
    char nuevaFecha[70];
    fscanf(fechaAux, "%d-%d-%d", dia, mes anio);

    if(dia<21)
    {
        dia+=7;
        nuevaFecha = (dia+'0')+(mes+'0')+(anio+'0');
        return nuevaFecha;
    }
    else if(dia<24 && mes!=2)
    {
        dia+=7;
        nuevaFecha = (dia+'0')+(mes+'0')+(anio+'0');
        return nuevaFecha;
    }
    else if(dia<=24 && (mes==1 || mes==3 || mes==5 || mes==6 || mes==7 || mes==8 || mes==10 || mes==12))
    {
        dia+=7;
        nuevaFecha = (dia+'0')+(mes+'0')+(anio+'0');
        return nuevaFecha;
    }
    else if(dia>24 && (mes==1 || mes==3 || mes==5 || mes==7 || mes==8 || mes==10))
    {
        cont = 31;
        for(int i==0; i<7; i++)
        {
            if(dia==cont)
            {
                dia=7-cont;
                mes++;
                break;
            }
            else
            {
                dia++;
            }
        }
        nuevaFecha = (dia+'0')+(mes+'0')+(anio+'0');
        return nuevaFecha;
    }
    else if(dia>24 && (mes==4 || mes==6 || mes==9 || mes==11))
    {
        cont = 30;
        for(int i==0; i<7; i++)
        {
            if(dia==cont)
            {
                dia=7-cont;
                mes++;
                break;
            }
            else
            {
                dia++;
            }
        }
        nuevaFecha = (dia+'0')+(mes+'0')+(anio+'0');
        return nuevaFecha;
    }
    else
    {
        cont = 31;
        for(int i==0; i<7; i++)
        {
            if(dia==cont)
            {
                dia=7-cont;
                mes=1;
                anio=1
                     break;
            }
            else
            {
                dia++;
            }
        }
        nuevaFecha = (dia+'0')+(mes+'0')+(anio+'0');
        return nuevaFecha;
    }
}