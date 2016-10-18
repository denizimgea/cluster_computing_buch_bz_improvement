/* Beispiel-Programm aus dem Buch  */
/* »Cluster Computing« von         */
/* Heiko Bauke und Stephan Mertens */
/*                                 */
/* Springer 2005                   */
/* ISBN 3-540-42299-4              */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mpi.h"

int main(int argc, char *argv[]) {
  int rank, i;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  for (i=0; i<100; ++i) {
    fprintf(stdout, "Text von Knoten %i. i = %i\n", rank, i);
    fprintf(stderr, "Fehler von Knoten %i. i = %i\n", rank, i);
    usleep(100);  /* simuliere Arbeit */
  }
  MPI_Finalize();     
  return EXIT_SUCCESS;
}
