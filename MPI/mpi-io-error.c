/* Beispiel-Programm aus dem Buch  */
/* »Cluster Computing« von         */
/* Heiko Bauke und Stephan Mertens */
/*                                 */
/* Springer 2005                   */
/* ISBN 3-540-42299-4              */


#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

void file_errhandler(MPI_File *fh, int *err) {
  int err_cl, err_len;
  char err_str[MPI_MAX_ERROR_STRING+1];
  fprintf(stderr, "Dateizugriffsfehler\n");
  /* Fehlerklasse */
  MPI_Error_class(*err, &err_cl);
  MPI_Error_string(err_cl, err_str, &err_len);
  err_str[err_len]='\0';
  fprintf(stderr, "error class: %s\n", err_str);
  /* Fehlercode */
  MPI_Error_string(*err, err_str, &err_len);
  err_str[err_len]='\0';
  fprintf(stderr, "error code : %s\n", err_str);
  MPI_File_close(fh);  /* Datei schließen */
  MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);   /* Programm abbrechen */
}

int main(int argc, char *argv[]) {
  MPI_File fh;
  MPI_Errhandler errh_old, errh_new;
  int err, err_cl, err_len;
  char err_str[MPI_MAX_ERROR_STRING+1];

  MPI_Init(&argc, &argv);
  err=MPI_File_open(MPI_COMM_WORLD, "hidden_file.dat",     /* Datei  */
		    MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);  /* öffnen */
  if (err!=MPI_SUCCESS) {  /* Ist beim Öffnen ein Fehler aufgetreten? */
    fprintf(stderr, "MPI_File_open schlug fehl.\n");
    /* Fehlerklasse */
    MPI_Error_class(err, &err_cl);
    MPI_Error_string(err_cl, err_str, &err_len);
    err_str[err_len]='\0';
    fprintf(stderr, "error class: %s\n", err_str);
    /* Fehlercode */
    MPI_Error_string(err, err_str, &err_len);
    err_str[err_len]='\0';
    fprintf(stderr, "error code : %s\n", err_str);
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);  /* Programm abbrechen */
  }
  /* alte Fehlerbehandlungsroutine sichern und
     eigene Fehlerbehandlungsroutine registrieren */
  MPI_File_get_errhandler(fh, &errh_old);
  MPI_File_create_errhandler((MPI_File_errhandler_fn *)file_errhandler,
			     &errh_new);
  MPI_File_set_errhandler(fh, errh_new);
  /* Dateioperationen */
  
  /* alte Fehlerbehandlungsroutine wieder registrieren */
  MPI_File_set_errhandler(fh, errh_old);
  MPI_Errhandler_free(&errh_new);
  MPI_File_close(&fh);  /* Datei schließen */
  MPI_Finalize();
  return EXIT_SUCCESS;
}
