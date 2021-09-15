#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <shmem.h>
#include <shmemx.h>
#include <math.h>

int rank;

long long q_length;
long long elem_length;

double min;
double max;

double data[1048576];

#define N 67108864

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

static long long tsk_q[N];

void steal(int from, long long index, long long size){
	clock_t t = clock();
	shmem_get(&tsk_q[index], &tsk_q[index], size, from);
	t = clock() - t;
	double time_taken = ((double)t/CLOCKS_PER_SEC)*1000000;
	
	data[index/size] = time_taken;

	max = MAX(max, time_taken);
	min = MIN(min, time_taken); 	
}

void communicate(void (*method)(), long long tsk_size, long long msg_iter){
	if (rank == 0)
		for (long long i = 0; i < q_length; i += elem_length)
			(*method)(1, i, elem_length);
}

void printtable(long long tsk_size, long long msg_iter, double time_taken){
	printf("============ RESULTS ================\n");
	printf("Message size:	    %ld	     \n", tsk_size);
	printf("Message count:	    %ld 	     \n", msg_iter);
	printf("Total duration:	    %.3f      %s\n", time_taken, "ms"); 
	
	double average = time_taken*1000/msg_iter;
	printf("Average duration:   %.3f         %s\n", average, "us");
	
	printf("Minimum duration:   %.3f	  %s\n", min, "us");
	printf("Maximum duration:   %.3f	  %s\n", max, "us");
	
	double sum = 0;
	for (int i = 0; i < msg_iter; i ++)
		 sum += (data[i] - average) * (data[i] - average);
	
	sum /= (msg_iter - 1);
	sum = sqrt(sum);

	printf("Standard deviation: %.3f	  %s\n", sum, "us");
	printf("Message rate:	    %.3f	  %s\n", msg_iter/(time_taken/1000), "msg/s");
	printf("=====================================\n");
}

void createqueue(long long tsk_size, long long msg_iter){
	elem_length = tsk_size / sizeof(long long);
	
	q_length = msg_iter * elem_length;
	
	for (int64_t i = 0; i < q_length; i ++)
		tsk_q[i] = i;
}

int main(int argc, char *argv[]){

	shmem_init();
	
	long long msg_iter = 1048576;
	long long tsk_size = 512;

	rank = shmem_my_pe();

	if (argc > 1){
		msg_iter = atoi(argv[1]);
		if (argc > 2)
			tsk_size = atoi(argv[2]);
	}

	shmem_barrier_all();
	
	createqueue(tsk_size, msg_iter);	

	clock_t t;
	
	if (rank == 0)
		t = clock();

	communicate(&steal, tsk_size, msg_iter);

	shmem_barrier_all();	

	if (rank == 0)
		printtable(tsk_size, msg_iter, ((double)(clock()-t)/CLOCKS_PER_SEC)*1000); 

	shmem_finalize();	
}
