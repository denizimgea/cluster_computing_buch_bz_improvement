/* Beispiel-Programm aus dem Buch  */
/* »Cluster Computing« von         */
/* Heiko Bauke und Stephan Mertens */
/*                                 */
/* Springer 2005                   */
/* ISBN 3-540-42299-4              */


#if !(defined MASTER_WORKER_H)

#define MASTER_WORKER_H
#include "mpi.h"

/* Verwaltungsstruktur für Master-Prozess */
typedef struct { 
  int num_workers, num_working_workers, *working;
  MPI_Comm comm;
  MPI_Datatype job_dt, result_dt;
} master_str;
typedef master_str * master_t;

/* Verwaltungsstruktur für Worker-Prozess */
typedef struct {
  MPI_Comm comm;
  MPI_Datatype job_dt, result_dt;
} worker_str;
typedef worker_str * worker_t;

/* Worker-Kommandos */
enum { MW_ask_for_job, MW_return_result, MW_job_done };
/* Kennzeichnungen für verschiedene Nachrichten */
enum { MW_listen_tag, MW_send_job_data_tag, MW_recv_result_data_tag };
/* Zustände eines Workers */
enum { MW_worker_free, MW_worker_working, MW_worker_suspended };

/* Master-Funktionen */
master_t master_new(int num_workers, MPI_Comm comm,
                    MPI_Datatype job_dt, MPI_Datatype result_dt);
void master_free(master_t m);
int master_some_working(master_t m);
int master_listen(master_t m, int *command);
void master_send_work(master_t m, int w, void *data);
void master_get_result(master_t m, int w, void *data);
void master_free_worker(master_t m, int w);
void master_suspend_worker(master_t m, int w);
void master_suspend_all_workers(master_t m);

/* Worker-Funktionen */
worker_t worker_new(MPI_Comm comm,
                    MPI_Datatype job_dt, MPI_Datatype result_dt);
void worker_free(worker_t w);
int worker_get_work(worker_t w, void *data);
void worker_send_result(worker_t w, void *data);
void worker_done(worker_t w);

#endif
