#include <stdio.h>
#include <mpi.h>

int i, rank, size;
float** A, TA;

void memoryAllocation()




void matT()
{
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);    

    if (rank == 0) {


        

        // Memory deallocation
        for (i = 0; i < N; i++) {
            free(A[i]);
        }
        free(A);
    }

    MPI_Finalize();
}

void matBlockT()
{
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);    

    if (rank == 0) {
        A = (float**)malloc(sizeof(float*)*N);
        for (i = 0; i < N; i++) {
            A[i] = (float*)malloc(sizeof(float)*N);
        }
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                A[i][j] = i + j;
            }
        }
    }

    MPI_Finalize();
}

int main(int argc, char *argv[])
{
    srand(time(NULL));

    matT();
    matBlockT();

    return 0;
}
