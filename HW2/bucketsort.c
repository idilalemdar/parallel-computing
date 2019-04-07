//
// Created by cydonia on 05.04.2019.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

void quicksort(double a[], int n) {
    if (n <= 1) return;
    double p = a[n/2];
    double *b, *c;
    b=(double*) calloc(n,sizeof(double));
    c=(double*) calloc(n,sizeof(double));
    int i, j = 0, k = 0;
    for (i=0; i < n; i++) {
        if (i == n/2) continue;
        if ( a[i] <= p) b[j++] = a[i];
        else            c[k++] = a[i];
    }
    quicksort(b,j);
    quicksort(c,k);
    for (i=0; i<j; i++) a[i] = b[i];
    free(b);
    a[j] = p;
    for (i= 0; i<k; i++) a[j+1+i] = c[i];
    free(c);
}

int total(int counts[], int count){
    int result = 0;
    for (int i = 0; i < count; ++i) {
        result += counts[i];
    }
    return result;
}

int calculateStart(int rank, int totals[]){
    int sum = 0;
    for (int i = 0; i < rank; ++i) {
        sum += totals[i];
    }
    return sum;
}

double bucketsort(double arr[], int size, int nproc, int rank, int min, int max, double res_arr[]){
    int counts[nproc];
    for (int n = 0; n < nproc; ++n) {
        counts[n] = 0;
    }
    int partition = size / nproc;  // Assume size will be divisible by 32
    double *buckets = (double*)malloc(nproc * partition * sizeof(double));
    int start = partition * rank;
    int end = start + partition;
    double subrange = (max - min) / (nproc * 1.0) + min;
    double startTime = MPI_Wtime();
	for (int i = start; i < end; ++i) {
        double val = arr[i];
        int bucket = val / subrange;
        *(buckets + bucket*partition + counts[bucket]++) = val;
    }
    double *recvbuffer= (double*)malloc(nproc * partition * sizeof(double));
    MPI_Alltoall(buckets, partition, MPI_DOUBLE, recvbuffer, partition, MPI_DOUBLE, MPI_COMM_WORLD);
    int countrecv[nproc];
    MPI_Alltoall(counts, 1, MPI_INT, countrecv, 1, MPI_INT, MPI_COMM_WORLD);
    int totalcount = total(countrecv, nproc);
    double *results = (double *)malloc(totalcount * sizeof(double));
    int k = 0;
    for (int j = 0; j < nproc; ++j) {
        int bucketCount = countrecv[j];
        int row = j*partition;
        for (int i = 0; i < bucketCount; ++i) {
            results[k++] = *(recvbuffer + row + i);
        }
    }
    quicksort(results, totalcount);

    int totalrecv[nproc];
    MPI_Allgather(&totalcount, 1, MPI_INT, totalrecv, 1, MPI_INT, MPI_COMM_WORLD);
    int startIndex = calculateStart(rank, totalrecv);
    int endIndex = startIndex + totalcount;
    int m = 0;
    for (int l = startIndex; l < endIndex; ++l) {
        res_arr[l] = results[m++];
    }

    double endTime = MPI_Wtime();
    free(buckets);
    free(recvbuffer);
    free(results);
    return endTime - startTime;
}

double average(double times[], int n){
    double sum = 0;
    for (int i = 0; i < n; ++i) {
        sum += times[i];
    }
    return sum / n;
}

int main(){

    FILE *fp;
    fp=fopen("input.txt","r");
    int size, min, max;
    double *arr;
    fscanf(fp,"%d",&size);
    fscanf(fp,"%d %d",&min, &max);

    arr=(double*) calloc(size,sizeof(double));
    int i=0;
    for(;i<size;i++) fscanf(fp,"%lf",&arr[i]);
    fclose(fp);

    MPI_Init(NULL, NULL);

    int nproc;
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    double *res_arr = (double*) calloc(size,sizeof(double));

    double timeElapsed = bucketsort(arr, size, nproc, rank, min, max, res_arr);
    double times[nproc];
    MPI_Allgather(&timeElapsed, 1, MPI_DOUBLE, times, 1, MPI_DOUBLE, MPI_COMM_WORLD);
    double avgTimeElapsed = average(times, nproc);
	double reduce[size];
	MPI_Allreduce(res_arr, reduce, size, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
	if(rank == 0){
        printf("bucketsort: %lf\n", avgTimeElapsed);
        for(i=0;i<size;i++) printf("%lf\n",reduce[i]);
}
    MPI_Finalize();
    free(arr);
    free(res_arr);
    return 0;
}
