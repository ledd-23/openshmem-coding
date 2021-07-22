#include <shmem.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_POINTS 10000

long long inside = 0, total = 0; //long long is double the size of long

int main(int argc, char **argv){

	int me, myshmem_n_pes;
	
	shmem_init();
	
	myshmem_n_pes = shmem_n_pes();
	me = shmem_my_pe();

	srand(1+me); //sets the seed

	//each PE calaculates NUM_POINTS times
	for(total = 0; total < NUM_POINTS; ++total) {
		double x,y;

		//coordinates between 0 and 1
		x = rand()/(double)RAND_MAX; 
		y = rand()/(double)RAND_MAX;

		//inside the circle
		if(x*x + y*y < 1) {
			++inside;
		}
	}

	shmem_barrier_all();

	if(me == 0) {
		for(int i = 1; i < myshmem_n_pes; ++i) {
			long long remoteInside,remoteTotal;

			shmem_longlong_get(&remoteInside,&inside,1,i);
			shmem_longlong_get(&remoteTotal,&total,1,i);

			//get inside and total of every PE
			total += remoteTotal;
			inside += remoteInside;
		}

		//ratio of the quarters of the circle and square is sure to be pi/4
		double approx_pi = 4.0*inside/(double)total;

		printf("Pi from %llu points on %d PEs: %lf\n", total, myshmem_n_pes, approx_pi);
	}

	shmem_finalize();

	return 0;
}
