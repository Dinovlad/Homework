#include <sys/types.h> 
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include "communication.h"

#include <signal.h>
#include <pthread.h>

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

#define limit 50

// clients list

#define nameLength 100

struct clientInfo { // client information

	pthread_t thr; // thread (id)

	int fd; // socket file descriptor

	char name[nameLength + 1];

};

struct clientInfo heap[limit * sizeof(struct clientInfo)]; // heap of empty structures
struct clientInfo *cls[limit * sizeof(struct clientInfo *)]; // clients
int clN; // clients number

void addClient(struct clientInfo *cl) {

	cls[clN] = cl;

	clN++;

}

void removeClient(int i) {

	for(i = i + 1; i < clN; i++) {
		cls[i - 1] = cls[i];
	}

	clN--;

}

// clients list end

// mutex

pthread_mutex_t mutex;

#define block() pthread_mutex_lock(&mutex)
#define unblock() pthread_mutex_unlock(&mutex)

// mutex end

#define mesize 1000 // message size

struct buffer {

	char name[nameLength];

	time_t time;

	char message[mesize];

} glbuf; // global buffer

void broadcast(void *buf, size_t count, struct clientInfo *source) {

	int i;

	for(i = 0; i < clN; i++) {

		if (cls[i] == source) {
			continue;
		}

		if (writeMessage(cls[i][0].fd, buf, count) < 0) {
			cls[i][0].fd = -1; // disable the structure
		}

	}

}

void cleanList() {

	int i = 0;

	while(i < clN) {

		if (cls[i][0].fd < 0) {

			glbuf.time = time(NULL);		

			sprintf(glbuf.name, "SERVER");
			sprintf(glbuf.message, "%s left.", cls[i][0].name);
			
			pthread_cancel(cls[i][0].thr);
			removeClient(i);

			broadcast(&glbuf, sizeof(struct buffer), NULL);

		}

		i++;

	}

}



void * processClient(void *arg) {	

	struct buffer buf;

	int l; // length (of the string read)

	// fisrt, register the client
	// the first message is treated as the name

	int cfd = (int) arg;

	l = readMessage(cfd, buf.message, mesize);
	if (l < 0) {
		return NULL;
	}

	if (l > nameLength) {
		l = nameLength;
	}
	buf.message[l - 1] = 0;

	// start initialisation

	block(); 

	int i = 0;
	while(heap[i].fd >= 0) { // search for an empty structure in the heap
		i++;
	}

	struct clientInfo *client = &heap[i];

	client[0].fd = cfd;
	sprintf(client[0].name, "%s", buf.message); 	
	client[0].thr = pthread_self();

	addClient(client); //add the *structure to the clients list

	unblock();

	// the client registered

	sprintf(buf.name, "SERVER");
	buf.time = time(NULL);
	sprintf(buf.message, "Welcome, %s!", client[0].name);
	if (writeMessage(client[0].fd, &buf, sizeof(struct buffer)) < 0) {
		client[0].fd = -1;
		return NULL;
	}

	printf("Client %s has been initialised.\n", client[0].name);

	// start receiving messages

	sprintf(buf.name, "%s", client[0].name);

	while(1) {

		pthread_testcancel(); // cancellation point
	
		l = readMessage(client[0].fd, buf.message, mesize);
		if (l < 0) {
			client[0].fd = -1;
			return NULL;
		}

		buf.message[l - 1] = 0;

		buf.time = time(NULL);

		block();
		
		printf("Processing message from %s.\n", client[0].name);

		broadcast(&buf, sizeof(struct buffer) - mesize + l, client);
		cleanList();	
	
		unblock();

	}

	return NULL;

}

void * waitCommand(void *arg) {

	char command[10];
	
	while(1) {

		scanf("%s", command);

		if (strcmp(command, "exit") == 0) {
			
			printf("Terminating.\n");

			exit(0);

		}

	}

	return NULL;

}

void ignore(int i) {}

int main() {

	// 1. first create a socket

	int sfd = socket(AF_INET, SOCK_STREAM, 0); // socket file desriptor
	check();

	struct sockaddr_in addr;

        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(5000);

	bind(sfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in));
	check();

	listen(sfd, 10);
	check();

	// a passive socket created

	// 2. initialise the clients list

	for(clN = 0; clN < limit; clN++) {
		heap[clN].fd = -1; // mark the structures as empty
	}

	clN = 0; // clients number

	// the clients list initialised

	// 3. Prepare for multithread work
	
	errno = pthread_mutex_init(&mutex, NULL);
	check();

	pthread_t thr;

	// thread waiting for commands from terminal
	errno = pthread_create(&thr, NULL, waitCommand, NULL);
	check();
	
	signal(SIGPIPE, ignore); //prevent crashing when writing

	// prepared for multithreading

	// 4. Proceed to accept connections from clients

	int cfd; // client file descriptor

	printf("The server has successfully started.\n");

	while(1) {

		cfd = accept(sfd, NULL, NULL);
		check();

		printf("New client detected.\n");	

		errno = pthread_create(&thr, NULL, processClient, (void *) cfd);
		check();

	}

}
