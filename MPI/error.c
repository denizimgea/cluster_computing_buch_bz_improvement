/* Beispiel-Programm aus dem Buch  */
/* »Cluster Computing« von         */
/* Heiko Bauke und Stephan Mertens */
/*                                 */
/* Springer 2005                   */
/* ISBN 3-540-42299-4              */


#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"

void comm_errhandler(MPI_Comm *comm, int *err) {
  int err_cl, err_len;
  char err_str[MPI_MAX_ERROR_STRING+1];
  fprintf(stderr, "Kommunikationsfehler\n");
  /* Fehlerklasse */
  MPI_Error_class(*err, &err_cl);
  MPI_Error_string(err_cl, err_str, &err_len);
  err_str[err_len]='\0';
  fprintf(stderr, "error class: %s\n", err_str);
  /* Fehlercode */
  MPI_Error_string(*err, err_str, &err_len);
  err_str[err_len]='\0';
  fprintf(stderr, "error code : %s\n", err_str);
  MPI_Abort(*comm, EXIT_FAILURE);   /* Programm abbrechen */
}

int main(int argc, char *argv[]) {
  int C_rank, C_size,  x[10];
  MPI_Status status;
  MPI_Errhandler errh_old, errh_new;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &C_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &C_size);
 /* alte Fehlerbehandlungsroutine sichern und
     eigene Fehlerbehandlungsroutine registrieren */
  MPI_Errhandler_get(MPI_COMM_WORLD, &errh_old);
  MPI_Errhandler_create((MPI_Handler_function *)(&comm_errhandler),
			&errh_new);
  MPI_Errhandler_set(MPI_COMM_WORLD, errh_new); 
  /* Kommunikation */
  if (C_rank==0) {
    MPI_Send(x, 10, MPI_INT, 1, 0, MPI_COMM_WORLD);
    fprintf(stderr, "Daten erfolgreich gesendet\n");
  }
  if (C_rank==1) {
    MPI_Recv(x, 10, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    fprintf(stderr, "Daten erfolgreich empfangen\n");
  }
  /* alte Fehlerbehandlungsroutine wieder registrieren */
  MPI_Errhandler_set(MPI_COMM_WORLD, errh_old);
  MPI_Errhandler_free(&errh_new);
  MPI_Finalize();
  return EXIT_SUCCESS;
}
