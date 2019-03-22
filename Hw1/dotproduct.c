//
// Created by cydonia on 22.03.2019.
//

#include <mpi.h>
#include <stdio.h>
#define VEC_SIZE 10000000
#define ROOT 0

double calculateDotProduct(int startIndex, int endIndex){
    double result = 0;
    int mod;
    for (int i = startIndex; i < endIndex; ++i) {
        mod = i % 20;
        result += (2.0 - mod * 0.1)
                * (0.1 + mod * 0.1);
    }
    return result;
}

int main(int argc, char **argv){

    int nproc, rank;
    double sum, overallSum;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int volume = VEC_SIZE / nproc;
    sum = calculateDotProduct(rank * volume, (rank + 1) * volume);
    MPI_Reduce(&sum, &overallSum, 1, MPI_DOUBLE, MPI_SUM, ROOT, MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}
