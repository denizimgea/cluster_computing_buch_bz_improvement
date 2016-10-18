C Beispiel-Programm aus dem Buch  
C »Cluster Computing« von         
C Heiko Bauke und Stephan Mertens 
C                                 
C Springer 2005                   
C ISBN 3-540-42299-4              


      PROGRAM MPIIDENT
      IMPLICIT NONE
      INCLUDE "mpif.h"
      INTEGER MYRANK, NPROCS, LENGTH, ERROR
      CHARACTER(LEN=MPI_MAX_PROCESSOR_NAME) NAME

      CALL MPI_INIT(ERROR)
      CALL MPI_COMM_SIZE(MPI_COMM_WORLD, NPROCS, ERROR)
      CALL MPI_COMM_RANK(MPI_COMM_WORLD, MYRANK, ERROR)
      CALL MPI_GET_PROCESSOR_NAME(NAME, LENGTH, ERROR)
      WRITE (*, *) 'Prozess ', MYRANK, ' (von ', NPROCS,
     +     ') läuft auf ', NAME(1:LENGTH)
      CALL MPI_FINALIZE(ERROR)
      END
