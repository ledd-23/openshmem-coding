//Skeleton written by Professor Larkins. Modified by Danh Le.
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <shmem.h>
#include <shmemx.h>

#define MAXPROC 2112

int rank, size;
uint64_t parent_sent, left_sent, right_sent; // signal flag values
int64_t myval,leftval, rightval;             // local places to send/recv messages

void broadcast(int64_t *val) {
	int left = rank*2 + 1;

	if (left < size)
		shmem_putmem_signal(&myval, val, sizeof(int64_t), &parent_sent, 1, SHMEM_SIGNAL_SET, left);

	if (left + 1 < size) 
		shmem_putmem_signal(&myval, val, sizeof(int64_t), &parent_sent, 1, SHMEM_SIGNAL_SET, left+1);
}

void reduce(int64_t *local, int64_t *result) {
	int parent = (rank + 1) / 2 - 1;
	*result = *local + leftval + rightval;
	if (parent >= 0 && rank % 2 != 0) {
		shmem_putmem_signal(&leftval, result, sizeof(int64_t), &left_sent, 1, SHMEM_SIGNAL_SET, parent);
	} else if (parent >= 0 && rank % 2 == 0) {
		shmem_putmem_signal(&rightval, result, sizeof(int64_t), &right_sent, 1, SHMEM_SIGNAL_SET, parent);
	}
}


int main(int argc, char **argv) {
	  int64_t reduceval = 0;
	  setbuf(stdout, NULL);

	  shmem_init();

	  rank = shmem_my_pe();
	  size = shmem_n_pes();

	  if (rank == 0) {
	       myval = size;
	  }

          parent_sent = left_sent = right_sent = 0;
          leftval = rightval = 0;

	  shmem_barrier_all();
	  
	  if (rank != 0){	
		shmem_wait_until(&parent_sent, SHMEM_CMP_EQ, 1);
	  
	  }

          broadcast(&myval);

          shmem_barrier_all();
	
	  for (int i = 0; i < size; i ++){
		  if (i == rank)
	 	      printf("rank %d received %ld\n", rank, myval);
		      shmem_barrier_all();
	  }	      	 

	  shmem_barrier_all();

	  int left = rank*2 + 1;

	  if (left < size){
		  shmem_wait_until(&left_sent, SHMEM_CMP_EQ, 1);
	  }

	  if (left + 1 < size){
		  shmem_wait_until(&right_sent, SHMEM_CMP_EQ, 1);
	  }

          reduce(&myval, &reduceval);

          shmem_barrier_all();
	  if (rank == 0)
	      printf("reduced value is: %ld\n", reduceval);

          shmem_finalize();
}

