// Beispiel-Programm aus dem Buch  
// »Cluster Computing« von         
// Heiko Bauke und Stephan Mertens 
//                                 
// Springer 2005                   
// ISBN 3-540-42299-4              


#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <trng.h>
#include <unistd.h>
#include "mpi.h"

// Bewegungsrichtungen
typedef enum { north=0, south, east, west } compass_t;

// ein Punkt in der Ebene
struct point_t { int x, y; };

int generate_walk(int N, int n) {
  const compass_t allowd_dirs[4][3]=
    { {north, east,  west}, {south, east,  west}, 
      {north, south, east}, {north, south, west} };
  TRNG::LCG64 R;  // Zufallszahlengenerator 
  // Zufallszahlengenerator um n*N Schritte vorstellen
  R.jump(static_cast<long long>(n)*static_cast<long long>(N));  
  point_t r={0, 0};                 // aktuelle Koordinate
  point_t *visited=new point_t[N];  // speichert alle besuchten Punkte
  compass_t dir=static_cast<compass_t>(R(4));  // Richtung für 1. Schritt
  for (int i=0; i<N; ++i) {
    switch (dir) {
      case north:
        ++r.y;  break;
      case south:
        --r.y;  break;
      case east:
        ++r.x;  break;
      case west:
        --r.x;  break;
    }
    visited[i]=r;  // neue Koordinate merken
    // neue Koordinate schon einmal besucht?
    for (int j=0; j<i; ++j) 
      if (visited[j].x==visited[i].x && visited[j].y==visited[i].y) {
	delete[] visited; // ja, dann abbrechen
	return -1;
      }
    dir=allowd_dirs[dir][R(3)]; // Richtung für nächsten Schritt
  }
  delete[] visited;
  return r.x*r.x+r.y*r.y;
}

int main(int argc, char *argv[]) {
  int size, rank;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Prozesse in MPI_COMM_WORLD
  MPI_Comm_size(MPI_COMM_WORLD, &size);  // lokaler Prozessrang
  int length(20), n_walks(1000), start(0), c;
  // Weglänge und Anzahl der Wege einlesen
  while ((c=getopt(argc, argv, "N:n:s:"))!=-1) {
    switch (c) {
      case 'N':
        length=std::atoi(optarg);  break;
      case 'n':
        n_walks=std::atoi(optarg);  break;
      case 's':
        start=std::atoi(optarg);  break;
      default:
	// unbekannter Parameter, Programmabbruch
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE); 
    }
  }
  // jeden size-ten Weg erzeugen 
  for (int i(rank); i<n_walks; i+=size) {
    int r2(generate_walk(length, i+start));
    // falls Weg selbstvermeidend Abstandsquadrat ausgeben
    if (r2>=0)
      std::cout << "Rang: " << rank
		<< "\tWeg Nr. " << i+start 
		<< "\tAbstandsquadrat: " << r2 << '\n';
  }
  MPI_Finalize();
  return EXIT_SUCCESS;
}
