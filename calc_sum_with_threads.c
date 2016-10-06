#include <stdio.h>
#include <pthread.h>

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

pthread_mutex_t l;

#define lck pthread_mutex_lock
#define unl pthread_mutex_unlock

unsigned long long int S = 0;

int step;

void* thrd(void* arg) {

	unsigned long long int sum = 0;

	int i;
	int to = ((int) arg) + step;
	
	for(i = ((int) arg); i < to; i++) {
		sum += i;
	}

	lck(&l);
	S += sum;
	unl(&l);

	return NULL;

}

int main(int argc, char **argv) {

	int from = intValue(argv[1]);
	int to = intValue(argv[2]);
	int N = intValue(argv[3]);

	step = (to - from) / N;
	N--;

	pthread_mutex_init(&l, NULL);

	int i;

	pthread_t id[N];

	for (i = 0; i < N; i++) {

		if(pthread_create(&id[i], NULL, thrd,(void *) from)) {

			printf("Too many threads are requested.\n");

			return -1;

		}

		from += step;
	
	}

	unsigned long long int sum = 0;

	for(i = from; i <= to; i++) {
		sum += i;
	} 

	lck(&l);
	S += sum;
	unl(&l);

	for(i = 0; i < N; i++) {
		pthread_join(id[i], (void **) NULL);
	}

	printf("The sum: %llu\n", S);

	pthread_mutex_destroy(&l);

	return 0;

}
