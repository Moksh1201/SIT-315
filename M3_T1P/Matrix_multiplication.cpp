#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <mpi.h>

using namespace std;

void multiplyMatrices(int **A, int **B, int **C, int N) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            C[i][j] = 0;
            for (int k = 0; k < N; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int N = 4;

    // Allocate memory for matrices A, B, and C
    int **A = new int *[N];
    int **B = new int *[N];
    int **C = new int *[N];
    for (int i = 0; i < N; ++i) {
        A[i] = new int[N];
        B[i] = new int[N];
        C[i] = new int[N];
    }

    srand(time(0) + rank); // Add rank to ensure different seed for each process
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            A[i][j] = rand() % 100;
            B[i][j] = rand() % 100;
        }
    }

    // Scatter matrix A and B among processes
    MPI_Bcast(&(A[0][0]), N * N, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&(B[0][0]), N * N, MPI_INT, 0, MPI_COMM_WORLD);

    // Allocate memory for the partial result matrix
    int **partialC = new int *[N];
    for (int i = 0; i < N; ++i) {
        partialC[i] = new int[N];
    }

    // Each process performs partial matrix multiplication
    multiplyMatrices(A, B, partialC, N);

    // Gather partial results to root process
    MPI_Gather(&(partialC[0][0]), N * N / size, MPI_INT, &(C[0][0]), N * N / size, MPI_INT, 0, MPI_COMM_WORLD);

    // Output results from root process
    if (rank == 0) {
        ofstream outputFile("output.txt");
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                outputFile << C[i][j] << " ";
            }
            outputFile << endl;
        }
        outputFile.close();
    }

    // Deallocate memory
    for (int i = 0; i < N; ++i) {
        delete[] A[i];
        delete[] B[i];
        delete[] C[i];
        delete[] partialC[i];
    }
    delete[] A;
    delete[] B;
    delete[] C;
    delete[] partialC;

    MPI_Finalize();

    return 0;
}
