/* Beispiel-Programm aus dem Buch  */
/* �Cluster Computing� von         */
/* Heiko Bauke und Stephan Mertens */
/*                                 */
/* Springer 2005                   */
/* ISBN 3-540-42299-4              */


#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"

const int n=255, k1=3, k2=3, g=41, t_end=1000;
int Nx=400, Ny=300;
enum { gather_tag, sendrecv_tag };

void * secure_malloc(size_t size) {
  void * p=malloc(size);
  if (p==NULL)
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  return p;
}

void bz_iterate(int *X, int *X_new, int Nx, int Ny,
		int n, int k1, int k2, int g);

int main(int argc, char *argv[]) {
  int C_rank, C_size, P[2]={0, 0}, P_periods[2]={1, 1}, P_l[2], 
    *merged_matrix=NULL, *process_matrix, *updated_process_matrix, *matrix_replace_helper, i, j, k, t, Nx_l, Ny_l,
    send_offsets[4], receive_offsets[4], sender_ranks[4], receiver_ranks[4], coords[2];
  MPI_Comm comm;
  MPI_Datatype submatrix_t, innermatrix_t, row_t, col_t, types[8];
  MPI_Request request;
  MPI_Status status;
  /* MPI initialisieren und kartesischen Kommunikator bauen */
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &C_size);
  MPI_Dims_create(C_size, 2, P);
  MPI_Cart_create(MPI_COMM_WORLD, 2, P, P_periods, 1, &comm);
  MPI_Comm_rank(comm, &C_rank);
  MPI_Cart_coords(comm, C_rank, 2, P_l);
  /* Gr��e des zellul�ren Automaten abrunden */
  Nx -=Nx%P[0];    Nx_l=Nx/P[0];
  Ny -=Ny%P[1];    Ny_l=Ny/P[1];
  /* MPI-Datentypen bauen */
  MPI_Type_vector(1, Nx_l + 2, Nx_l + 2, MPI_INT, &row_t);
  MPI_Type_commit(&row_t); 
  MPI_Type_vector(Ny_l, 1, Nx_l+2, MPI_INT, &col_t);
  MPI_Type_commit(&col_t); 
  MPI_Type_vector(Ny_l, Nx_l, Nx_l+2, MPI_INT, &innermatrix_t);
  MPI_Type_commit(&innermatrix_t);
  MPI_Type_vector(Ny_l, Nx_l, Nx, MPI_INT, &submatrix_t);
  MPI_Type_commit(&submatrix_t);
  /* Speicher f�r Automaten allozieren und mit Zufallswerten f�llen */
  process_matrix          =secure_malloc((Nx_l+2)*(Ny_l+2)*sizeof(*process_matrix));
  process_matrix         +=(Nx_l+2)+1;
  updated_process_matrix  =secure_malloc((Nx_l+2)*(Ny_l+2)*sizeof(*updated_process_matrix));
  updated_process_matrix +=(Nx_l+2)+1;
  srand(C_rank); 
  for (j=0; j<Ny_l; ++j)
    for (i=0; i<Nx_l; ++i)
      process_matrix[i+(Nx_l+2)*j]=rand()%n;

  /* Daten f�r MPI_Sendrecv aufbereiten */
  k=0;
  for (j=0; j<=1; ++j)  /* R�nge und Typen f�r hor. & vert. Austausch */
    for (i=-1; i<=1; i+=2, ++k) {
      types[k]=j==0 ? col_t : row_t;
      MPI_Cart_shift(comm, j, i, receiver_ranks+k, sender_ranks+k);
    }
  send_offsets[0]=0;                         receive_offsets[0]=Nx_l;
  send_offsets[1]=Nx_l-1;                    receive_offsets[1]=-1;
  send_offsets[2]=-1;                        receive_offsets[2]=(Nx_l+2)*Ny_l - 1;
  send_offsets[3]=(Nx_l+2)*(Ny_l-1) - 1;     receive_offsets[3]=-(Nx_l+2) - 1;
  /* Regel des Automaten iterieren */
  for (t=0; t<t_end; ++t) {
    for (i=0; i<8; ++i) 
      MPI_Sendrecv(process_matrix+send_offsets[i], 1, types[i], sender_ranks[i], sendrecv_tag,
		   process_matrix+receive_offsets[i], 1, types[i], receiver_ranks[i], sendrecv_tag,
		   comm, &status);
    bz_iterate(process_matrix, updated_process_matrix, Nx_l, Ny_l, n, k1, k2, g);
    matrix_replace_helper = process_matrix;
    process_matrix = updated_process_matrix;
    updated_process_matrix = matrix_replace_helper;
  }
  /* Daten einsammeln und ausgeben */
  MPI_Isend(process_matrix, 1, innermatrix_t, 0, gather_tag, comm, &request);
  if (C_rank==0) {
    merged_matrix=secure_malloc(Nx*Ny*sizeof(*merged_matrix));
    for (P_l[1]=0; P_l[1]<P[1]; ++P_l[1])
      for (P_l[0]=0; P_l[0]<P[0]; ++P_l[0]) {
        MPI_Cart_rank(comm, P_l, &C_rank);
        MPI_Recv(merged_matrix+P_l[0]*Nx_l+P_l[1]*Nx*Ny_l,
		 1, submatrix_t, C_rank, gather_tag, comm, &status);
      }
    printf("P5\n%i %i\n%i\n", Nx, Ny, n);  /* PGM-Header */
    for (i=0; i<Nx*Ny; ++i)
      fputc(merged_matrix[i], stdout);
  }
  MPI_Wait(&request, &status);
  /* Ressourcen freigeben */
  MPI_Type_free(&submatrix_t);  MPI_Type_free(&innermatrix_t);
  MPI_Comm_free(&comm);
  MPI_Finalize();
  return EXIT_SUCCESS;
}
