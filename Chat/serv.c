#include <sys/types.h> 
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>
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

#define limit 50

// clients list

#define nameLength 100

struct clientInfo { // client information

	int fd; // socket file descriptor

	char name[nameLength + 1];

};

struct clientInfo *heap; // heap of empty structures
struct clientInfo **cls; // clients
int *clN; // clients number

void addClient(struct clientInfo *cl) {

	cls[*clN] = cl;

	clN[0]++;

}

void removeClient(struct clientInfo *cl) {

	int i;

	for(i = 0; i < clN[0]; i++) {
		if(cls[i] == cl) {
			break;
		}
	}

	for(i = i + 1; i < clN[0]; i++) {
		cls[i - 1] = cls[i];
	}

	clN[0]--;

}

// clients list end

// semaphore

int semid;

struct sembuf sbuf[2];

// blocks the clients array or suspends the execution
// of the calling process if it is already blocked
void block() { 

	semop(semid, &sbuf[1], 1); 
	check();

}

void unblock() { // unblocks the clients array

        semop(semid, sbuf, 1); 
        check();

}

// semaphore end

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

	// 2. create a clients list

	clN = mmap(NULL, 4 + limit * sizeof(size_t) + limit * sizeof(struct clientInfo), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0); // clients number
	// heap of clients information structures
	heap = (struct clientInfo *) (clN + 4 + limit * sizeof(size_t));
	// the unused space must be set to zero
	memset(heap, 0, limit * sizeof(struct clientInfo));

	cls =  (struct clientInfo **) (clN + 4); // array of *clients information
	clN[0] = 0;

	// the clients list created

	// 3. create a semaphore for parallel work with the clients list

	semid = semget(ftok("serv.c", 0), 1, 0666 | IPC_CREAT);
	check();

	sbuf[0].sem_num = 0;
	sbuf[0].sem_flg = 0;
	sbuf[0].sem_op = 1;

	sbuf[1] = sbuf[0];
	sbuf[1].sem_op = -1;

	// set the initial value == 1

	union semun {

		int val;
		struct semid_ds *buf;
		unsigned short  *array;
		struct seminfo  *_buf;

	} su;

        su.val = 1;
        semctl(semid, 0, SETVAL, su);
	check(); 
	
	// the semaphore created

	// 4. Proceed to accept connections from clients

	int cfd; // client file descriptor

	printf("The server has successfully started.\n");

	while(1) {

		cfd = accept(sfd, NULL, NULL);
		check();

		printf("New client detected.\n");

		int pid = fork();

		if (pid == 0) { // the child must leave the cycle
			break; 
		}
		
		// the parent never leaves the cycle

	}

	#define mesize 1000 // message size

	struct buffer {

		char name[nameLength];

		time_t time;

		char message[mesize];

	} buf;

	int l; // length (of the string read)

	// fisrt, register the client
	// the first message is treated as the name

	l = readMessage(cfd, buf.message, mesize);
	if (l > nameLength) {
		l = nameLength;
	}
	buf.message[l - 1] = 0;

	// start initialisation

	block(); 

	int i = 0;
	while(heap[i].fd != 0) { // search for an empty structure in the heap
		i++;
	}

	struct clientInfo *client = &heap[i];

	client[0].fd = cfd;
	sprintf(client[0].name, "%s", buf.message); 	

	addClient(client); //add the *structure to the clients list

	unblock();

	// the client registered

	sprintf(buf.name, "SERVER");
	buf.time = time(NULL);
	sprintf(buf.message, "Welcome, %s!", client[0].name);
	writeMessage(client[0].fd, &buf, sizeof(struct buffer));

	printf("Client %s has been initialised.\n", client[0].name);

	// start receiving messages

	sprintf(buf.name, "%s", client[0].name);

	while(1) {

		l = readMessage(client[0].fd, buf.message, mesize);
		buf.message[l - 1] = 0;

		buf.time = time(NULL);

		block();
		
		printf("Processing message from %s.\n", client[0].name);
		
		for(i = 0; i < clN[0]; i++) {
	
			if (cls[i] == client) {
				continue;
			}

			writeMessage(cls[i][0].fd, &buf, sizeof(struct buffer) - mesize + l);

		printf("Message sent to %s.\n", cls[i][0].name);

		}

		unblock();

	}

}
