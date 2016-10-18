/* Beispiel-Programm aus dem Buch  */
/* »Cluster Computing« von         */
/* Heiko Bauke und Stephan Mertens */
/*                                 */
/* Springer 2005                   */
/* ISBN 3-540-42299-4              */


#include <stdlib.h>
#include <unistd.h>
#include "mpi.h"

int main(int argc, char *argv[]) {
  const int cnt=100000;
  int rank, size, p[cnt], i;
  MPI_Status status;
  
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  for (i=0; i<5; ++i) {  
    if (rank==0) {
      MPI_Recv(p, cnt, MPI_INT, 1, 101, MPI_COMM_WORLD, &status);
      usleep(500000);
      MPI_Recv(p, cnt, MPI_INT, 2, 102, MPI_COMM_WORLD, &status);
    } else if (rank==1) {
      MPI_Send(p, cnt, MPI_INT, 0, 101, MPI_COMM_WORLD);
      usleep(500000);
    } else if (rank==2) {
      MPI_Send(p, cnt, MPI_INT, 0, 102, MPI_COMM_WORLD);
      usleep(500000);
    }
    usleep(500000);
    MPI_Barrier(MPI_COMM_WORLD);
    usleep(500000);
  }
  MPI_Finalize();     
  return EXIT_SUCCESS;
}
