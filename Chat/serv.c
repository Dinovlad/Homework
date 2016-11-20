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


#define check() errCheck(__LINE__)

void errCheck(int line) {
	if (errno != 0) {

		printf("Error %d in the line %d occured:\n%s\n", errno, line - 1, strerror(errno));

		exit(-1);

	}
}

#define limit 50

struct clientInf { // client information

	int fd; // socket file descriptor

	char name[100];

};


struct clientInf **cls; // clients
int *clN; // clients number

void addClient(struct clientInf *cl) {

	cls[clN] = cl;

	clN++;

}

void removeClient(struct clientInf *cl) {

	int i;

	for(i = 0; i < clN; i++) {
		if(cls[i] == cl) {
			break;
		}
	}

	for(i = i + 1; i < clN; i++) {
		cls[i - 1] = cls[i];
	}

	clN--;

}



int main() {

	// 1. first create a socket

	int sfd = socket(AF_INET, SOCK_STREAM, 0); // socket file desriptor
	check();

	struct sockaddr_in addr;

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        serv_addr.sin_port = htons(5000);

	bind(sfd, &addr, sizeof(struct sockaddr_in));
	check();

	listen(sfd, 10);
	check();

	// a passive socket created

	// 2. create a clients list

	clN = mmap(NULL, 4 + limit * sizeof(size_t) + limit * sizeof(struct clientInfo), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0); // clients number
	// heap of clients information structures
	struct clientInfo *heap = clN + 4 + limit * sizeof(size_t);
	// the unused space must be set to zero 
	memset(heap, 0, limit * sizeof(struct clientInfo));
	cls =  clN + 4; // array of *clients information
	clN[0] = 0;

	// the clients list created

	// 3. create a semaphore for parallel work with the clients list

	int semid = semget(ftok("serv.c"), 1, IPC_CREAT);
	check();

	struct sembuf sbuf; //semaphore buffer
	sbuf.sem_num = 0;
	sbuf.sem_flg = 0;

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

	while(1) {

		cfd = accept(sfd, NULL, NULL);
		check();

		pid = fork();

		if (pid == 0) { // the child must leave the cycle
			break; 
		}
		
		// the parent never leaves the cycle

	}

	#define bufsize 1000

	char buf[bufsize + 1];
	int l; // length (of the string read)

	// fisrt, register the client
	// the first message is treated as the name

	l = read(client.fd, buf, bufsize);

	// start initialisation

	// declare the semaphore functions
	void block();
	void unblock();

	block(); 

	int i = 0;
	while(heap[i].fd != 0) { // search for an empty structure in the heap
		i++;
	}

	struct clientInfo *client = &heap[i];

	client[0].fd = cfd;

	buf[l] = 0;
	sprintf(client[0].name, "%s", buf); 

	addClient(client) //add the *structure to the clients list

	unblock();

	// the client registered

	// start receiving messages

	while(1) {

		l = read(client[0].fd, buf, bufsize);

		block();

		for(i = 0; i < clN; i++) {
	
			if (cls[i] == client) {
				continue;
			}

			write(cls[i][0].fd, buf, l);

		}

		unblock();

	}

}

// blocks the clients array or suspends the execution
// of the calling process if it is already blocked
void block() { 

        sbuf.sem_op = -1;
        semop(semid, &sbuf, 1); 
        check();

}

void unblock() { // unblocks the clients array

        sbuf.sem_op = 1;
        semop(semid, &sbuf, 1); 
        check();

}
