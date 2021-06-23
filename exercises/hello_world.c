#include <stdio.h>
#include <shmem.h>

// Forked from the tutorial
int main (int argc, char **argv) {
	int me, npes;
	shmem_init (); //Library Initialization
	me = shmem_my_pe ();
	npes = shmem_n_pes ();
	printf ("Hello World from PE %4d of %4d\n", me, npes);
	return 0;
}
