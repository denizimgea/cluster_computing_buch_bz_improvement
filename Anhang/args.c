/* Beispiel-Programm aus dem Buch  */
/* »Cluster Computing« von         */
/* Heiko Bauke und Stephan Mertens */
/*                                 */
/* Springer 2005                   */
/* ISBN 3-540-42299-4              */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  /* getopt */

void print_help(const char *name, FILE *fp) {
  fprintf(fp, "Verwendung: %s [-T Zeit] [-N Größe] [-v] [-h]\n", name);
}

int main(int argc, char *argv[]) {
  long T, N;
  char verbose;
  int c;

  /* Standardwerte */
  T=1000;
  N=200;
  verbose=0;
  /* Kommandozeile auswerten */
  while ((c=getopt(argc, argv, "T:N:vh"))!=-1) {
    switch (c) {
      case 'T':
	T=atol(optarg);
	break;
      case 'N':
	N=atol(optarg);
	break;
      case 'v':
	verbose=1;
	break;
      case 'h':
	print_help(argv[0], stdout);
	exit(EXIT_SUCCESS);
      default:
	print_help(argv[0], stderr);
	exit(EXIT_FAILURE); 
    }
  }
  printf("Parameter:\n  T = %ld\n  N = %ld\n", T, N);
  printf("Ausgaben sind %sgeschaltet.\n", (verbose ? "ein" : "aus"));
  return EXIT_SUCCESS;
}
