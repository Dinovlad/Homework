#include <unistd.h>
#include <errno.h> 
#include "communication.h"

// the procedure does not return until count bytes are read
// or a error occurs
// returns 0 on success, -1 on falure
int readFull(int fd, void *buf, size_t count) {

	ssize_t n;

	while(count) {

		n = read(fd, buf, count);

		if (n < 0) {
			return -1;
		}

		buf = (void *) (buf + n);

		count -= n;

	}

	return 0;

}

/** 
The function reads the next message and
returns the number of bytes read and written to buf on success
and -1 on falure

if the buffer has insufficient capacity to hold the message,
errno is set to -2
*/
int readMessage(int socketfd, void *buf, int bufsize) {

	int size;
	
	if (readFull(socketfd, &size, 4) < 0) { // read the size of the message
		return -1;
	}
	
	if (bufsize < size) {

		errno = -2;

		return -1;

	}

	if (readFull(socketfd, buf, size) < 0) {
		return -1;
	}

	return size;

}

int writeFull(int fd, void *buf, size_t count) {

	ssize_t n;

	while(count) {

		n = write(fd, buf, count);
		
		if (n < 0) {
			return -1;
		}

		buf = (void *) (buf + n);

		count -= n;

	}

	return 0;

}

int writeMessage(int socketfd, void *buf, int count) {

	if (writeFull(socketfd, &count, 4) < 0) {
		return -1;
	}

	if (writeFull(socketfd, buf, count) < 0) {
		return -1;
	}

	return 0;

}
