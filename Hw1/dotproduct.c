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

void experiment(int nproc, int rank, int volume){
    double sum, overallSum;
    double startTime = MPI_Wtime();
    sum = calculateDotProduct(rank * volume, (rank + 1) * volume);
    MPI_Reduce(&sum, &overallSum, 1, MPI_DOUBLE, MPI_SUM, ROOT, MPI_COMM_WORLD);
    if (rank == ROOT){
        printf("Result: %f, TimeConsumed: %f\n", overallSum, MPI_Wtime() - startTime);
    }
}

int main(){
    int nproc, rank;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int volume = VEC_SIZE / nproc;
    for (int i = 0; i < 10 ; ++i) {
        experiment(nproc, rank, volume);
    }
    MPI_Finalize();
    return 0;
}
