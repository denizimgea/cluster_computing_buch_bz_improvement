/* Beispiel-Programm aus dem Buch  */
/* »Cluster Computing« von         */
/* Heiko Bauke und Stephan Mertens */
/*                                 */
/* Springer 2005                   */
/* ISBN 3-540-42299-4              */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

double pintegral(double (*f)(double), double a, double b, 
		 long n, int k, int tasks); /* siehe pintegral.c */

double f(double x) {
  return sqrt(1.0-x*x);
}

int main (int argc, char *argv[]) {
  double result, x, T;
  long n=1000000;

  if (argc==2) 
    n=atol(argv[1]);
  T=(double)clock()/(double)CLOCKS_PER_SEC;
  result=pintegral(f, 0.0, 1.0, n, 0, 1);
  T=(double)clock()/(double)CLOCKS_PER_SEC-T;
  printf("%ld Stuetzst., %d Proz. Zeit %6.4g Sek, Ergebnis %15.12g\n", 
	 n, 1, T, result);
  return EXIT_SUCCESS;
}
