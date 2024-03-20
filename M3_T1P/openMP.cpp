#include <iostream>
#include <cstdlib>
#include <ctime>
#include <mpi.h>
#include <omp.h>
#include "matrix_operations.h"

using namespace std;

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int rows1 = 3, cols1 = 3, cols2 = 3;

    // Allocate memory for matrices in all processes
    int **mat1 = new int*[rows1];
    int **mat2 = new int*[cols1];
    int **result = new int*[rows1];
    for (int i = 0; i < rows1; i++) {
        mat1[i] = new int[cols1];
        result[i] = new int[cols2];
    }
    for (int i = 0; i < cols1; i++) {
        mat2[i] = new int[cols2];
    }

    // Generate random values for matrices only on root process (rank 0)
    if (rank == 0) {
        srand(time(0)); // Seed for random number generation
        for (int i = 0; i < rows1; i++) {
            for (int j = 0; j < cols1; j++) {
                mat1[i][j] = rand() % 10; // Random values between 0 and 9
            }
        }
        for (int i = 0; i < cols1; i++) {
            for (int j = 0; j < cols2; j++) {
                mat2[i][j] = rand() % 10; // Random values between 0 and 9
            }
        }
    }

    // Broadcast matrix data to all processes
    for (int i = 0; i < rows1; i++) {
        MPI_Bcast(mat1[i], cols1, MPI_INT, 0, MPI_COMM_WORLD);
    }
    for (int i = 0; i < cols1; i++) {
        MPI_Bcast(mat2[i], cols2, MPI_INT, 0, MPI_COMM_WORLD);
    }

    // Measure execution time
    double startTime = MPI_Wtime();

    // Perform matrix multiplication on each process
    matrixMultiplication(mat1, mat2, result, rows1, cols1, cols2);

    // Measure execution time
    double endTime = MPI_Wtime();
    double elapsedTime = endTime - startTime;

    // Print the execution time on root process (rank 0)
    if (rank == 0) {
        cout << "Matrix multiplication execution time: " << elapsedTime << " seconds" << endl;
    }

    // Gather the results
    // (omitting gathering code for simplicity)

    // Print the matrices and the result on root process (rank 0)
    if (rank == 0) {
        cout << "Matrix 1:" << endl;
        printMatrix(mat1, rows1, cols1);

        cout << "Matrix 2:" << endl;
        printMatrix(mat2, cols1, cols2);

        cout << "Result Matrix:" << endl;
        printMatrix(result, rows1, cols2);
    }

    // Deallocate memory
    for (int i = 0; i < rows1; i++) {
        delete[] mat1[i];
        delete[] result[i];
    }
    delete[] mat1;
    delete[] result;
    for (int i = 0; i < cols1; i++) {
        delete[] mat2[i];
    }
    delete[] mat2;

    MPI_Finalize();
    return 0;
}


// matrix_operations.h
#ifndef MATRIX_OPERATIONS_H
#define MATRIX_OPERATIONS_H

void matrixMultiplication(int **mat1, int **mat2, int **result, int rows1, int cols1, int cols2);
void printMatrix(int **matrix, int rows, int cols);

#endif

//matrix_operations.cpp
#include "matrix_operations.h"
#include <iostream>

void matrixMultiplication(int **mat1, int **mat2, int **result, int rows1, int cols1, int cols2) {
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < rows1; i++) {
        for (int j = 0; j < cols2; j++) {
            result[i][j] = 0;
            for (int k = 0; k < cols1; k++) {
                result[i][j] += mat1[i][k] * mat2[k][j];
            }
        }
    }
}

void printMatrix(int **matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
}
