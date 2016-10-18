// Beispiel-Programm aus dem Buch  
// »Cluster Computing« von         
// Heiko Bauke und Stephan Mertens 
//                                 
// Springer 2005                   
// ISBN 3-540-42299-4              


#include <iostream>
#include <cstdlib>
#include "mpi.h"

int main(int argc, char *argv[]) {
  char name[MPI::MAX_PROCESSOR_NAME+1];

  MPI::Init(argc, argv);
  int nprocs=MPI::COMM_WORLD.Get_size();
  int myrank=MPI::COMM_WORLD.Get_rank();
  int len;
  MPI::Get_processor_name(name, len) ;
  name[len]='\0';
  std::cout << "Prozess " << myrank << " (von " << nprocs 
	    << ") läuft auf " << name << ".\n";
  MPI::Finalize();
  return EXIT_SUCCESS;
}
