#include <signal.h> //signal
#include <stdio.h> //printf
#include <stdlib.h> //exit
#include <unistd.h> //sleep
#include <string.h>

int main(int argv, char **argc) {
	
	char *s = argc[1]; // must be the id of the receiver process

	int rid = 0; // receiver id

	while(s[0] != 0) {

		rid *= 10;
		rid += s[0] - '0';

		s++;

	}

	printf("Receiver id: %d.\n", rid);

	char mes[256];

	while(1){

		scanf("%s", mes);

		int N = strlen(mes);
		
		int i;
		int j;

		for(i = 0; i < N; i++) {
			for(j = 1; j <= 8; j++) {

				if(mes[i] & (256 >> j)) { // if > 0
					kill(rid, 31);	
				} else { // if == 0
					kill(rid, 30);
				}

				usleep(1);

			}
		}

		kill(rid, 10);

	}
	
}
