#include <shmem.h>
#include <stdio.h>
#include <stdlib.h>
int main(void) {
	static long source[5], dest[5];
	static long signal; //to hold signal from higher rank pe
	static long value; //to hold value of assigned array element

	shmem_init();

	int mype = shmem_my_pe();
	int npes = shmem_n_pes();

	if (mype == 0)
		for (int i = 0; i < 5; i++)
			source[i] = i;
		
	//maybe a put of some sort for each pe != 0 to hold a number in the array
	shmem_broadcast(SHMEM_TEAM_WORLD, dest, source, 5, 0);

	//also a put here to put our value in the lower rank pe

	if (mype != 5)
		shmem_wait_until(signal, SHMEM_CMP_EQ, dest[mype]); //wait until signal is received from the higher rank pe
	
	long sum = signal + dest[mype - 1];

	printf("current sum %d of pe %d", sum, mype);

	shmem_barrier_all();

	shmem_finalize();
	return 0;
}


