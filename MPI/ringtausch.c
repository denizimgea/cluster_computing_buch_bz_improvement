/* Beispiel-Programm aus dem Buch  */
/* »Cluster Computing« von         */
/* Heiko Bauke und Stephan Mertens */
/*                                 */
/* Springer 2005                   */
/* ISBN 3-540-42299-4              */


/***********************************************************************
 * Time-stamp: <Thursday, 05.05.2005, 12:54:36; edited by mertens>
 *
 * ringtausch.c
 *
 * Messung von MPI Paketlaufzeiten im Ringtausch-Szenario
 *
 * Transfer wahlweise mit nichtblockierendem (default) oder
 * persistentem send/receive.
 *
 * Aufruf (via mpirun): pinpong [-M samples] [-d size] [-p] [-B] [-e]
 *
 *   -B: Ausgabe der Bandbreite (default: Laufzeit)
 *   -p: persistentes send/recv (default: nichtblockierend)
 *   -e: mit Ausgabe einer Fehlerschranke (errorbars)
 *   size: maximale Paketgroesse ist 2^(size-1) Bytes (default: 2^19)
 *   samples:  Anzahl der Messungen pro Paketgroesse (default: 1000)
 *
 * (C) stephan.mertens@physik.uni-magdeburg.de
 * 
 ***********************************************************************/

#include <math.h>
#include <mpi.h>    
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>       /* fuer getopt */

#define PING 1

int main (int argc, char *argv[])
{
  int myrank;  
  int numprocs;
  int src, dest;
  MPI_Status status, stats[2];
  MPI_Request req[2];
  unsigned long numsamples; /* Anzahl Messungen pro Paketgroesse */
  unsigned long packetsize; /* Paketgroesse */
  unsigned int dmax;        /* Verdopplungschritte Paketgroesse */
  unsigned long k, size, d;
  double t, t_ave, tt_ave;  
  char *x1, *x2, c;
  int  i, persistent, bandbreite, fehler; 

  MPI_Init (&argc,&argv);

  MPI_Comm_rank (MPI_COMM_WORLD, &myrank); 
  MPI_Comm_size (MPI_COMM_WORLD, &numprocs); 

  src = (myrank-1) % numprocs;
  dest = (myrank+1) % numprocs;

  /* Defaultwerte */
  numsamples = 1000;
  dmax = 20;
  persistent = 0;
  bandbreite = 0;
  fehler = 0;

  /* Kommandozeilen-Argumente */
  while ((c = getopt (argc, argv, "M:d:pBe")) != -1)
  {
    switch (c)
    {
    case 'M':
      numsamples = atol (optarg);
      break;
    case 'd':
      dmax = atoi (optarg);
      break;
    case 'p':
      persistent = 1;
      break;
    case 'B':
      bandbreite = 1;
      break;
    case 'e':
      fehler = 1;
      break;
    }
  }

  if (myrank == 0) {
    printf ("# MPI-Paketlaufzeiten: Ping-Pong Messung\n");
    printf ("# beteiligte Prozesse: %d \n", numprocs); 
    printf ("# Methode: ");
    if (persistent) printf ("persistentes Send/Recv\n");
    else printf ("nichtblockierendes Send/Recv\n");
    printf ("# Zahl der Messungen: %u fuer jede Paketgroesse\n", numsamples);
    printf ("# maximale Paketgroesse: %u Bytes \n",sizeof(char)*(1ul << (dmax-1))); 
    printf ("# Format: Paketgroesse [bytes]  ");
    if (bandbreite) {
      printf("Bandbreite [Mbit/s]  ");
      if (fehler) printf("Standarabw. [Mbit/s]\n");
      else printf("\n");
    }
    else {
      printf("Laufzeit [ms]   ");
      if (fehler) printf("Standardabw. [ms]\n");
      else printf("\n");
    }
  }

  for (packetsize = 1, d = 0; d < dmax; packetsize *= 2, d++) {
    size =  sizeof(char)*packetsize; 
    x1 = (char*) malloc (size);
    x2 = (char*) malloc (size);
    t_ave = 0.0;
    tt_ave = 0.0;
    MPI_Barrier (MPI_COMM_WORLD);
    if (persistent) {
      MPI_Recv_init (x1, packetsize, MPI_CHAR, src, PING, MPI_COMM_WORLD, req);
      MPI_Send_init (x2, packetsize, MPI_CHAR, dest, PING, MPI_COMM_WORLD, req+1);
      for (k = 0; k < numsamples; k++) {
	t = MPI_Wtime ();
	for (i = 0; i < numprocs; i++) {
	  MPI_Start (req);
	  MPI_Start (req+1);
	  MPI_Waitall (2, req, stats);
	}
	t = MPI_Wtime () - t;
	t = t/numprocs;
	if (bandbreite) {
	  t = ((double)size)/(t*1024*128); /* Mbit/s */
	}
	else t *= 1000.0; /* Millisekunden */
	t_ave += t;
	tt_ave += (t*t);
      }
      MPI_Request_free (req);
      MPI_Request_free (req+1);
    }
    else {
      for (k = 0; k < numsamples; k++) {
	t = MPI_Wtime ();
	for (i = 0; i < numprocs; i++) {
	  MPI_Irecv (x1, packetsize, MPI_CHAR, src, PING, MPI_COMM_WORLD, req);
	  MPI_Isend (x2, packetsize, MPI_CHAR, dest, PING, MPI_COMM_WORLD, req+1);
	  MPI_Waitall (2, req, stats);
	}
	t = MPI_Wtime () - t;
	t = t/numprocs;
	if (bandbreite) {
	  t = ((double)size)/(t*1024*128);
	}
	else t *= 1000.0; /* Millisekunden */
	t_ave += t;
	tt_ave += (t*t);
      }
    }
    t_ave = t_ave/numsamples;
    tt_ave = tt_ave/numsamples;
    if (myrank == 0) {
      if (fehler) {
	printf ("%10u  %20.10lf  %20.10lf \n", size, t_ave, 
		sqrt(tt_ave - t_ave*t_ave)/sqrt((double)numsamples));
      }
      else printf ("%10u  %20.10lf\n", size, t_ave);
      fflush (stdout);
    }
    free (x2); free (x1);
  }

  MPI_Finalize(); 
}
