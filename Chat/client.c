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

	printf("Connection established.\n");	

	// start communication

	#define mesize 1000

	int pid = fork();

	if (pid == 0) { // the child sends the messages

		char mes[mesize];

		int l;

		while(1) {

			l = read(STDIN_FILENO, mes, mesize);
			mes[l - 1] = 0;

			writeMessage(sfd, mes, l);
			check();

		}

	}

	// the parent receives the messages
	
	#define nameLength 100

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

		printf("[%02d:%02d:%02d] %s: %s\n", t[0].tm_hour, t[0].tm_min, t[0].tm_sec, buf.name, buf.message);

	}

}
