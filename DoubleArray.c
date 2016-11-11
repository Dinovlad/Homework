#include <stdio.h>

#define m 10000
#define n 10000

/*

The program demonstrates the location of the elements
of a double array in the memory. They all are arranged one
after another.

*/

char a[m][n];

int main() {
	
	long i;
	long j;

	printf("The address of the byte array: %ld.\n", &a[0][0]);
	printf("The size of the byte array: %d x %d.\n", m, n);

	while(1) {

		printf("Enter the number of an element (0 0 to exit): "); 

		scanf("%ld %ld", &i, &j);

		if ((i == 0) && (j == 0)) {
			return 0;
		}

		printf("The relative address of the [%lu][%lu] byte: %lu.\n", i, j, &a[i][j] - &a[0][0]);

	}

}
