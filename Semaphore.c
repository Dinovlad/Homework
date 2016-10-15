#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define key 36

struct sembuf buffer;

int semid;

void op(short num, short op) {

	buffer.sem_num = num;
	buffer.sem_op = op;
	
	if (semop(semid, &buffer, 1) < 0) {

		printf("Problems with %d semaphore.\n", num);

		exit(-1);

	}

}

int main(int argc, char **argv) {

	buffer.sem_flg = 0;

	semid = semget(key, 2, 0666 | IPC_CREAT | IPC_EXCL);

	if (semid > 0) {

		op(1, 5); // 5 is maximum number of items in the store
	
	} else if (errno == EEXIST) {

		semid = semget(key, 2, 0666);
		
		if(semid < 0) {

			printf("Semaphore cannot be accessed.\n");

			return -1;

		}

	} else {

		printf("Semaphore cannot be created.\n");

		return -1;

	}

	if (strcmp(argv[1], "take") == 0) {

		op(0, -1); // take 1 item or wait for it
		op(1, 1); // increase available space

		printf("Item was taken.\n");

	} else if (strcmp(argv[1], "put") == 0) {

		op(1, -1); // decrease available space or wait for it
		op(0, 1); // put 1 item

		printf("Item was supplied.\n");

	} else {

		printf("Illegal argument.\n");

		return -1;

	}




	return 0;

}
