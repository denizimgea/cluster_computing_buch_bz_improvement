/* Beispiel-Programm aus dem Buch  */
/* »Cluster Computing« von         */
/* Heiko Bauke und Stephan Mertens */
/*                                 */
/* Springer 2005                   */
/* ISBN 3-540-42299-4              */


#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"

int main(int argc, char *argv[]) {
  int myrank, numprocs, k; 
  const int msg_size=1024, samples=100, ping=1, pong=2;
  double t, x[msg_size];
  MPI_Status status;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank); 
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs); 
  if (numprocs!=2) {
    fprintf(stderr, "Sorry, genau zwei Prozesse erforderlich.\n");
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }
  if (myrank==0) {
    MPI_Barrier(MPI_COMM_WORLD);
    for (k=0; k<samples; ++k) {
      t=MPI_Wtime();
      MPI_Send(x, msg_size, MPI_DOUBLE, 1, ping, MPI_COMM_WORLD);
      MPI_Recv(x, msg_size, MPI_DOUBLE, 1, pong, MPI_COMM_WORLD, &status);
      t=MPI_Wtime()-t;
      printf("%d: %lf\n", k, 0.5*t);
    }
  } else { 
    MPI_Barrier(MPI_COMM_WORLD);
    for (k=0; k<samples; ++k) {
      MPI_Recv(x, msg_size, MPI_DOUBLE, 0, ping, MPI_COMM_WORLD, &status);
      MPI_Send(x, msg_size, MPI_DOUBLE, 0, pong, MPI_COMM_WORLD); 
    }
  }  
  MPI_Finalize(); 
  return EXIT_SUCCESS;
}
