/* Beispiel-Programm aus dem Buch  */
/* »Cluster Computing« von         */
/* Heiko Bauke und Stephan Mertens */
/*                                 */
/* Springer 2005                   */
/* ISBN 3-540-42299-4              */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <sys/time.h>
#define MAX_THREAD 100

double pintegral(double (*f)(double), double a, double b, 
		 long n, int k, int tasks); /* siehe pintegral.c */

long n;                           /* Zahl der Stützstellen */
int p;                            /* Zahl der aktiven Threads */
double resultat;                  /* numerischer Wert des Integrals */
pthread_mutex_t my_mutex;         /* schützt resultat */

double f(double x) { 
  return sqrt(1.0-x*x); 
}

void * calculate(void *parm) {
  int me=*((int *)parm);
  double x=pintegral(f, 0.0, 1.0, n, me, p);
  pthread_mutex_lock(&my_mutex);    /* Beginn seq. Abschnitt */
  resultat+=x;
  pthread_mutex_unlock(&my_mutex);  /* Ende seq. Abschnitt */
  return NULL;
}

int main(int argc, char *argv[]) {
  int thread_k[MAX_THREAD], k;
  double T;
  struct timeval tv;
  pthread_t thread_id[MAX_THREAD], tid;
  pthread_attr_t attr;

  p=1;  n=1000000;  /* Defaultwerte */
  if (argc>=2) n=atol(argv[1]);  
  if (argc>=3) p=atoi(argv[2]);  p=p>MAX_THREAD ? MAX_THREAD : p;
  pthread_attr_init(&attr);  /* Eigenschaften der Threads */
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
  gettimeofday(&tv, NULL);
  T=tv.tv_sec+tv.tv_usec/1000000.0;
  resultat=0.0; 
  pthread_mutex_init(&my_mutex, NULL);
  for (k=0; k<p; ++k) {  /* p Threads erzeugen */
    thread_k[k]=k;
    pthread_create(&tid, &attr, calculate, &thread_k[k]);
    thread_id[k]=tid;
  }  
  for (k=0; k<p; ++k)  /* warte, bis alle Threads beendet wurden */
    pthread_join(thread_id[k], NULL); 
  gettimeofday(&tv, NULL);
  T=tv.tv_sec+tv.tv_usec/1000000.0-T;
  printf("Int. = %15.12g: %d Threads, %6.4g Sek.\n", resultat, p, T);
  return EXIT_SUCCESS;
}
