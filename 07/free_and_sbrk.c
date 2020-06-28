#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_ALLOCS 1000000

int main(int argc, char *argv[])
{
	char *ptr[MAX_ALLOCS];
	int freeStep, freeMin, freeMax, blockSize, numAllocs, j;

	printf("\n");

	if (argc < 3 || strcmp(argv[1], "--help") == 0) {
		fprintf(stderr, "%s num-allocs block-size [step [min [max]]]\n",
			argv[0]);
		return -1;
	}

	numAllocs = atoi(argv[1]);
	if (numAllocs > MAX_ALLOCS) {
		fprintf(stderr, "num-allocs > %d\n", MAX_ALLOCS);
		return -1;
	}

	blockSize = atoi(argv[2]);
	freeStep  = (argc > 3) ? atoi(argv[3]) : 1;
	freeMin	  = (argc > 4) ? atoi(argv[4]) : 1;
	freeMax	  = (argc > 5) ? atoi(argv[5]) : numAllocs;

	if (freeMax > numAllocs) {
		fprintf(stderr, "free-max > num-allocs\n");
		return -1;
	}

	printf("Initial program break: %10p\n", sbrk(0));
	printf("Allocating %d*%d bytes\n", numAllocs, blockSize);

	for (j = 0; j < numAllocs; j++) {
		ptr[j] = malloc(blockSize);
		if (ptr[j] == NULL) {
			fprintf(stderr, "malloc");
			return -1;
		}
		printf("Program break is now: %10p\n", sbrk(0));
	}

	printf("Program break is now: %10p\n", sbrk(0));
	printf("Freeing blocks from %d to %d in steps of %d\n", freeMin,
	       freeMax, freeStep);
	for (j = freeMin - 1; j < freeMax; j += freeStep)
		free(ptr[j]);

	printf("After free(), program break is: %10p\n", sbrk(0));
	return 0;
}