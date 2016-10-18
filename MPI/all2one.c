/* Beispiel-Programm aus dem Buch  */
/* »Cluster Computing« von         */
/* Heiko Bauke und Stephan Mertens */
/*                                 */
/* Springer 2005                   */
/* ISBN 3-540-42299-4              */


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "mpi.h"   

int main(int argc, char *argv[]) {
  const int ping=1, pong=2;
  double t, t_ave, tt_ave;  
  int *x, c, namelength, d, packets, size,
    myrank,           /* Prozessrang */
    numprocs,         /* Anzahl der Prozesse */ 
    numpackets=1000,  /* Anzahl Pakete */
    packetsize=500;   /* Paketgroesse */
  MPI_Status status;
  char procname[MPI_MAX_PROCESSOR_NAME+1];
  
  MPI_Init(&argc, &argv);  /* Initialisierung MPI-System */
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank); 
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs); 
  MPI_Get_processor_name (procname, &namelength);
  procname[namelength]='\0';
  
  /* Kommandozeilen-Argumente */
  while ((c=getopt (argc, argv, "M:s:"))!=-1) {
    switch (c) {
      case 'M':
	sscanf(optarg, "%ud", &numpackets);
	break;
      case 's':
	sscanf(optarg, "%ud", &packetsize);
	break;
    }
  }

  size=sizeof(*x)*packetsize; 
  if ((x=malloc(size))==NULL)
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  
  MPI_Barrier(MPI_COMM_WORLD);
  if (myrank==0) {
    packets=0;
    d=numprocs-1;
    while (d>0) {
      MPI_Recv(x, packetsize, MPI_INT, MPI_ANY_SOURCE, ping, 
	       MPI_COMM_WORLD, &status);
      ++packets;
      x[0]=packets;
      MPI_Send(x, packetsize, MPI_INT, status.MPI_SOURCE, pong,
               MPI_COMM_WORLD);
      if (packets>=numpackets) 
	--d;
    }
    numpackets=packets;
    printf("Prozess 0 auf %s hat %u Pakete a %u Bytes ausgetauscht.\n",
	   procname, numpackets, size);
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
    while (1) {
      t=MPI_Wtime();
      MPI_Send(x, packetsize, MPI_INT, 0, ping, MPI_COMM_WORLD);
      MPI_Recv(x, packetsize, MPI_INT, 0, pong, MPI_COMM_WORLD,
	       &status);
      t=0.5*(MPI_Wtime()-t);
      t*=1000.0; /* Millisekunden */
      t_ave+=t;
      ++packets;
      if (x[0]>=numpackets)
	break;
    }
    t_ave=t_ave/((double)packets);
    MPI_Send(procname, namelength+1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    MPI_Send(&packets, 1, MPI_UNSIGNED_LONG, 0, 0, MPI_COMM_WORLD);
    MPI_Send(&t_ave, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
  }
  free(x);
  MPI_Finalize(); 
  return EXIT_SUCCESS;
}
