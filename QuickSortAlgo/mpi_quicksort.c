#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define ARRAY_SIZE 100000

// Function to perform quicksort algorithm
void quickSort(int *arr, int left, int right) {
    int i = left, j = right;
    int tmp;
    // Selecting pivot element
    int pivot = arr[(left + right) / 2];

    // Partitioning the array
    while (i <= j) {
        while (arr[i] < pivot)
            i++;
        while (arr[j] > pivot)
            j--;
        if (i <= j) {
            // Swapping elements
            tmp = arr[i];
            arr[i] = arr[j];
            arr[j] = tmp;
            i++;
            j--;
        }
    }

    // Recursively sort sub-arrays
    if (left < j)
        quickSort(arr, left, j);
    if (i < right)
        quickSort(arr, i, right);
}

int main(int argc, char **argv) {
    int rank, size;
    // Initializing MPI environment
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Allocating memory for the array
    int *arr = (int *)malloc(ARRAY_SIZE * sizeof(int));

    // Generating random numbers for the array
    srand(time(NULL));
    for (int i = 0; i < ARRAY_SIZE; i++) {
        arr[i] = rand() % 1000;
    }

    double start_time, end_time;
    start_time = MPI_Wtime();

    // Sorting the array using quicksort
    quickSort(arr, 0, ARRAY_SIZE - 1);

    end_time = MPI_Wtime();

    double execution_time = end_time - start_time;

    // Printing sorted array and execution time
    // printf("Sorted array by process %d:\n", rank);
    // for (int i = 0; i < ARRAY_SIZE; i++) {
    //     printf("%d ", arr[i]);
    // }
    printf("\n");

    printf("Execution time for process %d: %f seconds\n", rank, execution_time);

    // Finalizing MPI environment and freeing allocated memory
    MPI_Finalize();
    free(arr);
    return 0;
}
