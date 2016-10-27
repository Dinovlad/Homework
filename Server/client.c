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

int id;

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

/**

Arguments format:

<id> <a> <b>

<id> exit

The client sends numbers a and b to the server and waits
for a reply in a message of type (id + 1)

if argv[2] is "exit" the client sends a terminating message to the server

*/

void send() {

	if (msgsnd(msqid, &buffer, size, 0) < 0) {
		error("The message cannot be sent.");
	}

}

void receive() {

	if (msgrcv(msqid, &buffer, size, id + 1, MSG_NOERROR) < 0) {
		error("No message can be received.");
	}

}

int main(int argc, char **argv) {

	key_t mKey = ftok("client.c", 0);

	if (mKey < 0) {
		error("The message key cannot be generated.");
	}

	msqid = msgget(mKey, 0666 | IPC_CREAT);

	buffer.mtype = 1; //the server gets only the messages of type 1

	if (msqid < 0) {
		error("Message queue cannot be created or accessed.");
	}
	
	id = intValue(argv[1]);		

	if (strcmp(argv[2], "exit") == 0) {

		text[0] = 1; // terminating message
		text[1] = id;
		text[4] = getpid();

		send();

		receive();

		if (text[0] == 0) { // success
			printf("The server is terminating.\n");
		} else if (text[0] == -1) {
			printf("Request denied.\n");
		} else {
			printf("Strange reply.\n");
		}

	} else {
	
		text[0] = 0; // casual message
		text[4] = getpid();

		int i;
		for (i = 1; i < 4; i++) {
			text[i] = intValue(argv[i]);
		}

		send();

		receive();

		if (text[0] == 0) { // success
			printf("The reply: %d.\n", text[1]);
		} else if (text[0] == -1) {
			printf("Request denied.\n");
		} else {
			printf("Strange reply.\n");
		}

	}

	return 0;

}
