/* Beispiel-Programm aus dem Buch  */
/* »Cluster Computing« von         */
/* Heiko Bauke und Stephan Mertens */
/*                                 */
/* Springer 2005                   */
/* ISBN 3-540-42299-4              */


#if !(defined GRAPH_H)

#define GRAPH_H

typedef struct {
  unsigned nodes;
  char *A;
} graph_str;
typedef graph_str * graph_t;

graph_t graph_new(unsigned nodes);
graph_t graph_read(const char *fname);
graph_t graph_rand(unsigned nodes, double p, double (*R)(void));
void graph_free(graph_t G);
void graph_insert_edge(graph_t G, unsigned i, unsigned j);
void graph_del_edge(graph_t G, unsigned i, unsigned j);
int graph_has_edge(graph_t G, unsigned i, unsigned j);
void graph_coloring(graph_t G, unsigned ncols, unsigned *cols, 
		    unsigned depth, unsigned max_depth,
		    void (*call)(unsigned *, unsigned));

#endif
