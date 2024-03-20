#include <iostream>
#include <cstdlib>
#include <ctime>
#include <mpi.h>
#include <OpenCL/opencl.h>
#include <chrono>

using namespace std;

const char *kernelSource = 
"__kernel void matrixMultiplication(__global int* mat1, __global int* mat2, __global int* result, int rows1, int cols1, int cols2) {\n"
"    int row = get_global_id(0);\n"
"    int col = get_global_id(1);\n"
"    int sum = 0;\n"
"    for (int k = 0; k < cols1; k++) {\n"
"        sum += mat1[row * cols1 + k] * mat2[k * cols2 + col];\n"
"    }\n"
"    result[row * cols2 + col] = sum;\n"
"}";


void printMatrix(int *matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            cout << matrix[i * cols + j] << " ";
        }
        cout << endl;
    }
}

int main(int argc, char *argv[]) {
  
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int rows1 = 3, cols1 = 3, cols2 = 3;

    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;
    cl_mem bufferMat1, bufferMat2, bufferResult;

    // Initialize OpenCL
    clGetPlatformIDs(1, &platform, NULL);
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);
    queue = clCreateCommandQueue(context, device, 0, NULL);
    program = clCreateProgramWithSource(context, 1, &kernelSource, NULL, NULL);
    clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    kernel = clCreateKernel(program, "matrixMultiplication", NULL);

    // Allocate memory for matrices
    int *mat1 = new int[rows1 * cols1];
    int *mat2 = new int[cols1 * cols2];
    int *result = new int[rows1 * cols2];

    // Generate random values for matrices only on root process (rank 0)
    if (rank == 0) {
        srand(time(0));
        for (int i = 0; i < rows1 * cols1; i++) {
            mat1[i] = rand() % 10;
        }
        for (int i = 0; i < cols1 * cols2; i++) {
            mat2[i] = rand() % 10;
        }
    }
    //time

    std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
    start = std::chrono::high_resolution_clock::now();
    // Broadcast matrix data to all processes
    MPI_Bcast(mat1, rows1 * cols1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(mat2, cols1 * cols2, MPI_INT, 0, MPI_COMM_WORLD);

    // Create OpenCL buffers
    bufferMat1 = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * rows1 * cols1, mat1, NULL);
    bufferMat2 = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * cols1 * cols2, mat2, NULL);
    bufferResult = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(int) * rows1 * cols2, NULL, NULL);

    // Set kernel arguments
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufferMat1);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufferMat2);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufferResult);
    clSetKernelArg(kernel, 3, sizeof(int), &rows1);
    clSetKernelArg(kernel, 4, sizeof(int), &cols1);
    clSetKernelArg(kernel, 5, sizeof(int), &cols2);

    // Execute the kernel
    size_t globalSize[2] = { static_cast<size_t>(rows1), static_cast<size_t>(cols2) };
    clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalSize, NULL, 0, NULL, NULL);

    // Read the result back
    clEnqueueReadBuffer(queue, bufferResult, CL_TRUE, 0, sizeof(int) * rows1 * cols2, result, 0, NULL, NULL);

    //execution time
    end = std::chrono::high_resolution_clock::now();
    std::chrono::microseconds duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Execution time: " << duration.count() << " microseconds" << std::endl;
    

    // Print the matrices and the result 
    if (rank == 0) {
        cout << "Matrix 1:" << endl;
        printMatrix(mat1, rows1, cols1);

        cout << "Matrix 2:" << endl;
        printMatrix(mat2, cols1, cols2);

        cout << "Result Matrix:" << endl;
        printMatrix(result, rows1, cols2);
    }
   
    // Cleanup
    clReleaseMemObject(bufferMat1);
    clReleaseMemObject(bufferMat2);
    clReleaseMemObject(bufferResult);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    delete[] mat1;
    delete[] mat2;
    delete[] result;

    MPI_Finalize();
    return 0;
}
