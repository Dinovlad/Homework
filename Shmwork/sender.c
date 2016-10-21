#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void error(char *mes) {

	printf("%s\n", mes);

	exit(-1);

}

int main() {

	key_t key = ftok("sender.c", 0);

	int shmid = shmget(key, 256, 0666 | IPC_CREAT);

	if (shmid < 0) {
		error("A shared memory unit cannot be created or accessed.");
	}

	char *s  = shmat(shmid, NULL, 0);

	if (s < 0) {
		error("The shared memory cannot be attached.");
	}

	s[0] = 0; // no message

	scan:

	scanf("%255[^\r\n]", &s[1]);

/*	printf("Length of the string read: %d.\nString read:\n%s\n", strlen(&s[1]), &s[1]);
return 0;*/
	if (strcmp(&s[1], "exit") == 0) {

		s[0] = 2; // signal for the receiver to terminate

		printf("Terminating.\n");

		return 0;

	}

	s[0] = 1; // message sent
	
	while(s[0] == 1) {
		sleep(0.25);
	}

	goto scan;

}
