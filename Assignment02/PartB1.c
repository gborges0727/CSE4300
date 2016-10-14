#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#define NUM_THREADS 5

int last_threadID = 1;
int threadRunCounts[NUM_THREADS] = {0};
int queueSize = 15;
int producerSleep = 0;
int consumerSleep = 0;

pthread_mutex_t myTurnMutex;
pthread_cond_t myTurnSignal;

int checkArray(int arrayToCheck[]) {
	for (int i = 0; i < NUM_THREADS; i++){
		int x = threadRunCounts[i];
		printf("Thread number %d has been run %d times\n", i, x);
		if(threadRunCounts[i] >= 10)
			return false;
	}
	return true;
}

void *threadHello(void *t) {
	long my_id = (long)t;

	pthread_mutex_lock(&myTurnMutex);
	while(checkArray(threadRunCounts)) {
		threadRunCounts[my_id - 1] += 1;
		if((long)last_threadID != my_id) {
			printf("Not my turn: %ld\n", my_id);
			pthread_cond_signal(&myTurnSignal);
			pthread_cond_wait(&myTurnSignal, &myTurnMutex);
			continue;
		}
		else {
			printf("My turn: %ld\n", my_id);
			if(last_threadID == 5)
				last_threadID = 1;
			else
				last_threadID++;

			pthread_cond_signal(&myTurnSignal);
			pthread_cond_wait(&myTurnSignal, &myTurnMutex);
		}
	}
	pthread_mutex_unlock(&myTurnMutex);
	pthread_cond_signal(&myTurnSignal);
	pthread_exit(NULL);

}

int main(int argc, char* argv[]) {
	pthread_t threads[NUM_THREADS];
	int rc;
	long t;
	pthread_attr_t attr;

	// Initialize mutex and condition variable objects
	pthread_mutex_init(&myTurnMutex, NULL);
	pthread_cond_init (&myTurnSignal, NULL);

	// Create threads
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	for(t = 1; t <= NUM_THREADS; t++) {
		printf("In main: creating thread %ld\n", t);
		rc = pthread_create(&threads[t], &attr, threadHello, (void *)t);
	}

	// Wait for all threads to complete */
	for (int i=0; i<NUM_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}

	// Clean up and exit 
	pthread_attr_destroy(&attr);
	pthread_mutex_destroy(&myTurnMutex);
	pthread_cond_destroy(&myTurnSignal);
	pthread_exit(NULL);
}
