#include <unistd.h>
#include <stdio.h>
#include <time.h>
/**
 * The program reads the file specified by argv[1]
 * and executes the programs listed with the specified delay.
 * Each string in the file must have the following format:
 * 
 * <delay in seconds> <filename(or path)>
 * 
 * The file can contain several strings.
 */

int main(int argc, char **argv) {

	struct timespec offset;

	clock_gettime(CLOCK_MONOTONIC_RAW, &offset);

	FILE *f = fopen(argv[1], "r");

	char buf[256];

	while(fgets(buf, 256, f) != NULL) {

		int id = fork();
		if (id == 0) {

			int i = 0;

			while(buf[i] == ' ') {
				i++;
			}

			if (buf[i] == '\n') {
				return 0;
			}

			float delay = buf[i++] - '0';

			while (buf[i] != ' ') {
				
				delay *= 10;
				delay += buf[i] - '0';

				i++;

			}

			buf[i] = 0;	
			
			while (buf[++i] == ' '); // several whitespaces considered
			
			char *path = &buf[i];

			//close the name of the path with 0
			while ((buf[i] != '\n') && (buf[i] != ' ')) {
				i++;
			}

			buf[i] = 0;//closed
			
			struct timespec current;

			clock_gettime(CLOCK_MONOTONIC_RAW, &current);

			delay += offset.tv_sec - current.tv_sec +
				((float) (offset.tv_nsec -
					current.tv_nsec)) / 1000000000;
			printf("%f\n", delay);
			sleep(delay);

			execl(path, path, buf, NULL);

			return 0;

		} 

	}

	return 0;

}
