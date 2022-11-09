typedef struct Libro {
  char status[50];
  int numeroLibro;
  char fecha[70];
};

typedef struct EstructuraDeLibro {
  char nombre[50];
  int ISBN;
  int numeroEjemplares;
  struct Libro libro[100]
} biblio;