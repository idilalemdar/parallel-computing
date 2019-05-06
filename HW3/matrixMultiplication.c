//
// Created by cydonia on 05.05.2019.
//

#include <stdio.h>
//#include <mpi.h>
#include <math.h>
#define N 3200

double frobeniusNorm(double **matrix){
    double result = 0;
    for (int i = 0; i < N; ++i) {
        int row = i * N;
        for (int j = 0; j < N; ++j) {
            result += pow(*(matrix + row + j), 2);
        }
    }
    return sqrt(result);
}

double matrixMultiplication(int nproc, int rank, double matrix[N][N]){
    return 0;  // returns MPI_Wtime()
}

void populateMatrixB(double **matrixB){
    for (int i = 0; i < N; ++i) {
        double i1 = i + 1;
        int row = i * N;
        for (int j = 0; j < N; ++j) {
            (matrixB + row + j) = i1 - (j + 1)/3200.0;
        }
    }
}

void initMatrix(double **matrix){
    matrix = (double **)calloc(N, sizeof(double*));
    for (int i = 0; i < N; ++i) {
        matrix[i] = (double*)calloc(N, sizeof(double));
    }
}

void freeMatrix(double **matrix){
    for (int i = 0; i < N; ++i) {
        free(matrix[i]);
    }
    free(matrix);
}

int main(){
    double **resultMatrix;
    double **matrixB;
    initMatrix(resultMatrix);
    initMatrix(matrixB);
    populateMatrixB(matrixB);
    for (int i = 0; i < 3; ++i) {
        printf("%f\n", matrixB[0][i]);
    }
/*
    MPI_Init(NULL, NULL);

    int nproc;
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    double timeElapsed = matrixMultiplication(nproc, rank, resultMatrix);
    double norm = frobeniusNorm(resultMatrix);
    printf("Time elapsed:%f Frobenius Norm: %f\n", timeElapsed, norm);

    MPI_Finalize();
*/
    freeMatrix(resultMatrix);
    freeMatrix(matrixB);
    return 0;
}