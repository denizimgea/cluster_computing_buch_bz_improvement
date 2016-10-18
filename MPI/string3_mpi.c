/* Beispiel-Programm aus dem Buch  */
/* »Cluster Computing« von         */
/* Heiko Bauke und Stephan Mertens */
/*                                 */
/* Springer 2005                   */
/* ISBN 3-540-42299-4              */


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "mpi.h"

const int N=1000;
const double L=1, c=1, dt=0.001, t_end=2.4;
enum { left_copy, right_copy };

void string(double *u, double *u_old, double *u_new, int N, double eps);

void * secure_malloc(size_t size) {
  void * p=malloc(size);
  if (p==NULL)
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  return p;
}

double u_0(double x) {  return exp(-200.0*(x-0.5*L)*(x-0.5*L));  }

double u_0_dt(double x) {  return 0.0;  }

int main(int argc, char *argv[]) {
  int C_rank, C_size, *N_l, *N0_l, i;
  double dx=L/N, eps=dt*dt*c*c/(dx*dx), 
    *u=NULL, *u_l, *u_old_l, *u_new_l, *u_temp, x, t;
  MPI_Status status, statuses[2];
  MPI_Request requests[2];
  /* MPI initialisieren */
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &C_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &C_rank);
  /* Zahl der lokalen Stützstellen berechnen */
  N_l=secure_malloc(C_size*sizeof(*N_l));
  N0_l=secure_malloc(C_size*sizeof(*N0_l));
  for (i=0; i<C_size; ++i) {
    N_l[i]=(i+1)*(N-1)/C_size-i*(N-1)/C_size+1;
    N0_l[i]=i*(N-1)/C_size;
  }
  /* Speicher allozieren und Anfangswerte berechnen */
  u_old_l=secure_malloc((N_l[C_rank]+1)*sizeof(*u_old_l)); 
  u_l=secure_malloc((N_l[C_rank]+1)*sizeof(*u_l));          
  u_new_l=secure_malloc((N_l[C_rank]+1)*sizeof(*u_new_l));  
  for (i=0; i<=N_l[C_rank]; ++i) {
    x=(i+N0_l[C_rank])*dx;
    u_old_l[i]=u_0(x);
    u_l[i]=0.5*eps*(u_0(x-dx)+u_0(x+dx))+(1.0-eps)*u_0(x)+dt*u_0_dt(x);
  }
  /* DGL lösen */
  for (t=2*dt; t<=t_end; t+=dt) {
    string(u_l, u_old_l, u_new_l, N_l[C_rank], eps);
    /* nach rechts und links senden */
    MPI_Isend(&u_new_l[N_l[C_rank]-1], 1, MPI_DOUBLE,
              C_rank+1<C_size ? C_rank+1 : MPI_PROC_NULL, right_copy,
              MPI_COMM_WORLD, &requests[0]);
    MPI_Isend(&u_new_l[1],             1, MPI_DOUBLE,
              C_rank-1>=0 ? C_rank-1 : MPI_PROC_NULL, left_copy,
              MPI_COMM_WORLD, &requests[1]);
    MPI_Recv(&u_new_l[0],           1, MPI_DOUBLE,
             C_rank-1>=0 ? C_rank-1 : MPI_PROC_NULL, right_copy,
             MPI_COMM_WORLD, &status);
    MPI_Recv(&u_new_l[N_l[C_rank]], 1, MPI_DOUBLE,
             C_rank+1<C_size ? C_rank+1 : MPI_PROC_NULL, left_copy,
             MPI_COMM_WORLD, &status);
    MPI_Waitall(2, requests, statuses);
    /* Felder tauschen */
    u_temp=u_old_l;  u_old_l=u_l;  u_l=u_new_l;  u_new_l=u_temp;
  }
  /* Daten einsammeln und Endposition ausgeben */
  if (C_rank==0) 
    u=secure_malloc((N+1)*sizeof(*u));
  ++N_l[C_size-1];
  MPI_Gatherv(u_l, N_l[C_rank], MPI_DOUBLE, u, N_l, N0_l, MPI_DOUBLE,
	      0, MPI_COMM_WORLD);
  if (C_rank==0) 
    for (i=0; i<=N; ++i)
      printf("%g\n", u[i]);
  /* Ressourcen freigeben */
  free(u);  free(u_old_l);  free(u_l);  free(u_new_l);
  MPI_Finalize();
  return EXIT_SUCCESS;
}
