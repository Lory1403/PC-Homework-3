#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <math.h>

int main(int argc, char **argv) {

    // Initialize MPI
    int MPI_rank, MPI_size;
    int matrix_size;
    double wt1, wt2;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &MPI_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &MPI_size);

    if (MPI_rank == 0)
    {
        // Handle arguments
        if (argc != 2) {
            if (MPI_rank == 0)
                fprintf(stderr, "Error - not enough arguments\n"
                                "Usage: %s <Size>\n", argv[0]);

            MPI_Finalize();
            return -1;
        }

        if (sscanf(argv[1], "%i", &matrix_size) != 1) {
            if (MPI_rank == 0)
                fprintf(stderr, "Error - <Row size> not an integer\n"
                                "Usage: %s <Size> <Block size>\n", argv[0]);

            MPI_Finalize();
            return -1;
        }

        // Check compatibility between number of processors and matrix size
        if (matrix_size % (int) sqrt(MPI_size) != 0 || sqrt(MPI_size) - (int)sqrt(MPI_size) != 0) {
            if (MPI_rank == 0)
                printf("Error - Incompatible number of MPI processes and matrix dimensions.\n");

            MPI_Finalize();
            return 1;
        }
    }

    MPI_Bcast(&matrix_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Variables
    const int BLOCKS_PER_SIDE = matrix_size / sqrt(MPI_size);
    const int BLOCK_SIZE = matrix_size / BLOCKS_PER_SIDE;

    int *matrix = (int *) malloc(matrix_size * matrix_size * sizeof(int));
    int *local_block = (int *) malloc(BLOCK_SIZE * BLOCK_SIZE * sizeof(int));
    int *transposed_local_block = (int *) malloc(BLOCK_SIZE * BLOCK_SIZE * sizeof(int));
    int *transposed_matrix = (int *) malloc(matrix_size * matrix_size * sizeof(int));

    // Fill matrix
    if (MPI_rank == 0) {
        for (int i = 0; i < matrix_size * matrix_size; i++) {
            matrix[i] = i;
        }
    }

    // Define new MPI types
    MPI_Datatype block_type;
    MPI_Datatype tmp;
    MPI_Type_vector(BLOCK_SIZE, BLOCK_SIZE, matrix_size, MPI_INT, &tmp);
    MPI_Type_create_resized(tmp, 0, sizeof(int), &block_type);
    MPI_Type_commit(&block_type);

    // Compute values for MPI_Scatterv
    int disps[BLOCKS_PER_SIDE * BLOCKS_PER_SIDE];
    int counts[BLOCKS_PER_SIDE * BLOCKS_PER_SIDE];
    for (int i = 0; i < BLOCKS_PER_SIDE; i++) {
        for (int j = 0; j < BLOCKS_PER_SIDE; j++) {
            disps[i * BLOCKS_PER_SIDE + j] = i * matrix_size * BLOCK_SIZE + j * BLOCK_SIZE;
            counts[i * BLOCKS_PER_SIDE + j] = 1;
        }
    }

    // Distribute blocks among the processors
    MPI_Scatterv(matrix, counts, disps, block_type, local_block, BLOCK_SIZE * BLOCK_SIZE, MPI_INT, 0, MPI_COMM_WORLD);

    // TODO: delete me
    /* each proc prints it's "local_block" out, in order */
    for (int proc = 0; proc < MPI_size; proc++) {
        if (proc == MPI_rank) {
            printf("Rank = %d\n", MPI_rank);
            if (MPI_rank == 0) {
                printf("Global matrix: \n");
                for (int ii = 0; ii < matrix_size; ii++) {
                    printf("\t");
                    for (int jj = 0; jj < matrix_size; jj++) {
                        printf("%3d ", (int) matrix[ii * matrix_size + jj]);
                    }
                    printf("\n");
                }
            }
            printf("Local block:\n");
            for (int ii = 0; ii < BLOCK_SIZE; ii++) {
                printf("\t");
                for (int jj = 0; jj < BLOCK_SIZE; jj++) {
                    printf("%3d ", (int) local_block[ii * BLOCK_SIZE + jj]);
                }
                printf("\n");
            }
            printf("\n");
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
    // end of TODO

    // Transpose local blocks
    for (int i = 0; i < BLOCK_SIZE; i++) {
        for (int j = 0; j < BLOCK_SIZE; j++) {
            transposed_local_block[i * BLOCK_SIZE + j] = local_block[j * BLOCK_SIZE + i];
        }
    }

    // TODO: delete me
    /* each proc prints it's "local_block" out, in order */
    for (int proc = 0; proc < MPI_size; proc++) {
        if (proc == MPI_rank) {
            printf("Rank = %d\n", MPI_rank);
            printf("Transposed Local block:\n");
            for (int ii = 0; ii < BLOCK_SIZE; ii++) {
                printf("\t");
                for (int jj = 0; jj < BLOCK_SIZE; jj++) {
                    printf("%3d ", (int) transposed_local_block[ii * BLOCK_SIZE + jj]);
                }
                printf("\n");
            }
            printf("\n");
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
    // end of TODO
    // Send transposed matrixes to the main matrix in the correct order
    for (int i = 0; i < BLOCKS_PER_SIDE; i++) {
        for (int j = 0; j < BLOCKS_PER_SIDE; j++) {
            disps[i * BLOCKS_PER_SIDE + j] = i * BLOCK_SIZE + j * BLOCK_SIZE * matrix_size;
            counts[i * BLOCKS_PER_SIDE + j] = 1;
        }
    }
    MPI_Gatherv(transposed_local_block, BLOCK_SIZE * BLOCK_SIZE, MPI_INT,
                transposed_matrix, counts, disps, block_type,
                0, MPI_COMM_WORLD);

    // TODO: delete me
    if (MPI_rank == 0) {
        printf("Transposed matrix: \n");
        for (int ii = 0; ii < matrix_size; ii++) {
            printf("\t");
            for (int jj = 0; jj < matrix_size; jj++) {
                printf("%3d ", (int) transposed_matrix[ii * matrix_size + jj]);
            }
            printf("\n");
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    // end of TODO

    free(matrix);
    free(local_block);
    free(transposed_local_block);
    free(transposed_matrix);

    MPI_Finalize();
    return 0;
}