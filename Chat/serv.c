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

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

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

// active clients list

#define nameLength 100
#define passLength 50

struct clientInfo { // client information

	char isActive;

	pthread_t thr; // thread (id)

	int fd; // socket file descriptor

	//char password[passLength + 1];

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

pthread_mutex_t lmutex;

// active clients list end

// clients register

char reg[limit * (4 + nameLength + 1 + passLength + 1)];
int regSize;

int regSeek(char *name) {

	int i = 0;

	while(i < regSize)  {

		if (strcmp(&reg[i + 4], name) == 0) {
			
			i += 4;
			while (reg[i]) {
				i++;
			}

			return i + 1;

		}

		i += *((int *) &reg[i]) + 4;

	}

	return -1;

}

void regAdd(char *name, char *buf, int bufSize) {

	int l = strlen(name) + 1; // + 1 accounting for 0 byte

	char *pos = &reg[regSize];

	*((int *) pos) = bufSize + l;

	pos = &pos[4];

	sprintf(pos, "%s", name);

	pos = &pos[l];

	int i = 0;

	while(i < bufSize) {
		
		pos[i] = buf[i];

		i++;

	}

	regSize += 4 + bufSize + l;

}

pthread_mutex_t rmutex;

// clients register end

// mutex

#define block(mutex) pthread_mutex_lock(mutex)
#define unblock(mutex) pthread_mutex_unlock(mutex)

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

		if ((cls[i][0].isActive == 0) || (cls[i] == source)) {
			continue;
		}

		if (writeMessage(cls[i][0].fd, buf, count) < 0) {
			cls[i][0].isActive = 0; // disable the structure
		}

	}

}

void cleanList() {

	int i = 0;

	while(i < clN) {

		if (cls[i][0].isActive == 0) {

			glbuf.time = time(NULL);		

			sprintf(glbuf.name, "SERVER");
			sprintf(glbuf.message, "%s left.", cls[i][0].name);
			
			pthread_cancel(cls[i][0].thr);
			close(cls[i][0].fd);
			
			removeClient(i);

			broadcast(&glbuf, sizeof(struct buffer), NULL);

		}

		i++;

	}

}



void * processClient(void *arg) {	

	int cfd = (int) arg; // client file descriptor

	struct buffer buf;

	int l; // length (of the string read)

	// fisrt, identificate the client

	sprintf(buf.name, "SERVER");
	sprintf(buf.message, "Please, enter your name.");
	buf.time = time(NULL);
	writeMessage(cfd, &buf, sizeof(struct buffer));

	// the first message is treated as the name	

	l = readMessage(cfd, buf.message, mesize);
	if (l < 0) {
		return NULL;
	}

	if (l > nameLength) {
		l = nameLength;
	}
	buf.message[l - 1] = 0;

	// start identification

	struct clientInfo *client; 

	block(&lmutex); 

	int i = 0;
	while (heap[i].isActive) { // search for an empty structure in the heap
		i++;
	}

	client = &heap[i];

	unblock(&lmutex);

	client[0].fd = cfd;
	sprintf(client[0].name, "%s", buf.message); 	
	client[0].thr = pthread_self();
	client[0].isActive = 1;

	//check previous registration

	i = regSeek(buf.message);

	if (i < 0) { // then register as a new client

		sprintf(buf.name, "SERVER");
		sprintf(buf.message, "Please, enter a password.");
		buf.time = time(NULL);
		if (writeMessage(cfd, &buf, sizeof(struct buffer)) < 0) {
			return NULL;
		}

		while (1) {

			l = readMessage(cfd, buf.message, mesize);
			if (l < 0) {
				return NULL;
			}
			
			if (l <= passLength) {
				
				buf.message[l - 1] = 0;

				block(&rmutex);
				regAdd(client[0].name, buf.message, l);
				unblock(&rmutex);

				break;

			}

			sprintf(buf.message, "The password is too long.");
			buf.time = time(NULL);
			if (writeMessage(cfd, &buf, sizeof(struct buffer)) < 0) {
				return NULL;
			}	
			
		}	

	} else {

		// then i == index of the password in the register

		char *passw = &reg[i];

		// inquire the password

		sprintf(buf.name, "SERVER");
		sprintf(buf.message, "Please, enter your password.");
		buf.time = time(NULL);
		if (writeMessage(cfd, &buf, sizeof(struct buffer)) < 0) {
			client[0].isActive = 0;
			return NULL;
		}

		while (1) {

			l = readMessage(cfd, buf.message, mesize);
			if (l < 0) {
				client[0].isActive = 0;
				return NULL;
			}
			
			if (l <= passLength) {
				
				buf.message[l - 1] = 0;

				if (strcmp(buf.message, passw) == 0) {
					break;
				}

			}

			sprintf(buf.message, "Incorrect password.");
			buf.time = time(NULL);
			if (writeMessage(cfd, &buf, sizeof(struct buffer)) < 0) {
				return NULL;
			}	
			
		}

	}

	block(&lmutex);
	addClient(client); //add the *structure to the clients list
	unblock(&lmutex);

	// the client registered

	sprintf(buf.name, "SERVER");
	buf.time = time(NULL);
	sprintf(buf.message, "Welcome, %s!", client[0].name);
	if (writeMessage(client[0].fd, &buf, sizeof(struct buffer)) < 0) {
		client[0].isActive = 0;
		return NULL;
	}

	printf("Client %s has been initialised.\n", client[0].name);

	// start receiving messages

	sprintf(buf.name, "%s", client[0].name);

	while(1) {

		pthread_testcancel(); // cancellation point
	
		l = readMessage(client[0].fd, buf.message, mesize);
		if (l < 0) {
			client[0].isActive = 0;
			return NULL;
		}

		buf.message[l - 1] = 0;

		buf.time = time(NULL);

		block(&lmutex);
		
		printf("Processing message from %s.\n", client[0].name);

		broadcast(&buf, sizeof(struct buffer) - mesize + l, client);
		cleanList();	

		unblock(&lmutex);

	}

	return NULL;

}

void * waitCommand(void *arg) {

	char command[10];

	int l;
	
	while(1) {

		l = read(STDIN_FILENO, command, 10);
		command[l - 1] = 0;

		if (strcmp(command, "exit") == 0) {
		
			int regfd = open("register", O_WRONLY);
	
			// copy reg into "register"

			writeFull(regfd, reg, regSize);
			check();

			close(regfd);
			check();

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
		heap[clN].isActive = 0; // mark the structures as empty
	}

	clN = 0; // clients number

	// the clients list initialised

	// 3. Prepare for multithread work
	
	errno = pthread_mutex_init(&lmutex, NULL);
	check();

	errno =  pthread_mutex_init(&rmutex, NULL);
	check();

	pthread_t thr;

	// thread waiting for commands from terminal
	errno = pthread_create(&thr, NULL, waitCommand, NULL);
	check();
	
	signal(SIGPIPE, ignore); //prevent crashing when writing

	// prepared for multithreading

	// 4. Load the clients register

	/*

	The structure of the register is the following:

	<size of the unit><name><password><size of the unit><...

	<size of the unit> has the type int and contains the
	number of bytes between the end of itself
	and the end of the unit

	*/

	int regfd = open("register", O_RDONLY | O_CREAT, S_IRWXU);
	check();

	#define unit 256

	int l;

	regSize = 0;

	// copy the file "register" into the reg array
	do {

		l = read(regfd, &reg[regSize], unit);
		check();

		regSize += l;
	
	} while (l);

	// the file copied

	close(regfd);
	check();

	// the register is ready

	// 5. Proceed to accept connections from clients

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
