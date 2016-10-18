/* Beispiel-Programm aus dem Buch  */
/* »Cluster Computing« von         */
/* Heiko Bauke und Stephan Mertens */
/*                                 */
/* Springer 2005                   */
/* ISBN 3-540-42299-4              */


#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"
#define SIZE 100
#define NP 4

int main(int argc, char *argv[]) {
  int len[3]={1, SIZE/NP, 1}, C_rank, C_size, i, j, k;
  double A[SIZE][SIZE], B[SIZE][SIZE/NP], C[SIZE][SIZE/NP];
  MPI_Datatype filetype, types[3]={MPI_LB, MPI_DOUBLE, MPI_UB}; 
  MPI_Aint displacements[3];
  MPI_File fh;
  MPI_Status status;
  /* MPI initialisieren */
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &C_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &C_rank);
  if (C_size!=NP)
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  /* vollständige Matrix einlesen */
  MPI_File_open(MPI_COMM_WORLD, "matrix_in1.dat", 
		MPI_MODE_RDONLY, MPI_INFO_NULL, &fh); 
  MPI_File_set_view(fh, 0, MPI_DOUBLE, MPI_DOUBLE, "native", 
		    MPI_INFO_NULL); 
  MPI_File_read_all(fh, A, SIZE*SIZE, MPI_DOUBLE, &status); 
  MPI_File_close(&fh);
  /* MPI Datentyp bauen */
  MPI_Address(&A[0][0], displacements); 
  MPI_Address(&A[0][C_rank*SIZE/NP], displacements+1); 
  MPI_Address(&A[0][SIZE], displacements+2); 
  for (i=2; i>=0; --i) 
    displacements[i]-=displacements[0];
  MPI_Type_struct(3, len, displacements, types, &filetype);
  MPI_Type_commit(&filetype); 
  /* Matrixauschnitte mit je SIZE/NP Spalten einlesen */
  MPI_File_open(MPI_COMM_WORLD, "matrix_in2.dat", 
		MPI_MODE_RDONLY, MPI_INFO_NULL, &fh); 
  MPI_File_set_view(fh, 0, MPI_DOUBLE, filetype, "native", MPI_INFO_NULL); 
  MPI_File_read_all(fh, B, SIZE*SIZE/NP, MPI_DOUBLE, &status); 
  MPI_File_close(&fh);
  /* Matrixmultiplikation */
  for (i=0; i<SIZE; ++i) 
    for (j=0; j<SIZE/NP; ++j) {
      C[i][j]=0;
      for (k=0; k<SIZE; ++k) 
	C[i][j]+=A[i][k]*B[k][j];
    }
  /* Matrixauschnitte mit je SIZE/NP Spalten schreiben */
  MPI_File_open(MPI_COMM_WORLD, "matrix_out.dat", 
		MPI_MODE_WRONLY | MPI_MODE_CREATE, MPI_INFO_NULL, &fh); 
  MPI_File_set_view(fh, 0, MPI_DOUBLE, filetype, "native", MPI_INFO_NULL);
  MPI_File_write_all(fh, C, SIZE*SIZE/NP, MPI_DOUBLE, &status); 
  MPI_File_close(&fh);
  MPI_Finalize();
  return EXIT_SUCCESS;
}
