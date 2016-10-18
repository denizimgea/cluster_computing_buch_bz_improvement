/* Beispiel-Programm aus dem Buch  */
/* »Cluster Computing« von         */
/* Heiko Bauke und Stephan Mertens */
/*                                 */
/* Springer 2005                   */
/* ISBN 3-540-42299-4              */


#include <stdlib.h>
#include <unistd.h>
#include "master_worker.h"

master_t master_new(int num_workers, MPI_Comm comm,
                    MPI_Datatype job_dt, MPI_Datatype result_dt) {
  int i;
  master_t m=malloc(sizeof(*m));
  if (m!=NULL) { /* genug Speicher vorhanden */
    m->num_working_workers=0;
    m->num_workers=num_workers;
    m->comm=comm;
    m->job_dt=job_dt;
    m->result_dt=result_dt;
    m->working=malloc(sizeof(*m->working)*num_workers);
    if (m->working!=NULL) /* genug Speicher vorhanden */
      for (i=0; i<num_workers; ++i)
	m->working[i]=MW_worker_free;
    else { /* nicht genug Speicher vorhanden */ 
      free(m);
      m=NULL;
    }
  }
  return m;
}

void master_free(master_t m) {
  if (m!=NULL) {
    free(m->working);
    free(m);
  }
}

int master_some_working(master_t m) {
   return (m->num_working_workers)>0;
}

int master_listen(master_t m, int *command) {
  MPI_Status status;
  int flag;
  MPI_Iprobe(MPI_ANY_SOURCE, MW_listen_tag, m->comm, &flag, &status);
  if (flag) {
    MPI_Recv(command, 1, MPI_INT, status.MPI_SOURCE, MW_listen_tag, 
	     m->comm, &status);
    return status.MPI_SOURCE;
  }
  return 0;
}

void master_send_work(master_t m, int w_rank, void *data) {
  MPI_Send(data, 1, m->job_dt, w_rank, MW_send_job_data_tag, m->comm);
  if (m->working[w_rank-1]==MW_worker_free) {
    m->working[w_rank-1]=MW_worker_working;
    ++(m->num_working_workers);
  }
}

void master_get_result(master_t m, int w_rank, void *data) {
  MPI_Status status;
  MPI_Recv(data, 1, m->result_dt, w_rank, MW_recv_result_data_tag, 
	   m->comm, &status);
}

void master_free_worker(master_t m, int w_rank) {
  if (m->working[w_rank-1]==MW_worker_working) 
    --(m->num_working_workers);
  m->working[w_rank-1]=MW_worker_free;
}

void master_suspend_worker(master_t m, int w_rank) {
  MPI_Send(NULL, 0, m->job_dt, w_rank, MW_send_job_data_tag, 
	   m->comm);
  if (m->working[w_rank-1]!=MW_worker_suspended)
    --(m->num_workers);
  m->working[w_rank-1]=MW_worker_suspended;
}

void master_suspend_all_workers(master_t m) {
  int w_rank, command;
  while (m->num_workers>0) {
    if (w_rank=master_listen(m, &command))
      if (command==MW_ask_for_job)
	master_suspend_worker(m, w_rank);
      else
	MPI_Abort(m->comm, EXIT_FAILURE);
    usleep(1);
  }
}

worker_t worker_new(MPI_Comm comm,
                    MPI_Datatype job_dt, MPI_Datatype result_dt) {
  worker_t w=malloc(sizeof(*w));
  if (w!=NULL) { /* genug Speicher vorhanden */
    w->comm=comm;
    w->job_dt=job_dt;
    w->result_dt=result_dt;
  }
  return w;
}

void worker_free(worker_t w) {
  free(w);
}

int worker_get_work(worker_t w, void *data) {
  int command=MW_ask_for_job, count;
  MPI_Status status;
  MPI_Send(&command, 1, MPI_INT, 0, MW_listen_tag, w->comm);
  MPI_Recv(data, 1, w->job_dt, 0, MW_send_job_data_tag, 
	   w->comm, &status);
  MPI_Get_count(&status, MPI_INT, &count);
  return count>0;
}

void worker_send_result(worker_t w, void *data) {
  int command=MW_return_result;
  MPI_Send(&command, 1, MPI_INT, 0, MW_listen_tag, w->comm);
  MPI_Send(data, 1, w->result_dt, 0, MW_recv_result_data_tag, 
	   w->comm);
}

void worker_done(worker_t w) {
  int command=MW_job_done;
  MPI_Send(&command, 1, MPI_INT, 0, MW_listen_tag, w->comm);
}
