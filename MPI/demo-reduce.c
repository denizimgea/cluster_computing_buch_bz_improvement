/* Beispiel-Programm aus dem Buch  */
/* »Cluster Computing« von         */
/* Heiko Bauke und Stephan Mertens */
/*                                 */
/* Springer 2005                   */
/* ISBN 3-540-42299-4              */


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"

void print_data(int p, const char *prompt, const int *d, int count) {
  int k;
  printf("Process %d (%s): ", p, prompt);
  for (k=0; k<count; ++k)
    printf("%4d", d[k]);
  printf("\n");
}

void MPI_Ggt(void *in, void *inout, int *len, MPI_Datatype *type) {
  int i, m, n, t;
  if (*type!=MPI_INT) 
    MPI_Abort(MPI_COMM_WORLD, MPI_ERR_OP);
  int *a=(int *)in, *b=(int *)inout;
  for (i=0; i<*len; ++i) {
    m=abs(a[i]); 
    n=abs(b[i]);
    while (n>0) {
      t=m%n;  m=n;  n=t;
    }
    b[i]=m;
  }
}

int main(int argc, char *argv[]) {
  const int dim=5;
  int myrank, nprocs, p, data[dim], result[dim];
  MPI_Status status;
  MPI_Op MPI_GGT;

  MPI_Init (&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  MPI_Op_create(&MPI_Ggt, 1, &MPI_GGT);
  srand((myrank+17) * time(NULL));
  for (p=0; p<dim; ++p) 
    data[p]=64+rand()%93;
  print_data(myrank, "data", data, dim); 
  MPI_Barrier(MPI_COMM_WORLD);
  if (myrank==0) printf("MPI_Reduce:\n");
  MPI_Reduce(data, result, dim, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
  if (myrank==0) print_data(0, "Max.", result, dim);  
  MPI_Reduce(data, result, dim, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);
  if (myrank==0) print_data(0, "Min.", result, dim);
  MPI_Reduce(data, result, dim, MPI_INT, MPI_GGT, 0, MPI_COMM_WORLD);
  if (myrank==0) print_data(0, "GGT ", result, dim); 
  MPI_Op_free(&MPI_GGT);
  MPI_Finalize();
  return EXIT_SUCCESS;
}
