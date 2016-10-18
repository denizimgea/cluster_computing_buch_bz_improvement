/* Beispiel-Programm aus dem Buch  */
/* »Cluster Computing« von         */
/* Heiko Bauke und Stephan Mertens */
/*                                 */
/* Springer 2005                   */
/* ISBN 3-540-42299-4              */


#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int main(int argc, char *argv[]) {
  int myrank, nprocs, len;
  char name[MPI_MAX_PROCESSOR_NAME+1];

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  MPI_Get_processor_name(name, &len);
  name[len]='\0';
  printf("Prozess %d (von %d) läuft auf %s.\n", myrank, nprocs, name);
  MPI_Finalize();
  return EXIT_SUCCESS;
}
