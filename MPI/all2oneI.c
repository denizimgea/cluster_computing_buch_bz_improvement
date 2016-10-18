/* Beispiel-Programm aus dem Buch  */
/* »Cluster Computing« von         */
/* Heiko Bauke und Stephan Mertens */
/*                                 */
/* Springer 2005                   */
/* ISBN 3-540-42299-4              */


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>       /* für getopt */
#include "mpi.h"

int main (int argc, char *argv[]) {
  const int ping=1, pong=2, maxprocs=100;
  double t, t_ave, tt_ave;  
  int **xlist, *x, c, namelength, d, packets, i, j, *indices, received,
    myrank,          /* Prozessrang */
    numprocs,        /* Anzahl der Prozesse */ 
    numpackets=1000, /* Anzahl Pakete */
    packetsize=500;  /* Paketgröße */
  MPI_Status status, *statuses;
  MPI_Request *requests;
  char procname[MPI_MAX_PROCESSOR_NAME+1];

  MPI_Init(&argc,&argv);  /* Initialisierung MPI-System */
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank); 
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs); 
  MPI_Get_processor_name(procname, &namelength);
  procname[namelength]='\0';

  /* Kommandozeilen-Argumente */
  numpackets;
  packetsize;
  while ((c=getopt(argc, argv, "M:s:")) != -1) {
    switch (c) {
    case 'M':
        sscanf(optarg, "%ud", &numpackets);
        break;
      case 's':
        sscanf(optarg, "%ud", &packetsize);
        break;
    }
  }
  
  if (myrank==0) {
    indices=malloc(numprocs*sizeof(*indices));
    statuses=malloc(numprocs*sizeof(*statuses));
    requests=malloc(numprocs*sizeof(*requests));
    xlist=malloc(numprocs*sizeof(*xlist));
    for (i=0; i<numprocs; ++i) 
      xlist[i]=malloc(packetsize*sizeof(*xlist[i]));
  } else {
    x=malloc(packetsize*sizeof(*x));
    nice(19);
  }

  MPI_Barrier(MPI_COMM_WORLD);
  if (myrank==0) {
    packets=0;
    d=numprocs-1;
    for (i=0; i<d; ++i) 
      MPI_Irecv(xlist[i], packetsize, MPI_INT, i+1, 
		ping, MPI_COMM_WORLD, &requests[i]);
    while (d>0) {
      MPI_Waitsome(numprocs-1, requests, &received, indices, statuses);
      for (i=0; i<received; ++i) {
        j=indices[i];
	xlist[j][0]=++packets;
        MPI_Send(xlist[j], packetsize, MPI_INT, statuses[i].MPI_SOURCE, 
		 pong, MPI_COMM_WORLD);
        if (packets>numpackets-numprocs+1) 
	  --d;
	else 
	  MPI_Irecv(xlist[j], packetsize, MPI_INT, statuses[i].MPI_SOURCE,
		    ping, MPI_COMM_WORLD, &requests[j]);
      }
    }
    numpackets=packets;
    printf("Prozess 0 auf %s hat %u Pakete a %u Bytes ausgetauscht.\n",
	   procname, numpackets, sizeof(*x)*packetsize);
    printf("Daran waren beteiligt:\n");
    for (d=1; d<numprocs; ++d) {
      MPI_Recv(procname, MPI_MAX_PROCESSOR_NAME+1, MPI_CHAR, d, 0, 
	       MPI_COMM_WORLD, &status);
      MPI_Recv(&packets, 1, MPI_UNSIGNED_LONG, d, 0, MPI_COMM_WORLD, 
	       &status);
      MPI_Recv(&t_ave, 1, MPI_DOUBLE, d, 0, MPI_COMM_WORLD, &status);
      printf("   Prozess %d auf %s: %u Pakete, %lf ms pro Paket\n", 
	     d, procname, packets, t_ave); 
    }
  } else {
    packets=0;
    t_ave=0;
    do {
      t=MPI_Wtime();
      MPI_Send(x, packetsize, MPI_INT, 0, ping, MPI_COMM_WORLD);
      MPI_Recv(x, packetsize, MPI_INT, 0, pong, MPI_COMM_WORLD,
	       &status);
      t=0.5*(MPI_Wtime()-t)*1000.0;  /* Millisekunden */
      t_ave+=t;
      ++packets;
    } while (!(x[0]>numpackets-numprocs+1));
    t_ave=t_ave/((double)packets);
    MPI_Send(procname, namelength+1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    MPI_Send(&packets, 1, MPI_UNSIGNED_LONG, 0, 0, MPI_COMM_WORLD);
    MPI_Send(&t_ave, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
  }
  
  MPI_Finalize();
  if (myrank == 0) {
    for (i=0; i<numprocs; i++) free(xlist[i]);
    free(xlist);
    free(requests);
    free(statuses);
    free(indices);
  } else
    free(x);
  
  return EXIT_SUCCESS;
}
