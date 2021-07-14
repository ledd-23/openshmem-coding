//Written by Professor Larkins
#include <stdlib.h>
#include <stdio.h>

#include <shmem.h>
#include <shmemx.h>

#define MAXPROC 2112

int a[MAXPROC];

int main(int argc, char **argv) {
	  int rank, size;
	  static int local;

	  shmem_init();

	  rank = shmem_my_pe();
	  size = shmem_n_pes();

	  if (rank == 0) {
		for (int i=0; i<MAXPROC; i++)
			a[i] = 0;
	  }

	  shmem_barrier_all();

	  local = shmem_g(&a[rank],  0);
	  local += rank;

	  shmem_barrier_all();

	  shmem_p(&a[rank], local, 0);

	  shmem_barrier_all();

	  if (rank == 0) {
		for (int i=0; i<size; i++)
			 printf("a[%d] = %d\n", i, a[i]);
	  }

	  shmem_finalize();
}
