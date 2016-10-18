/* Beispiel-Programm aus dem Buch  */
/* »Cluster Computing« von         */
/* Heiko Bauke und Stephan Mertens */
/*                                 */
/* Springer 2005                   */
/* ISBN 3-540-42299-4              */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mpi.h"

typedef struct { int x;  double y;  char str[13]; } coord;

void * secure_malloc(size_t size) {
  void * p=malloc(size);
  if (p==NULL) MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  return p;
}

void fill(coord *M, int N, int rank) {
  int i, j;
  for (j=0; j<N; ++j)
    for (i=0; i<N; ++i) {
      M[i+N*j].x=i;  M[i+N*j].y=j;
      strcpy(M[i+N*j].str, rank==0 ? "###" : "...");
    }
}

void print(coord *M, int N) {
  int i, j;
  for (j=0; j<N; ++j) {
    for (i=0; i<N; ++i)
      printf("%2i,%g,%s ", M[i+N*j].x, M[i+N*j].y, M[i+N*j].str);
    printf("\n");
  }
  printf("\n");
}

int main(int argc, char *argv[]) {
  const int N=8;
  int C_rank, C_size, i, len[4]={1, 1, 13, 1}, *tri_disp, *tri_len; 
  coord *M, *v;
  MPI_Status status;
  MPI_Datatype types[4]={MPI_INT, MPI_DOUBLE, MPI_CHAR, MPI_UB}; 
  MPI_Aint displacements[4];
  MPI_Datatype coord_t0, coord_t, row_t, col_t0, col_t, tri_t;
  /* MPI initialisieren */
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &C_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &C_rank);
  if (C_size!=2) MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  M=secure_malloc(N*N*sizeof(*M));
  /* MPI-Datentypen für Koordinate bauen */
  MPI_Address(&M[0].x, displacements); 
  MPI_Address(&M[0].y, displacements+1); 
  MPI_Address(M[0].str, displacements+2); 
  MPI_Address(&M[1].x, displacements+3); 
  for (i=3; i>=0; --i) 
    displacements[i]-=displacements[0];
  MPI_Type_struct(3, len, displacements, types, &coord_t0);
  MPI_Type_commit(&coord_t0); 
  MPI_Type_struct(4, len, displacements, types, &coord_t);
  MPI_Type_commit(&coord_t); 
  /* MPI-Datentyp für Zeilenvektor bauen */
  MPI_Type_contiguous(N, coord_t, &row_t);
  MPI_Type_commit(&row_t);
  /* MPI-Datentypen für Spaltenvektor bauen */
  MPI_Type_vector(N, 1, N, coord_t0, &col_t0);
  MPI_Type_commit(&col_t0);
  types[0]=col_t0;  MPI_Address(&M[0], displacements);    len[0]=1;
  types[1]=MPI_UB;  MPI_Address(&M[1], displacements+1);  len[1]=1;
  for (i=1; i>=0; --i)
    displacements[i]-=displacements[0];
  MPI_Type_struct(2, len, displacements, types, &col_t);
  MPI_Type_commit(&col_t);
  /* MPI-Datentyp für Dreiecksmatrix bauen */
  tri_disp=secure_malloc(N*sizeof(*tri_disp));
  tri_len=secure_malloc(N*sizeof(*tri_len));
  for (i=0; i<N; ++i) {
    tri_len[i]=N-i;  tri_disp[i]=i*N+i;
  }
  MPI_Type_indexed(N, tri_len, tri_disp, coord_t, &tri_t);
  MPI_Type_commit(&tri_t);
  /* nicht mehr benötigte Datentypen feigeben */
  MPI_Type_free(&coord_t0);  MPI_Type_free(&col_t0);
  /* neue Datenstrukturen benutzen */
  fill(M, N, C_rank);
  if (C_rank==0)  /* sende M(0,0) */
    MPI_Send(M, 1, coord_t, 1, 0, MPI_COMM_WORLD);
  else {  /* empfange M(2, 3) */
    MPI_Recv(M+2+3*N, 1, coord_t, 0, 0, MPI_COMM_WORLD, &status);
    print(M, N);
  }
  fill(M, N, C_rank);
  if (C_rank==0)  /* sende 1. u. 2. Zeile */
    MPI_Send(M+N, 2, row_t, 1, 0, MPI_COMM_WORLD);
  else {  /* empfange 4. u. 5. Zeile */
    MPI_Recv(M+4*N, 2, row_t, 0, 0, MPI_COMM_WORLD, &status);
    print(M, N);
  }
  fill(M, N, C_rank); 
  if (C_rank==0)  /* sende 1. u. 2. Spalte */
    MPI_Send(M+1, 2, col_t, 1, 0, MPI_COMM_WORLD);
  else {  /* empfange 4. u. 5 Spalte */
    MPI_Recv(M+4, 2, col_t, 0, 0, MPI_COMM_WORLD, &status);
    print(M, N);
  }
  fill(M, N, C_rank); 
  if (C_rank==0) {  /* sende Vektor der Größe einer Dreiecksmatrix */
    v=secure_malloc(N*(N+1)/2*sizeof(*v));
    for (i=0; i<N*(N+1)/2; ++i) {
      v[i].x=i;  v[i].y=0;  strcpy(v[i].str, "+++");
    }
    MPI_Send(v, N*(N+1)/2, coord_t, 1, 0, MPI_COMM_WORLD);
  } else {  /* empfange Dreiecksmatrix */
    MPI_Recv(M, 1, tri_t, 0, 0, MPI_COMM_WORLD, &status);
    print(M, N);
  }
  MPI_Type_free(&tri_t);  MPI_Type_free(&col_t);
  MPI_Type_free(&row_t);  MPI_Type_free(&coord_t);
  MPI_Finalize(); 
  return EXIT_SUCCESS;
}
