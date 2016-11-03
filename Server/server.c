#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <errno.h>

#define size 16

/*
The server must be run with 1 argument specifying 
the maximun number of its clients.
*/


void error(char *mes) {

	printf("%s\n", mes);

	exit(-1);

}

int intValue(char *s) {

        int result = 0;

        while((*s) != 0) {

                if (((*s) < '0') || ((*s) > '9')) {
                        error("Not an integer string.");
                }

                result *= 10;

                result += (*s) - '0';

                s++;

        }

        return result;

}

/*

Message type 1 means that the message sent to the server

Any other message type is equal to (id + 1) and is sent to the client with this id

*/

struct msgbuf {

	long mtype;

	int mtext[4];

};

struct msgbuf buffer;

#define text buffer.mtext

int msqid;

void send(int cid) {

	buffer.mtype = cid + 1;

	if (msgsnd(msqid, &buffer, size, 0) < 0) {

		printf("Message to the client %d cannot be sent.\n", cid);

		exit(-1);

	}

}

void sendFlag(int cid, int flg) {
	
	text[0] = flg; // success or fail

	send(cid);

}

void receive() { // return the type of the message received

	if (msgrcv(msqid, &buffer, size, 1, MSG_NOERROR) < 0) {

		printf("%s\n", strerror(errno));

		error("No message can be received.");
	}

}

struct sembuf sbuf;

int semid;

void inc() {

	sbuf.sem_op = 1;

	if (semop(semid, &sbuf, 1) < 0) {
		error("Problems with increasing the semaphore.");
	}

}

int dec() {

	sbuf.sem_op = -1;

	if (semop(semid, &sbuf, 1) < 0) {
		
		if (errno == EAGAIN) {
			return -1;
		}

		error("Problems with decreasing the semaphore.");

	}

	return 0;
	
}

int main(int argc, char **argv) {
	
	int limit = intValue(argv[1]);

	printf("The maximum number of clients: %d.\n", limit);

	key_t mKey = ftok("client.c", 0);

	if (mKey < 0) {
		error("The message key cannot be generated.");
	}

	msqid = msgget(mKey, 0666 | IPC_CREAT);

	if (msqid < 0) {
		error("Message queue cannot be created or accessed.");
	}

	key_t sKey = ftok("server.c", 0);

	if (sKey < 0) {
		error("The semaphore key cannot be generated.");
	}

	semid = semget(sKey, 1, 0666 | IPC_CREAT);

	if (semid < 0) {
		error("A semaphore cannot be created.");
	}

	union semun {

		int val;
		struct semid_ds *buf;
		unsigned short  *array;
		struct seminfo  *_buf;
	
	} su;

	su.val = limit;

	if (semctl(semid, 0, SETVAL, su) < 0) {
		error("Problems with initialising the semaphore.\n");
	}

	sbuf.sem_num = 0;
	sbuf.sem_flg = IPC_NOWAIT;

	while(1) {

		printf("Waiting for a client.\n");

		receive();

		//the first byte of the message determines
		//wether it is common or terminating

		int cid = text[1]; // client id

		printf("A message received from the client with id: %d.\n", cid);

		if (dec() < 0) {

			sendFlag(cid, -1);

			continue;

		}

		if (text[0] == 1) { // terminating message

			printf("The terminating message received.\n");

			if (semctl(semid, 0, IPC_RMID) < 0) {
				printf("The semaphore cannot be destroyed.\n");
			}

			sendFlag(cid, 0); // success

			printf("Terminating.\n");

			return 0;

		}

		int pid = fork();

		if (pid > 0) {
			
			printf("The request of the client %d is being processed.\n", cid);

			// now process the request

			int res = text[2] + text[3];

			printf("Going to sleep.\n");

			sleep(res);

			text[0] = 0; // success
			text[1] = res;

			send(cid);

			printf("The answer sent.\n");

			inc();

			return 0;

		}

	}

}
