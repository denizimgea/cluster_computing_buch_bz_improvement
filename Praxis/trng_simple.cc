// Beispiel-Programm aus dem Buch  
// »Cluster Computing« von         
// Heiko Bauke und Stephan Mertens 
//                                 
// Springer 2005                   
// ISBN 3-540-42299-4              


#include <cstdlib>
#include <iostream>
#include <trng.h>
#include "mpi.h"

int main(int argc, char *argv[]) {
  int rank, size;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Prozesse in MPI_COMM_WORLD
  MPI_Comm_size(MPI_COMM_WORLD, &size);  // lokaler Prozessrang  
  TRNG::MRG3 R;         // Zufallszahlengenerator
  R.split(size, rank);  // in size Folgen aufspalten und Nr. rank wählen
  double sum(0.0);
  for (int i(0); i<100; ++i)
    sum+=R.uniform();
  std::cout << "Rang : " << rank << "\tSumme : " << sum << "\n";
  MPI_Finalize();
  return EXIT_SUCCESS;
}
