//
// Created by cydonia on 05.05.2019.
//

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#define N 3200

double frobeniusNorm(double *matrix){
    double result = 0;
    for (int i = 0; i < N; ++i) {
        int row = i * N;
        for (int j = 0; j < N; ++j) {
            result += pow(*(matrix + row + j), 2);
        }
    }
    return sqrt(result);
}

double matrixMultiplication(int nproc, int rank, double *resultMatrix, double* matrixB){
    int batch = N / nproc;
    int start = rank * batch;
    int end = (rank + 1) * batch;
    int m;
    double startTime = MPI_Wtime();
    for (int i = start; i < end; ++i) {
        m = i * N;
        for (int j = 0; j < N; ++j) {
            m += j;
            for (int k = 0; k < N; ++k) {
                resultMatrix[m] += ((i + 1.0)/(k + 1.0)) * (*(matrixB + k * N + j));
            }
        }
    }
    double endTime = MPI_Wtime();
    return endTime - startTime;
}

void populateMatrixB(double *matrixB){
    for (int i = 0; i < N; ++i) {
        double i1 = i + 1;
        int row = i * N;
        for (int j = 0; j < N; ++j) {
            *(matrixB + row + j) = i1 - (j + 1)/3200.0;
        }
    }
}

int main(){
    double *resultMatrix;
    double *matrixB;
    resultMatrix = (double *)calloc(N*N, sizeof(double));
    matrixB = (double *)calloc(N*N, sizeof(double));
    populateMatrixB(matrixB);

    MPI_Init(NULL, NULL);

    int nproc;
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    double timeElapsed = matrixMultiplication(nproc, rank, resultMatrix, matrixB);
    double reduce[N*N];
    MPI_Allreduce(resultMatrix, reduce, N*N, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    double norm = frobeniusNorm(reduce);
    printf("Time elapsed:%f Frobenius Norm: %f\n", timeElapsed, norm);

    MPI_Finalize();

    free(resultMatrix);
    free(matrixB);
    return 0;
}