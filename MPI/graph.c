/* Beispiel-Programm aus dem Buch  */
/* »Cluster Computing« von         */
/* Heiko Bauke und Stephan Mertens */
/*                                 */
/* Springer 2005                   */
/* ISBN 3-540-42299-4              */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "graph.h"

graph_t graph_new(unsigned nodes) {
  unsigned i, j;
  graph_t G;
  G=malloc(sizeof(*G));
  if (G!=NULL) {
    G->nodes=nodes;
    G->A=malloc(sizeof(*G->A)*nodes*nodes);
    if (G->A!=NULL) {
      for (i=0; i<G->nodes; ++i)
	for (j=0; j<G->nodes; ++j) 
	  G->A[i*nodes+j]=0;
    } else {
      free(G);
      G=NULL;
    }
  }
  return G;
}

graph_t graph_read(const char *fname) {
  unsigned nodes, edges, i, j, k;
  graph_t G;
  G=NULL;
  FILE *fd;
  fd=fopen(fname, "r");
  if (fd!=NULL) {
    if (fscanf(fd, "%u%u", &nodes, &edges)==2) {
      G=graph_new(nodes);
      if (G!=NULL) 
	for (k=0; k<edges; ++k) {
	  fscanf(fd, "%u%u", &i, &j);
	  if (i<G->nodes && j<G->nodes) 
	    G->A[i*G->nodes+j]=G->A[j*G->nodes+i]=1;
	}
    }
    fclose(fd);
  }
  return G;
}

graph_t graph_rand(unsigned nodes, double p, double (*R)(void)) {
  unsigned i, j;
  graph_t G;
  G=malloc(sizeof(*G));
  if (G!=NULL) {
    G->nodes=nodes;
    G->A=malloc(sizeof(*G->A)*nodes*nodes);
    if (G->A!=NULL) {
      memset(G->A, 0, G->nodes*G->nodes*sizeof(*(G->A)));
      for (i=0; i<G->nodes; ++i)
	for (j=i+1; j<G->nodes; ++j) 
	  if (R()<p) 
	    G->A[i*G->nodes+j]=G->A[j*G->nodes+i]=1;
    } else {
      free(G);
      G=NULL;
    }
  }
  return G;
}

void graph_free(graph_t G) {
  free(G->A);
  free(G);
}

void graph_insert_edge(graph_t G, unsigned i, unsigned j) {
  G->A[i*G->nodes+j]=G->A[j*G->nodes+i]=1;
}

void graph_del_edge(graph_t G, unsigned i, unsigned j) {
  G->A[i*G->nodes+j]=G->A[j*G->nodes+i]=0;
}

int graph_has_edge(graph_t G, unsigned i, unsigned j) {
  return G->A[i*G->nodes+j]>0;
}

static graph_t graph_coloring_G;
static unsigned *graph_coloring_cols, graph_coloring_max_depth, 
  graph_coloring_ncols;
static void (*graph_coloring_call)(unsigned *, unsigned);

static void graph_coloring_rec(unsigned depth) {
  unsigned i, j, coloring_ok;
  for (i=0; i<graph_coloring_ncols; ++i) {
    graph_coloring_cols[depth]=i;
    coloring_ok=1;
    for (j=0; j<depth && coloring_ok; ++j)
      if (graph_has_edge(graph_coloring_G, depth, j) && 
	  graph_coloring_cols[depth]==graph_coloring_cols[j])
	coloring_ok=0;
    if (coloring_ok)
      if (depth+1<graph_coloring_max_depth)
	graph_coloring_rec(depth+1);
      else 
	graph_coloring_call(graph_coloring_cols, graph_coloring_G->nodes);
  }
}

void graph_coloring(graph_t G, unsigned ncols, unsigned *cols, 
		    unsigned depth, unsigned max_depth,
		    void (*call)(unsigned *, unsigned)) {
  graph_coloring_G=G;
  graph_coloring_ncols=ncols;
  graph_coloring_cols=cols;
  graph_coloring_max_depth=max_depth;  
  graph_coloring_call=call;
  graph_coloring_rec(depth);
}
