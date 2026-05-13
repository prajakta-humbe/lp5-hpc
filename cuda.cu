// cuda_program.cu
%%writefile cuda_program.cu
#include <iostream>
#include <cuda.h>
#include <chrono>

using namespace std;
using namespace std::chrono;

// ================= VECTOR ADDITION =================

// CUDA Kernel for Vector Addition
__global__ void vectorAdd(int *A, int *B, int *C, int n)
{
    int i = blockIdx.x * blockDim.x + threadIdx.x;

    if (i < n)
    {
        C[i] = A[i] + B[i];
    }
}

// Sequential Vector Addition
void sequentialVectorAdd(int *A, int *B, int *C, int n)
{
    for (int i = 0; i < n; i++)
    {
        C[i] = A[i] + B[i];
    }
}

// ================= MATRIX MULTIPLICATION =================

// CUDA Kernel for Matrix Multiplication
__global__ void matrixMul(int *A, int *B, int *C, int N)
{
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    if (row < N && col < N)
    {
        int sum = 0;

        for (int k = 0; k < N; k++)
        {
            sum += A[row * N + k] * B[k * N + col];
        }

        C[row * N + col] = sum;
    }
}

// Sequential Matrix Multiplication
void sequentialMatrixMul(int *A, int *B, int *C, int N)
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            int sum = 0;

            for (int k = 0; k < N; k++)
            {
                sum += A[i * N + k] * B[k * N + j];
            }

            C[i * N + j] = sum;
        }
    }
}

// ================= MAIN FUNCTION =================

int main()
{
    // ================= VECTOR ADDITION =================

    int n = 100000;
    int size = n * sizeof(int);

    int *A = new int[n];
    int *B = new int[n];
    int *C_seq = new int[n];
    int *C_par = new int[n];

    // Initialize vectors
    for (int i = 0; i < n; i++)
    {
        A[i] = i;
        B[i] = i;
    }

    // Sequential Vector Addition Timing
    auto start = high_resolution_clock::now();

    sequentialVectorAdd(A, B, C_seq, n);

    auto end = high_resolution_clock::now();

    auto duration =
        duration_cast<microseconds>(end - start);

    cout << "\n===== VECTOR ADDITION =====\n";
    cout << "Sequential Time: "
         << duration.count()
         << " microseconds\n";

    // CUDA Memory Allocation
    int *d_A, *d_B, *d_C;

    cudaMalloc((void **)&d_A, size);
    cudaMalloc((void **)&d_B, size);
    cudaMalloc((void **)&d_C, size);

    // Copy data from Host to Device
    cudaMemcpy(d_A, A, size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, B, size, cudaMemcpyHostToDevice);

    // Parallel Vector Addition Timing
    start = high_resolution_clock::now();

    vectorAdd<<<(n + 255) / 256, 256>>>(d_A, d_B, d_C, n);

    cudaDeviceSynchronize();

    end = high_resolution_clock::now();

    duration =
        duration_cast<microseconds>(end - start);

    // Copy result back to Host
    cudaMemcpy(C_par, d_C, size, cudaMemcpyDeviceToHost);

    cout << "Parallel CUDA Time: "
         << duration.count()
         << " microseconds\n";

    // ================= MATRIX MULTIPLICATION =================

    int N = 200;
    int matrixSize = N * N * sizeof(int);

    int *M1 = new int[N * N];
    int *M2 = new int[N * N];
    int *M_seq = new int[N * N];
    int *M_par = new int[N * N];

    // Initialize matrices
    for (int i = 0; i < N * N; i++)
    {
        M1[i] = 1;
        M2[i] = 1;
    }

    // Sequential Matrix Multiplication Timing
    start = high_resolution_clock::now();

    sequentialMatrixMul(M1, M2, M_seq, N);

    end = high_resolution_clock::now();

    duration =
        duration_cast<microseconds>(end - start);

    cout << "\n===== MATRIX MULTIPLICATION =====\n";

    cout << "Sequential Time: "
         << duration.count()
         << " microseconds\n";

    // CUDA Memory Allocation
    int *d_M1, *d_M2, *d_M3;

    cudaMalloc((void **)&d_M1, matrixSize);
    cudaMalloc((void **)&d_M2, matrixSize);
    cudaMalloc((void **)&d_M3, matrixSize);

    // Copy matrices from Host to Device
    cudaMemcpy(d_M1, M1, matrixSize, cudaMemcpyHostToDevice);
    cudaMemcpy(d_M2, M2, matrixSize, cudaMemcpyHostToDevice);

    // Define Threads and Blocks
    dim3 threads(16, 16);
    dim3 blocks((N + 15) / 16, (N + 15) / 16);

    // Parallel Matrix Multiplication Timing
    start = high_resolution_clock::now();

    matrixMul<<<blocks, threads>>>(d_M1, d_M2, d_M3, N);

    cudaDeviceSynchronize();

    end = high_resolution_clock::now();

    duration =
        duration_cast<microseconds>(end - start);

    // Copy result back to Host
    cudaMemcpy(M_par, d_M3, matrixSize, cudaMemcpyDeviceToHost);

    cout << "Parallel CUDA Time: "
         << duration.count()
         << " microseconds\n";

    // ================= FREE MEMORY =================

    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);

    cudaFree(d_M1);
    cudaFree(d_M2);
    cudaFree(d_M3);

    delete[] A;
    delete[] B;
    delete[] C_seq;
    delete[] C_par;

    delete[] M1;
    delete[] M2;
    delete[] M_seq;
    delete[] M_par;

    return 0;
}

//!nvidia-smi

//!nvcc cuda_program.cu -o cuda_program

//!./cuda_program