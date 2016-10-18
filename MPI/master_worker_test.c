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
#include "master_worker.h" 

int count;

int job_pending(void) { return count<10; }

void get_job_data(int *data) { ++count; *data=count; }

void do_job(int *data, double *result) { *result=3.14*(*data); }

int main(int argc, char *argv[]) {
  int rank, size, w_rank, command, job_data;
  double result_data;
  master_t master;
  worker_t worker;

  /* initialsiere MPI */
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (size<2) /* brauche mindestens 2 Prozesse */
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  
  if (rank==0) { /* Master */
    /* alloziere Speicher für Master-Prozesses mit size-1 Workern */
    master=master_new(size-1, MPI_COMM_WORLD, MPI_INT, MPI_DOUBLE);
    if (master==NULL)
      MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    count=0; 
    /* bearbeite Hauptschleife solange noch Jobs vorliegen oder 
       Worker beschäftigt sind und beantworte Anfragen der Worker */
    while (job_pending() || master_some_working(master)) {
      if (w_rank=master_listen(master, &command)) 
	switch (command) {
	  case MW_ask_for_job:
	    if (job_pending()) {
	      get_job_data(&job_data);
	      printf("%d nach %d gesendet\n", job_data, w_rank);
	      master_send_work(master, w_rank, &job_data);
	    } else {
	      printf("beende worker %d\n", w_rank);
	      master_suspend_worker(master, w_rank);
	    }
	    break;
	  case MW_return_result:
	    master_get_result(master, w_rank, &result_data);
	    printf("%g von %d empfangen\n", result_data, w_rank);
	    break;
	  case MW_job_done: 
	    master_free_worker(master, w_rank);
	    break;
	}
      usleep(1);  /* schlafen legen */
    }
    master_suspend_all_workers(master); 
    master_free(master);
  } else { /* Worker */
    worker=worker_new(MPI_COMM_WORLD, MPI_INT, MPI_DOUBLE);
    if (worker==NULL)
      MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    while (worker_get_work(worker, &job_data)) {
      do_job(&job_data, &result_data);
      worker_send_result(worker, &result_data);
      worker_done(worker);
    }
    worker_free(worker);
  }

  MPI_Finalize(); /* MPI beenden */
  return EXIT_SUCCESS;
}
