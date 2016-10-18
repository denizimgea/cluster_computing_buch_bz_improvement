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

unsigned long n_colorings=0;

double Rand(void) { return (double)rand()/(double)RAND_MAX; }

void count_colorings(unsigned *cols, unsigned nodes) {
  unsigned i;
  for (i=0; i<nodes; ++i) 
    printf("%d ", cols[i]);
  printf("\n");
  ++n_colorings;
}

int main(int argc, char *argv[]) {
  graph_t G=NULL;
  unsigned *cols, nodes=0, ncols=0;
  double p=0.0;
  int c;
  srand(time(NULL));
  while ((c=getopt(argc, argv, "G:p:n:k:"))!=-1) {
    switch (c) {
      case 'G':
	G=graph_read(optarg);
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
    }
    if (nodes>0 && p>0.0 && p<1.0 && G==NULL) 
      G=graph_rand(nodes, p, Rand);
  }
  if (G==NULL || ncols<3) {
    fprintf(stderr, "Falsche Parameter\n");
    return EXIT_FAILURE;
  }
  if ((cols=malloc(sizeof(*cols)*G->nodes))==NULL) {
    fprintf(stderr, "Speicher voll\n");
    return EXIT_FAILURE;
  }
  graph_coloring(G, ncols, cols, 0, G->nodes, count_colorings);
  printf("Anzahl der gefundenen Colorierungen: %ld\n", n_colorings);
  free(cols);
  graph_free(G);
  return EXIT_SUCCESS;
}
