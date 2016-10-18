/* Beispiel-Programm aus dem Buch  */
/* »Cluster Computing« von         */
/* Heiko Bauke und Stephan Mertens */
/*                                 */
/* Springer 2005                   */
/* ISBN 3-540-42299-4              */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

double pintegral(double (*f)(double), double a, double b, 
		 long n, int k, int tasks); /* siehe pintegral.c */

double f(double x) {
  return sqrt(1.0-x*x);
}

int main (int argc, char *argv[]) {
  int myrank, nprocs, src;
  MPI_Status status;
  double result, x, T;
  long n=1000000;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  if (argc==2) 
    n=atol(argv[1]);
  T=MPI_Wtime();
  result=pintegral(f, 0.0, 1.0, n, myrank, nprocs);
  if (myrank>0) 
    MPI_Send(&result, 1, MPI_DOUBLE, 0, 99, MPI_COMM_WORLD);
  else {
    for (src=1; src<nprocs; ++src) {
      MPI_Recv(&x, 1, MPI_DOUBLE, src, 99, MPI_COMM_WORLD, &status);
      result+=x;
    }
    T=MPI_Wtime()-T;
    printf("%ld Stuetzst., %d Proz. Zeit %6.4g Sek, Ergebnis %15.12g\n", 
	   n, nprocs, T, result);
  }
  MPI_Finalize();
  return EXIT_SUCCESS;
}
