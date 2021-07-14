//Skeleton written by Professor Larkins. Modified by Danh Le.
#include <stdlib.h>
#include <stdio.h>

#include <shmem.h>
#include <shmemx.h>

#define MAXPROC 2112

int rank, size;
uint64_t parent_sent, left_sent, right_sent; // signal flag values
int64_t myval,leftval, rightval;             // local places to send/recv messages

void broadcast(int64_t *val) {
	if (rank == 0){
		for (int i = 0; i < size; i ++){
			shmem_p(&myval, *val, i);
		}
	}	
}



void reduce(int64_t *local, int64_t *result) {
	*result = *local*10;
}



int main(int argc, char **argv) {
	  int64_t value = 0, reduceval = 0;
	  setbuf(stdout, NULL);

	  shmem_init();

	  rank = shmem_my_pe();
	  size = shmem_n_pes();

	  if (rank == 0) {
	       value = size;
	  }

          parent_sent = left_sent = right_sent = 0;
          myval = leftval = rightval = 0;

	  shmem_barrier_all();

          broadcast(&value);

          shmem_barrier_all();

          for (int i=0; i<size; i++) {
		if (i==rank)
		   printf("rank %d received %ld\n", rank, myval);
		   shmem_barrier_all();
	  }

	  shmem_barrier_all();

          reduce(&value, &reduceval);

          shmem_barrier_all();
	  if (rank == 0)
	      printf("reduced value is: %ld\n", reduceval);

         shmem_finalize();
}

