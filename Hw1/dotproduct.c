//
// Created by cydonia on 22.03.2019.
//

#include <mpi.h>
#include <stdio.h>
#define VEC_SIZE 10000000
#define ROOT 0

double calculateDotProduct(int howMany){
    double result = 0;
    double vec1 = 2.0;
    double vec2 = 0.1;
    for (int i = 0; i < 10; ++i) {
        result += vec1 * vec2;
        vec1 -= 0.1;
        vec2 += 0.1;
    }
    result *= 2*howMany;
    return result;
}

int main(int argc, char **argv){

    int nproc, rank;
    double sum, overallSum;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int volume = VEC_SIZE / nproc;

    double startTime = MPI_Wtime();
    sum = calculateDotProduct(volume/20);
    MPI_Reduce(&sum, &overallSum, 1, MPI_DOUBLE, MPI_SUM, ROOT, MPI_COMM_WORLD);
    if (rank == ROOT) {
        printf("Result:%f TimeConsumed:%f\n", overallSum, MPI_Wtime() - startTime);
    }
    MPI_Finalize();
    return 0;
}
