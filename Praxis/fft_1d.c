/* Beispiel-Programm aus dem Buch  */
/* »Cluster Computing« von         */
/* Heiko Bauke und Stephan Mertens */
/*                                 */
/* Springer 2005                   */
/* ISBN 3-540-42299-4              */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fftw_mpi.h>
#include "mpi.h"

void * secure_malloc(size_t size) {
  void * p=malloc(size);
  if (p==NULL)
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  return p;
}

int main(int argc, char *argv[]) {
  int C_rank, C_size, size, i, j, n,
    n1, start1, n2, start2, *n_g=NULL, *start_g=NULL;
  double time;
  fftw_mpi_plan plan;
  fftw_complex dummy, *data=NULL, *work=NULL, *buffer;
  fftw_direction dir;
  FILE *fd;
  MPI_Datatype type[2]={MPI_DOUBLE, MPI_DOUBLE}; 
  int blocklen[2]={1, 1}; 
  MPI_Aint disp[2];
  MPI_Datatype MPI_FFTW_COMPLEX;
  MPI_Status Status;

  /* MPI initialisieren */
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &C_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &C_size);
  /* MPI-Datentyp für komplexe Zahlen bauen */
  MPI_Address(&dummy, disp); 
  MPI_Address(&dummy.im, disp+1); 
  disp[1]-=disp[0];  disp[0]=0;
  MPI_Type_struct(2, blocklen, disp, type, &MPI_FFTW_COMPLEX);
  MPI_Type_commit(&MPI_FFTW_COMPLEX); 
  /* Transformationsrichtung und Zahl der Datenpunkte bestimmen */
  if (argc!=5)
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  if (strcmp(argv[1], "--forward"))
    dir=FFTW_FORWARD;
  else if (strcmp(argv[1], "--backward"))
    dir=FFTW_BACKWARD;
  else 
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  sscanf(argv[2], "%i", &n);
  /* Plan für Transformation erstellen und lokale Parameter bestimmen */
  plan=fftw_mpi_create_plan(MPI_COMM_WORLD, n, dir, FFTW_ESTIMATE); 
  fftw_mpi_local_sizes(plan, &n1, &start1, &n2, &start2, &size);
  /* Arbeitsspeicher allozieren */
  data=secure_malloc(sizeof(*data)*size);
  work=secure_malloc(sizeof(*work)*size);
  if (C_rank==0) {
    n_g=secure_malloc(sizeof(*n_g)*C_size);
    start_g=secure_malloc(sizeof(*start_g)*C_size);
  }
  /* Daten einlesen und an Prozesse verteilen */
  MPI_Gather(&n1, 1, MPI_INT, n_g, 1, MPI_INT,0, MPI_COMM_WORLD);
  MPI_Gather(&start1, 1, MPI_INT, start_g, 1, MPI_INT, 0, MPI_COMM_WORLD);
  if (C_rank==0) { 
    if ((fd=fopen(argv[3], "r"))==NULL)
      MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    for (i=0; i<C_size; ++i) {
      buffer=secure_malloc(sizeof(*buffer)*n_g[i]);
      for (j=0; j<n_g[i]; ++j)
	fscanf(fd, "%lf%lf", &buffer[j].re, &buffer[j].im);
      if (i!=0)
	MPI_Send(buffer, n_g[i], MPI_FFTW_COMPLEX, i, 0, MPI_COMM_WORLD);
      else
	memcpy(data, buffer, sizeof(*data)*n_g[i]);
      free(buffer);
    }
    fclose(fd);
  } else
    MPI_Recv(data, n1, MPI_FFTW_COMPLEX, 0, 0, MPI_COMM_WORLD, &Status);
  /* Daten transformieren */
  MPI_Barrier(MPI_COMM_WORLD);
  time=MPI_Wtime();
  fftw_mpi(plan, 1, data, work);
  MPI_Barrier(MPI_COMM_WORLD);
  time=MPI_Wtime()-time;
  if (C_rank==0) 
    printf("%i Datenpunkte in %g Sekunden auf %i Knoten transformiert\n", 
	   n, time, C_size);
  /* Daten einsammeln und ausgeben */
  MPI_Gather(&n2, 1, MPI_INT, n_g, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Gather(&start2, 1, MPI_INT, start_g, 1, MPI_INT, 0, MPI_COMM_WORLD);
  if (C_rank==0) {
    if ((fd=fopen(argv[4], "w"))==NULL)
      MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    for (i=0; i<C_size; ++i) {
      buffer=secure_malloc(sizeof(*buffer)*n_g[i]);
      if (i!=0)
	MPI_Recv(buffer, n_g[i], MPI_FFTW_COMPLEX, i, 0, MPI_COMM_WORLD, 
		 &Status);
      else
  	memcpy(buffer, data, sizeof(*data)*n_g[i]);
      for (j=0; j<n_g[i]; ++j)
	fprintf(fd, "%g\t%g\n", buffer[j].re, buffer[j].im);
      free(buffer);
    }
    fclose(fd);
  } else
    MPI_Send(data, n2, MPI_FFTW_COMPLEX, 0, 0, MPI_COMM_WORLD);
  /* Speicher wieder frei geben */
  free(n_g);   free(start_g); 
  free(data);  free(work);  
  fftw_mpi_destroy_plan(plan);
  MPI_Finalize(); 
  return EXIT_SUCCESS;
}
