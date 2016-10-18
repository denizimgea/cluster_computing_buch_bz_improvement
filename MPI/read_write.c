/* Beispiel-Programm aus dem Buch  */
/* »Cluster Computing« von         */
/* Heiko Bauke und Stephan Mertens */
/*                                 */
/* Springer 2005                   */
/* ISBN 3-540-42299-4              */


#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#define SIZE 100

int main(int argc, char *argv[]) {
  MPI_File fh;
  MPI_Status status;
  int a[SIZE], C_rank, i;
  
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &C_rank);
  /* alle Prozesse lesen gleichen Vektor mit SIZE Ganzzahlen */
  MPI_File_open(MPI_COMM_WORLD, "data_in.dat", 
		MPI_MODE_RDONLY, MPI_INFO_NULL, &fh); 
  MPI_File_set_view(fh, 0, MPI_INT, MPI_INT, "native", MPI_INFO_NULL); 
  MPI_File_read_all(fh, a, SIZE, MPI_INT, &status); 
  MPI_File_close(&fh);
  /* aufwendige Rechnung */
  for (i=0; i<SIZE; ++i) 
    a[i]*=C_rank+1;
  /* Prozesse schreiben verschiedene Vektoren mit SIZE Ganzzahlen 
     in Blöcken hintereinander */
  MPI_File_open(MPI_COMM_WORLD, "data_out.dat", 
		MPI_MODE_WRONLY | MPI_MODE_CREATE, MPI_INFO_NULL, &fh); 
  MPI_File_set_view(fh, 0, MPI_INT, MPI_INT, "native", MPI_INFO_NULL);
  MPI_File_seek(fh, C_rank*SIZE, MPI_SEEK_SET);
  MPI_File_write_all(fh, a, SIZE, MPI_INT, &status); 
  MPI_File_close(&fh);
  MPI_Finalize();
  return EXIT_SUCCESS;
}
