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

	char *s = shmat(shmid, NULL, 0);

	if (s < 0) {
		error("The shared memory cannot be attached.");
	}

	read:

	while(s[0] == 0) {
		sleep(0.25);
	}

	if (s[0] == 2) {

		if (shmctl(shmid, IPC_RMID, NULL) < 0) {
			printf("Cannot clear memory.\n");
		}

		printf("Terminating.\n");

		return 0;

	}

	printf("Message received:\n%s\n", &s[1]);

	s[0] = 0;

	goto read;

}
