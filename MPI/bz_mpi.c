/* Beispiel-Programm aus dem Buch  */
/* »Cluster Computing« von         */
/* Heiko Bauke und Stephan Mertens */
/*                                 */
/* Springer 2005                   */
/* ISBN 3-540-42299-4              */


#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"

const int n = 255, k1 = 3, k2 = 3, g = 41, iteration_count = 1000;
int matrix_width = 400, matrix_height = 300;
enum { gather_tag, sendrecv_tag };

void *secure_malloc(size_t size) {
  void *p = malloc(size);
  if (p == NULL)
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  return p;
}

void bz_iterate(int *X, int *X_new, int matrix_width, int matrix_height,
		int n, int k1, int k2, int g);

int main(int argc, char *argv[]) {
  // Declare variables
  int C_rank, C_size, grid_dimensions[2] = {0, 0}, P_periods[2] = {1, 1}, process_coordinates[2],
    *merged_matrix = NULL, *process_matrix, *updated_process_matrix, *matrix_replace_helper, process_matrix_width,
    process_matrix_height, send_offsets[4], receive_offsets[4], sender_ranks[4], receiver_ranks[4];
  MPI_Comm comm;
  MPI_Datatype submatrix_t, innermatrix_t, row_t, col_t, types[4];
  MPI_Request request;
  MPI_Status status;

  // Initialize MPI
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &C_size);

  // Create two-dimensional cartesian communicator to divide up the grid
  // Create a two-dimensional layout and save it in the array grid_dimensions
  MPI_Dims_create(C_size, 2, grid_dimensions);
  // Create the cartesian grid communicator according to the dimension data in the array grid_dimension,
  // save it in the variable comm.
  MPI_Cart_create(MPI_COMM_WORLD, 2, grid_dimensions, P_periods, 1, &comm);
  // Save the rank of the process in the cartesian communicator comm.
  MPI_Comm_rank(comm, &C_rank);
  // Save the coordinates of the current process in the variable process_coordinates.
  MPI_Cart_coords(comm, C_rank, 2, process_coordinates);
  // Round down matrix_width and matrix_height to ensure each process has a matrix with the same size
  matrix_width -=matrix_width%grid_dimensions[0];    process_matrix_width=matrix_width/grid_dimensions[0];
  matrix_height -=matrix_height%grid_dimensions[1];    process_matrix_height=matrix_height/grid_dimensions[1];

  // Create MPI data types for the matrices, row vectors and column vectors.
  MPI_Type_vector(1, process_matrix_width + 2, process_matrix_width + 2, MPI_INT, &row_t);
  MPI_Type_commit(&row_t); 
  MPI_Type_vector(process_matrix_height, 1, process_matrix_width+2, MPI_INT, &col_t);
  MPI_Type_commit(&col_t); 
  MPI_Type_vector(process_matrix_height, process_matrix_width, process_matrix_width+2, MPI_INT, &innermatrix_t);
  MPI_Type_commit(&innermatrix_t);
  MPI_Type_vector(process_matrix_height, process_matrix_width, matrix_width, MPI_INT, &submatrix_t);
  MPI_Type_commit(&submatrix_t);

  // Allocate memory for the process matrices and fill process matrix with randomized values.
  process_matrix = secure_malloc((process_matrix_width + 2) * (process_matrix_height + 2) * sizeof(int));
  process_matrix += (process_matrix_width + 2) + 1;
  updated_process_matrix = secure_malloc((process_matrix_width + 2) * (process_matrix_height + 2) * sizeof(int));
  updated_process_matrix += (process_matrix_width + 2) + 1;
  srand(C_rank); 
  for (int j = 0; j < process_matrix_height; ++j)
    for (int i = 0; i < process_matrix_width; ++i)
      process_matrix[i + (process_matrix_width + 2) * j] = rand() % n;

  // Preparation of the data for the vector exchange.
  int k = 0;
  for (int j = 0; j <= 1; ++j)
    for (int i = -1; i <= 1; i += 2, ++k) {
        types[k] = j == 0 ? col_t : row_t;
      MPI_Cart_shift(comm, j, i, receiver_ranks + k, sender_ranks + k);
    }
  send_offsets[0] = 0;
  receive_offsets[0] = process_matrix_width;
  send_offsets[1] = process_matrix_width - 1;
  receive_offsets[1] = -1;
  send_offsets[2] = -1;
  receive_offsets[2] = (process_matrix_width + 2) * process_matrix_height - 1;
  send_offsets[3] = (process_matrix_width + 2) * (process_matrix_height - 1) - 1;
  receive_offsets[3] = (-1) * (process_matrix_width + 3);

  // Iterate the cellular automaton. Exchange vectors with neighboring processes after each iteration.
  for (int t = 0; t < iteration_count; ++t) {
    for (int i = 0; i < 4; ++i)
      MPI_Sendrecv(process_matrix+send_offsets[i], 1, types[i], sender_ranks[i], sendrecv_tag,
		   process_matrix+receive_offsets[i], 1, types[i], receiver_ranks[i], sendrecv_tag,
		   comm, &status);
    bz_iterate(process_matrix, updated_process_matrix, process_matrix_width,
               process_matrix_height, n, k1, k2, g);
    matrix_replace_helper = process_matrix;
    process_matrix = updated_process_matrix;
    updated_process_matrix = matrix_replace_helper;
  }

  // Collect data from each process. Output it in PGM format.
  MPI_Isend(process_matrix, 1, innermatrix_t, 0, gather_tag, comm, &request);
  if (C_rank == 0) {
    merged_matrix = secure_malloc(matrix_width * matrix_height * sizeof(int));
    for (process_coordinates[1] = 0; process_coordinates[1] < grid_dimensions[1]; ++process_coordinates[1])
      for (process_coordinates[0] = 0; process_coordinates[0] < grid_dimensions[0]; ++process_coordinates[0]) {
        MPI_Cart_rank(comm, process_coordinates, &C_rank);
        MPI_Recv(merged_matrix + process_coordinates[0] * process_matrix_width + process_coordinates[1]
                 * matrix_width * process_matrix_height, 1, submatrix_t, C_rank, gather_tag, comm, &status);
      }
    printf("P5\n%i %i\n%i\n", matrix_width, matrix_height, n);  /* PGM-Header */
    for (int i = 0; i < matrix_width * matrix_height; ++i)
      fputc(merged_matrix[i], stdout);
  }

  // Free resources and end program.
  MPI_Wait(&request, &status);
  MPI_Type_free(&submatrix_t);
  MPI_Type_free(&innermatrix_t);
  MPI_Comm_free(&comm);
  MPI_Finalize();
  return EXIT_SUCCESS;
}
