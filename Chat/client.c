#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include "communication.h"

#define check() errCheck(__LINE__)

void errCheck(int line) {
        if (errno != 0) {

                printf("Error %d in the line %d occured:\n%s\n", errno, line - 1, strerror(errno));

                exit(-1);

        }
}

#define debug() dbg(__LINE__)

void dbg(int line) {
        printf("The line %d passed.\n", line);
}


int main(int argc, char **argv) {

	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	check();
	
	struct sockaddr_in addr;

        addr.sin_family = AF_INET;
        //addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(5000);

	inet_pton(AF_INET, argv[1], &addr.sin_addr);
	check();

	connect(sfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in));
	check();

	#define mesize 1000

	char mes[mesize];

	printf("Enter your name:\n");
	scanf("%s", mes);

	writeMessage(sfd, mes, strlen(mes) + 1); // send the client's name
	check();

	// start communication

	int pid = fork();

	if (pid == 0) { // the child sends the messages
		while(1) {

			scanf("%s", mes);

			writeMessage(sfd, mes, strlen(mes) + 1);
			check();

		}
	}

	// the parent receives the messages
	
	#define nameLength 50

	struct buffer {

		char name[nameLength];

		time_t time;

		char message[mesize];

	} buf;

	struct tm *t; // time structure

	while(1) {

		readMessage(sfd, &buf, sizeof(struct buffer));
		check();
		
		t = localtime(&buf.time);

		printf("[%d:%d:%d] %s: %s\n", t[0].tm_hour, t[0].tm_min, t[0].tm_sec, buf.name, buf.message);

	}

}
