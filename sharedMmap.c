#include <stdio.h>
#include <sys/mman.h>

/**
The programme demonstrates the use of mmap()
to create a shared memory unit
*/

int main() {

	int *i = mmap(NULL, 4, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);

	i[0] = 12;

	fork();

	printf("%d\n", i[0]);

	return 0;

}
