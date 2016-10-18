/* Beispiel-Programm aus dem Buch  */
/* »Cluster Computing« von         */
/* Heiko Bauke und Stephan Mertens */
/*                                 */
/* Springer 2005                   */
/* ISBN 3-540-42299-4              */


#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "mpi.h"

#define BUFSIZE 100000
#define DELAY 5

void * secure_malloc(size_t size) {
  void * p=malloc(size);
  if (p==NULL) {
    fprintf(stderr, "Speicher voll.\n");
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }
  return p;
}

int main(int argc, char *argv[]) { 

  int myrank, nprocs, c;
  char *buf;
  MPI_Request req;
  MPI_Status status;
  time_t T0, T1; 
  long bufsize;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

  if (nprocs<2) {
    fprintf(stderr, "Benötige mindestens zwei Prozesse.\n");
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }
  
  bufsize=BUFSIZE;
  while ((c=getopt(argc, argv, "b:")) != -1) {
    switch (c) {
      case 'b':
	bufsize=atol(optarg);
	break;
    }
  }
  
  buf=secure_malloc(bufsize);
  
  if (myrank==0) {
    printf("Teste asynchronen Nachrichtentransport\n");
    printf("======================================\n\n");
    printf("Einen Augenblick Geduld bitte ...\n\n");
  }
  MPI_Barrier(MPI_COMM_WORLD);    
  
  if (myrank==0) {
    /* time(&T0); */
    /* printf("Sender: MPI_Issend um %s", ctime(&T0)); */
    MPI_Issend(buf, bufsize, MPI_CHAR, 1, 99, MPI_COMM_WORLD, &req);
    sleep(DELAY);
    /* time(&T1); */
    /* printf("Sender: MPI_Wait um   %s", ctime(&T1)); */
    MPI_Wait(&req, &status);
  } else if (myrank==1) {
    time(&T0);
    /* printf("Empfänger: startet MPI_Recv um %s", ctime(&T0)); */
    MPI_Recv(buf, bufsize, MPI_CHAR, 0, 99, MPI_COMM_WORLD, &status);
    time(&T1);
    /* printf("Empfänger:  Paket empfangen um %s", ctime(&T1)); */
    printf("Für Pakete der Größe %ld wird asynchroner Transport %sunterstützt.\n", 
	   bufsize, T1-T0>=DELAY ? "NICHT ": "");
    if (argc<2)
      printf("\nProbieren Sie andere Paketgrößen mit der Option -b <size>\n");
  }
  free(buf);
  
  MPI_Finalize();
  return EXIT_SUCCESS;
}
