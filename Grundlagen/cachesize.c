/* Beispiel-Programm aus dem Buch  */
/* »Cluster Computing« von         */
/* Heiko Bauke und Stephan Mertens */
/*                                 */
/* Springer 2005                   */
/* ISBN 3-540-42299-4              */


/* Einfluss der Cache-Größe auf die Fliesskommaleistung
 * 
 * Aufruf: cachesize [-n maxsize] [-r repetitions]
 *
 *   maxsize     = maximale Vektorlaenge (double) / 1024 (default: 4096)
 *   repetitions = Zahl der Wiederholungen (default: 50)
 *
 * (C) Stephan Mertens
 * http://www.uni-magdeburg.de/mertens
 */

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>


double wallclocktime (void)
{
  struct timeval tp;
  struct timezone tzp;
  
  gettimeofday (&tp, &tzp);
  return (tp.tv_sec  + tp.tv_usec * 0.000001);
}


double sum (unsigned long k, double a[], double *result)
/* Laufzeit der Summe über a[0]..a[k-1] */
{
  unsigned long i;
  double res;
  double runtime;
  res = 0.0;
  runtime = wallclocktime ();
  for (i = 0; i < k; i++) res += a[i];
  runtime = wallclocktime ()- runtime;
  *result = res;
  return (runtime);
}


int main (int argc, char *argv[])
{
  unsigned long n, k;
  double *a, result;
  double runtime, mflops, mflops2;
  int c;  
  int rep, maxrep;
  
  n = 4096*1024;
  maxrep = 50;
  while ((c=getopt(argc, argv, "n:r:")) != -1) {
    switch (c) {
    case 'n':
      n = atol (optarg) * 1024;
      break;
    case 'r':
      maxrep = atoi (optarg);
      break;
    default:
      printf ("usage: %s [-n maxsize] [-r repetitions]\n", argv[0]);
      exit (-1);
    }
  }
  
  a = (double *)malloc (sizeof(double)*n);
  
  for (k = 0; k < n; k++) a[k] = 1.0;
  
  for (k = 1024; k <= n; k *= 2) {
    runtime = sum (k, a, &result);
    mflops2 = (1.0e-6*k)/runtime; 
    runtime = 0.0;
    for (rep = 0; rep < maxrep; rep++) runtime += sum (k, a, &result);
    runtime = runtime/rep;
    mflops = (1.0e-6*k)/runtime;
    printf ("%8lu kBytes: %6.2lf MFlop/s (%6.2lf MFlop/s), %2.8lf Sek.\n", 
       (sizeof(double)*k)/1024, mflops, mflops2, runtime); 
    /* Ausgabe zum Plotten: */
    /* printf ("%8lu %6.2lf\n", (sizeof(double)*k)/1024, mflops); */
  }

  free (a);
  return 0;
}
