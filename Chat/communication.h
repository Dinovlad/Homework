int readFull(int fd, void *buf, size_t count);

int writeFull(int fd, void *buf, size_t count);

int readMessage(int socketfd, void *buf, int bufsize);

int writeMessage(int socketfd, void *buf, int count);
