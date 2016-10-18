/* Beispiel-Programm aus dem Buch  */
/* »Cluster Computing« von         */
/* Heiko Bauke und Stephan Mertens */
/*                                 */
/* Springer 2005                   */
/* ISBN 3-540-42299-4              */


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "graph.h"
#include "master_worker.h"
#include "mpi.h"

unsigned long n_colorings=0;
master_t master;
worker_t worker;
MPI_Datatype col_array;

double Rand(void) { return (double)rand()/(double)RAND_MAX; } 

void count_colorings(unsigned *cols, unsigned nodes) {
  unsigned i;
  for (i=0; i<nodes; ++i)
    printf("%d ", cols[i]);
  printf("\n");
  ++n_colorings;
}

void error(int rank, const char *message) {
  fprintf(stderr, "Rang %i: %s\n", rank, message);
  MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
}

void send_result(unsigned *cols, unsigned nodes) {
  worker_send_result(worker, cols);
}

void master_loop(unsigned *cols, unsigned nodes) {
  int w_rank, command;
  unsigned *cols2;
  while (1) {
    if (w_rank=master_listen(master, &command))
      switch (command) {
	case MW_ask_for_job:
	  if (cols!=NULL) 
	    master_send_work(master, w_rank, cols);
	  else
	    master_suspend_worker(master, w_rank);
	  return;	    
	case MW_return_result:
	  if ((cols2=malloc(sizeof(*cols2)*nodes))==NULL)
	    error(0, "Speicher voll");
	  master_get_result(master, w_rank, cols2);
	  printf("Rang %i: ", w_rank);
	  count_colorings(cols2, nodes);
	  free(cols2);
	  break;
	case MW_job_done:
	  master_free_worker(master, w_rank);
      }
    usleep(1);
  }
}

int main(int argc, char *argv[]) {
  graph_t G=NULL;
  unsigned *cols, depth=0, i, nodes=0, ncols=0;
  double p=0.0;
  char c;
  int rank, size, w_rank, command;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (size<2) /* brauche mindestens 2 Prozesse */
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  if (rank==0) { /* Master */
    srand(time(NULL));
    while ((c=getopt(argc, argv, "G:p:n:k:d:"))!=-1) {
      switch (c) {
	case 'G':
	  G=graph_read(optarg);
	  nodes=G->nodes;
	  break;
	case 'p':
	  sscanf(optarg, "%lf", &p);
	  break;
	case 'n':
	  sscanf(optarg, "%u", &nodes);
	  break;
	case 'k':
	  sscanf(optarg, "%u", &ncols);
	  break;
	case 'd':
	  sscanf(optarg, "%u", &depth);
      }
      if (nodes>0 && p>0.0 && p<1.0 && G==NULL)
	G=graph_rand(nodes, p, Rand);
    }
    if (G==NULL || ncols<3 || depth==0 || depth>=G->nodes) 
      error(rank, "Falsche Parameter oder Speicher voll");
  }
  MPI_Bcast(&nodes, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
  if (rank!=0) /* Worker */
    if ((G=graph_new(nodes))==NULL)
      error(rank, "Speicher voll");
  MPI_Bcast(G->A, G->nodes*G->nodes, MPI_CHAR, 0, MPI_COMM_WORLD);
  MPI_Bcast(&ncols, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
  MPI_Bcast(&depth, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
  if ((cols=malloc(sizeof(*cols)*G->nodes))==NULL) 
    error(rank, "Speicher voll");
  MPI_Type_contiguous(G->nodes, MPI_UNSIGNED, &col_array); 
  MPI_Type_commit(&col_array); 
  if (rank==0) { /* Master */
    master=master_new(size-1, MPI_COMM_WORLD, col_array, col_array);
    if (master==NULL)
      error(rank, "Speicher voll");
    graph_coloring(G, ncols, cols, 0, depth, master_loop);
    while (master_some_working(master)) {
      master_loop(NULL, G->nodes);
    }
    printf("Anzahl der gefundenen Colorierungen: %ld\n", n_colorings);
    master_suspend_all_workers(master);
    master_free(master);
  } else { /* Worker */
    worker=worker_new(MPI_COMM_WORLD, col_array, col_array);
    if (worker==NULL)
      error(rank, "Speicher voll");
    while (worker_get_work(worker, cols)) {
      graph_coloring(G, ncols, cols, depth, G->nodes, send_result);
      worker_done(worker);
    }
    worker_free(worker);
  }
  free(cols); 
  graph_free(G);
  MPI_Finalize();
  return EXIT_SUCCESS;
}
