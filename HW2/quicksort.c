#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

// NOTE THAT this code works SERIALLY not in parallel
// Use this only with 1 process

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
    for (i=0; i<j; i++) a[i] =b[i];
    a[j] = p;
    for (i= 0; i<k; i++) a[j+1+i] =c[i];
}

int main()

{

// Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

	FILE *fp;
	fp=fopen("input.txt","r");
	int size, min, max;
	double *arr, start, end;
	fscanf(fp,"%d",&size);
	fscanf(fp,"%d %d",&min, &max);

	arr=(double*) calloc(size,sizeof(double));
	int i=0;
	for(;i<size;i++) fscanf(fp,"%lf",&arr[i]);
	
	start=MPI_Wtime();
	quicksort(arr,size);
	end=MPI_Wtime();

	printf("quicksort: %lf\n",end-start);
	for(i=0;i<size;i++) printf("%lf\n",arr[i]);
	
	fclose(fp);
    // Finalize the MPI environment.
    MPI_Finalize();

	return 0;
}
