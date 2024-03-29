#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <OpenCL/opencl.h>
#include <mpi.h>

#define ARRAY_SIZE 100000

// Function to perform quicksort on an array
void quickSort(int *arr, int left, int right) {
    int i = left, j = right;
    int tmp;
    int pivot = arr[(left + right) / 2]; // Selecting pivot element

    // Partitioning the array
    while (i <= j) {
        while (arr[i] < pivot)
            i++;
        while (arr[j] > pivot)
            j--;
        if (i <= j) {
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
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Allocate memory for the array
    int *arr = (int *)malloc(ARRAY_SIZE * sizeof(int));

    // Generate random numbers for the array
    srand(time(NULL));
    for (int i = 0; i < ARRAY_SIZE; i++) {
        arr[i] = rand() % 1000;
    }

    // OpenCL initialization
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;

    cl_int err;
    err = clGetPlatformIDs(1, &platform, NULL);
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    queue = clCreateCommandQueue(context, device, 0, &err);

    // Define the kernel source code for quicksort
    const char *kernelSource = "__kernel void quickSort(__global int *arr, int left, int right) { /* OpenCL quick sort implementation */ }";

    // Create and build the OpenCL program
    program = clCreateProgramWithSource(context, 1, &kernelSource, NULL, &err);
    clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    kernel = clCreateKernel(program, "quickSort", &err);

    // Create buffer for the array
    cl_mem arrBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE, ARRAY_SIZE * sizeof(int), NULL, &err);
    clEnqueueWriteBuffer(queue, arrBuffer, CL_TRUE, 0, ARRAY_SIZE * sizeof(int), arr, 0, NULL, NULL);

    // Set kernel arguments
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &arrBuffer);
    clSetKernelArg(kernel, 1, sizeof(int), &rank);
    clSetKernelArg(kernel, 2, sizeof(int), &size);

    // Execute the kernel
    size_t globalSize = ARRAY_SIZE;
    clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalSize, NULL, 0, NULL, NULL);

    // Read back the sorted array
    clEnqueueReadBuffer(queue, arrBuffer, CL_TRUE, 0, ARRAY_SIZE * sizeof(int), arr, 0, NULL, NULL);

    // Measure execution time
    double start_time, end_time;
    start_time = MPI_Wtime();

    // Perform quicksort on the CPU
    quickSort(arr, 0, ARRAY_SIZE - 1);

    end_time = MPI_Wtime();

    double execution_time = end_time - start_time;

    // Print execution time
    printf("\n");
    printf("Execution time for process %d: %f seconds\n", rank, execution_time);

    // Release OpenCL resources
    clReleaseMemObject(arrBuffer);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    // Finalize MPI
    MPI_Finalize();
    
    // Free allocated memory
    free(arr);
    return 0;
}
