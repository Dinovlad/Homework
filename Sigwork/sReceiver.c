#include <signal.h> //signal
#include <stdio.h> //printf
#include <stdlib.h> //exit
#include <unistd.h> //sleep
#include <sys/wait.h> //waitpid, WHOHANG

char message[256];
char *s;
int currentBit;

void zero(int n) {

	(*s) <<= 1;

	currentBit++;

	if (currentBit == 8) {
		
		currentBit = 0;
		
		s++;

	}

}

void one(int n) {

	(*s) = ((*s) << 1) + 1;

	currentBit++;

	if (currentBit == 8) {
		
		currentBit = 0;
		
		s++;

	}

}

void rd(int n) { // read

	(*s) = 0;

	if(currentBit != 0) {

		printf("Information lost.\n");

		currentBit = 0;

	}

	s = message;

	printf("Message received:\n%s\n", s);

}

int main() {

	s = message;
	currentBit = 0;

	signal(30, zero);
	signal(31, one);	
	signal(10, rd);	

	printf("Pid of the process: %d.\n", getpid());

	while(1) {
		sleep(1000000);
	}

}
