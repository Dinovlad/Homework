#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

/** 
 * The programme writes the string argv[3] to the memory
 * unit with the key argv[2] if argv[1] is "write",
 * reads the string by the key argv[2] if argv[1] is "read",
 * waits for a string to be written by the key
 * and reads it if argv[1] is "wait".
 */

int intValue(char *s) {

        int i = 1;

        unsigned int result = s[0] - '0';

        while(s[i] != 0) {

                result *= 10;

                result += s[i] - '0';

                i++;

        }

        return result;

}

int main(int argc, char **argv) {

	if (strcmp(argv[1], "write") == 0) {

		int N = strlen(argv[3]);

		char *message;
		int shmid;
		key_t key = intValue(argv[2]);

		shmid = shmget(key, N + 2, 0666 |IPC_CREAT | IPC_EXCL);

		if (shmid == -1) {

			shmid = shmget(key, N + 2, 0);
		
			if (shmid == -1) {

				printf("Memory unit cannot be created.\n");

				return -1;

			}
	
		}

		message = (char *) shmat(shmid, NULL, 0);

		sprintf(&message[1], "%s\0", argv[3]);
		message[N + 1] = 0; // close the string
		message[0] = 1; // trigger the flag

		printf("The message was sent.\n");

		shmdt(message);

	} else if (strcmp(argv[1], "read") == 0) {
		
		char *message;
		int shmid;
		key_t key = intValue(argv[2]);

		shmid = shmget(key, 2, 0);

		if (shmid == -1) {

			printf("Memory unit cannot be accessed by the specified key.\n");

			return -1;
	
		}

		message = (char *) shmat(shmid, NULL, 0);

		if (message[0] == 0) {
			
			printf("No message to read.\n");

			return 0;

		}
		
		message[0] = 0;

		printf("The message received:\n%s\n", &message[1]);

		shmdt(message);

	} else if (strcmp(argv[1], "wait") == 0) {

		char *mes;
		key_t key = intValue(argv[2]);
		int shmid = shmget(key, 2, 0);

		if (shmid == -1) {

			printf("Memory unit cannot be accessed by the specified key.\n");

			return -1;
	
		}

		mes = (char *) shmat(shmid, NULL, 0);

		while(mes[0] == 0) {
			sleep(0.25);
		}
		
		mes[0] = 0;

		printf("Message received:\n%s\n", &mes[1]);

		shmdt(mes);

	} else {

		printf("Illegal argument.\n");

		return -1;

	}

	return 0;

}
