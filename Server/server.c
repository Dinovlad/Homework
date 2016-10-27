#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define size 40

void error(char *mes) {

	printf("%s\n", mes);

	exit(-1);

}

/*

Message type 1 means that the message sent to the server

Any other message type is equal to (id + 1) and is sent to the client with this id

*/

struct msgbuf {

	int mtype;

	int mtext[5];

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

void sendFlag(int cid, int mes) {
	
	buffer.mtext[0] = mes; // success or fail

	send(cid);

}

void receive() { // return the type of the message received

	if (msgrcv(msqid, &buffer, size, 1, MSG_NOERROR) < 0) {
		error("No message can be received.");
	}

}

int main() {

	key_t mKey = ftok("client.c", 0);

	if (mKey < 0) {
		error("The message key cannot be generated.");
	}

	msqid = msgget(mKey, 0666 | IPC_CREAT);


	if (msqid < 0) {
		error("Message queue cannot be created or accessed.");
	}

	int *ids;

	key_t shmKey = ftok("server.c", 0);

	if (shmKey < 0) {
		error("The shared memory key cannot be generated.");
	}

	int shmid = shmget(shmKey, 8, 0666 | IPC_CREAT);

	if (shmid < 0) {
		error("Shared memory cannot be created.");
	}

	ids = shmat(shmid, NULL, 0);

	if (ids < 0) {
		error("Shared memory cannot be attached.");
	}

	ids[0] = 0;
	ids[1] = 0;

	while(1) {

		printf("Waiting for a client.\n");

		receive();

		printf("A message received.\n");

		//the first byte of the message determines
		//wether it is common or terminating

		int cid = text[1]; // client id

		printf("The client id: %d.\n", cid);

		if ((ids[0] == cid) || (ids[1] == cid)) {

			printf("A client with this id is already being served.\n");

			kill(text[4], 9);		

			continue;

		}

		if (ids[0] == 0) {

			ids[0] = cid;

			cid = 0;

		} else if (ids[1] == 0) {

			ids[1] = cid;

			cid = 1;

		} else {

			sendFlag(cid, -1);

			continue;

		}

		if (text[0] == 1) { // terminating message

			printf("The terminating message received.\n");

			wait(NULL);

			sendFlag(ids[cid], 0); // success

			printf("Terminating.\n");

			return 0;

		}

		int pid = fork();

		if (pid == 0) {
			
			printf("The request of the client %d is being processed.\n", ids[cid]);

			// now process the request

			int res = text[2] + text[3];

			printf("Going to sleep.\n");

			sleep(res);

			text[0] = 0; // success
			text[1] = res;

			send(ids[cid]);

			printf("The answer sent.\n");

			ids[cid] = 0;

			return 0;

		}

	}

}
