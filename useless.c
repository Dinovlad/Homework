#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys/time.h>
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

	int t; // time count

	FILE *f = fopen(argv[1], "r");

	char buf[256];

	while(fgets(buf, 256, f) != NULL) {

		int id = 1;

		id = fork();
		if (id > 0) {

			int i = 1;
			int delay = buf[0] - '0';

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

			sleep(delay);

			execl(path, path, buf, NULL);

			return 0;

		}

	}

}
