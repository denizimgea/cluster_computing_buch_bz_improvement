/* Beispiel-Programm aus dem Buch  */
/* »Cluster Computing« von         */
/* Heiko Bauke und Stephan Mertens */
/*                                 */
/* Springer 2005                   */
/* ISBN 3-540-42299-4              */


#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "mpi.h"

int main(int argc, char *argv[]) {
  time_t t;
  char str[256];
  int rank;
  MPI_Comm Universum;
  MPI_Datatype String256;
  MPI_Init(&argc, &argv);
  MPI_Comm_dup(MPI_COMM_WORLD, &Universum);
  MPI_Comm_rank(Universum, &rank);
  MPI_Type_contiguous(256, MPI_CHAR, &String256);
  MPI_Type_commit(&String256);
  MPI_Type_set_name(String256, "Zeichenkette mit 255 Zeichen");
  MPI_Comm_set_name(Universum, "MPI Universum");
  if (rank==0) {
    t=time(NULL);
    strncpy(str, ctime(&t), 256);
  }
  MPI_Bcast(str, 1, String256, 0, Universum);
  printf("rank %i: %s", rank, str);
  MPI_Comm_free(&Universum);
  MPI_Type_free(&String256);
  MPI_Finalize();
}
