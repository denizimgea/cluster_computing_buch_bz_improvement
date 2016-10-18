/* Beispiel-Programm aus dem Buch  */
/* »Cluster Computing« von         */
/* Heiko Bauke und Stephan Mertens */
/*                                 */
/* Springer 2005                   */
/* ISBN 3-540-42299-4              */


#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int main(int argc, char *argv[]) {
  int i, j, i_max=1;
  double sum, x;
  if (argc>1)
    i_max=atoi(argv[1]);
  for (i=0; i<i_max; ++i) {
    sum=0.0;
    for (j=0; j<1000000; ++j)
      sum+=sin((double)rand()/(double)RAND_MAX*3.141592654);
    printf("i = %i\tsum = %f\n", i, sum);
  }
  return EXIT_SUCCESS;
}
