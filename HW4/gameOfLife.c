//
// Created by cydonia on 24.05.2019.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <string.h>
#define SIZE 500
#define INIT_COORS 57

typedef struct Result {
    int cellCount;
    double timeElapsed;
} result;

typedef struct Coordinate {
    int row;
    int column;
} coordinate;

coordinate coors[INIT_COORS] = {{3,13}, {3,14},
                      {4,12}, {4,16}, {4,24}, {4,25},{4,35}, {4,36},
                      {5,11}, {5,17}, {5,23}, {5,25}, {5,29}, {5,30}, {5,31},{5,35}, {5,36},
                      {6,1}, {6,2}, {6,11}, {6,15}, {6,17},{6,18},{6,23}, {6,24}, {6,29},
                      {7,1}, {7,2}, {7,6}, {7,11}, {7,17}, {7,30},
                      {8,6}, {8,12}, {8,16},
                      {9,13}, {9,14}, {9,19}, {9,20},
                      {10,19}, {10,21}, {10,34}, {10,35}, {10,36},
                      {11,19}, {11,33}, {11,34}, {11,35},
                      {12,2}, {12,3},
                      {13,1}, {13,4},
                      {14,2}, {14,3},
                      {15,6}, {15,7}, {15,8}};

void initialize(int* matrix){
    int row, column;
    for (int i = 0; i < INIT_COORS; ++i) {
        row = coors[i].row;
        column = coors[i].column;
        *(matrix + row * SIZE + column) = 1;
    }
}

int countNeighbors(int* matrix, int r, int c){
    int count = 0;
    int row, column;
    coordinate neighborCoordinates[8] = {{r-1,c-1}, {r-1,c}, {r-1,c+1}, {r,c-1}, {r,c+1}, {r+1,c-1}, {r+1,c}, {r+1,c+1}};
    for (int i = 0; i < 8; ++i) {
        row = neighborCoordinates[i].row;
        column =neighborCoordinates[i].column;
        if ((row >= 0 && row < SIZE) && (column >= 0 && column < SIZE)){
            count += *(matrix + row * SIZE + column);
        }
    }
    return count;
}


int move(int* old, int* new, int nproc, int rank){
    int count = 0;

    int* newSource = (int*)calloc(SIZE * SIZE, sizeof(int));

    int processPerDimension = (int)sqrt(nproc);
    int partition = SIZE / processPerDimension;

    int border1 = rank / processPerDimension;
    int border2 = rank % processPerDimension;

    int rowStart = border1 * partition;
    int rowEnd = rowStart + partition;
    int columnStart = border2 * partition;
    int columnEnd = columnStart + partition;

    for (int i = rowStart; i < rowEnd; ++i) {
        int iSize = i * SIZE;
        for (int j = columnStart; j < columnEnd; ++j) {
            int neighborCount = countNeighbors(old, i , j);
            if (*(old + iSize + j) == 1) {  // live cell
                if (neighborCount == 2 || neighborCount == 3) { // goes on living
                    *(newSource + iSize + j) = 1;
                    count++;
                } // else, dies
            } else { // dead cell
                if (neighborCount == 3){ // reproduction
                    *(newSource + iSize + j) = 1;
                    count++;
                }
            }
        }
    }
    MPI_Allreduce(newSource, new, SIZE*SIZE, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    free(newSource);
    return count;
}

void clearOld(int* matrix){
    int iterCount = SIZE * SIZE;
    for (int i = 0; i < iterCount; ++i) {
        *(matrix + i) = 0;
    }
}

result play(int* matrix, int steps, int nproc, int rank){
    int* temp;
    int* old = matrix;
    int* buffer = (int*)calloc(SIZE * SIZE, sizeof(int));
    int* new = buffer;
    int count;
    int totalCount;
    double start = MPI_Wtime();
    for (int i = 0; i < steps; ++i) {
        count = move(old, new, nproc, rank);
        clearOld(old);
        temp = new;
        new = old;
        old = temp;
    }
    MPI_Allreduce(&count, &totalCount, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    double end = MPI_Wtime();
    free(buffer);

    result res = {totalCount, end - start};
    return res;
}

int main(int argc, char* argv[]){
    int* matrix = (int*)calloc(SIZE * SIZE, sizeof(int));
    initialize(matrix);

    MPI_Init(&argc, &argv);
    int steps = atoi(argv[1]);

    int nproc;
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    result res = play(matrix, steps, nproc, rank);

    printf("NumberOfFilledCells:%d, TotalTimeConsumed: %f\n", res.cellCount, res.timeElapsed);

    MPI_Finalize();
    free(matrix);
    return 0;
}