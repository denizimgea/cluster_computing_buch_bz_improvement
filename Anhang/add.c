/* Beispiel-Programm aus dem Buch  */
/* »Cluster Computing« von         */
/* Heiko Bauke und Stephan Mertens */
/*                                 */
/* Springer 2005                   */
/* ISBN 3-540-42299-4              */


#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  double sum;
  int i;

  sum=0.0;
  for (i=1; i<argc; ++i) 
    sum+=atof(argv[i]);
  printf("sum is %g\n", sum);
  return EXIT_SUCCESS;
}
