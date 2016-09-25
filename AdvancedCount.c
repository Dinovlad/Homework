#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <unistd.h>

/** The program calculates the sum of integers 
 * from argv[1] to argv[2] using argv[3] processes
 * and prints it in the console.
 * */

int intValue(char *s) {

	int result = 0;
	int i = 0;
	char c = s[0];

	while (c != 0) {

		result *= 10;
		result += c - '0';

		i++;
		c = s[i];

	}

	return result;

}

int main(int argc, char **argv) {

	printf("The first argument: %d\n", intValue(argv[1]));
	
	int from = intValue(argv[1]);
	int to = intValue(argv[2]);
	int N = intValue(argv[3]);
	
	int step = (to - from) / N;

	unsigned long long int S = 0;
	
	int pipefd[2];
	pipe(pipefd);

	int id;
	int i;
	int j;
        unsigned char buf[8];
	
	N--;

	for(i = 0; i < N; i++) {
		
		id = getpid();
		id = fork();	

		if (id == 0) { // it is the child
			
			int end = from + (i + 1) * step;

       			for(j = from + i * step; j < end; j++) {
				S += j;
        		}

        		for(j = 0; j < 8; j++) {


                        	buf[j]= (unsigned char) S;

				S >>= 8;

                	}		
			
			write(pipefd[1], buf, 8);

			return 0;
		
		}

	}

	for(i = from + N * step; i <= to; i++) {
		S += i;
	}

	unsigned long long int buff;

	for(i = 0; i < N; i++) {

		read(pipefd[0], buf, 8);
		
		buff = buf[7];

		for(j = 6; j >= 0; j--) {
		
			buff <<= 8;
			buff += (unsigned long long int) buf[j];

		}

		S += buff;

	}

	printf("%llu\n", S);
	
	wait(NULL);

	return 0;

}
