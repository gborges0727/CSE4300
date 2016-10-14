#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#define NUM_THREADS 3

int threadRunCounts[NUM_THREADS] = {0};
int producerSleep = 0;
int consumerSleep = 0;
int queueMaxSize = 45;
int queue = 0;

pthread_mutex_t queueMutex;

int checkFinished() {
	if((threadRunCounts[0] || threadRunCounts[1]) && threadRunCounts[2] > 1000)
		return false;
	else
		return true;
}

void *producerFunc(void *t) {
	long my_id = (long)t;
	while(checkFinished()) {
		threadRunCounts[my_id - 1] += 1;
		pthread_mutex_lock(&queueMutex);
		if(queue >= queueMaxSize) {
			producerSleep ++;
			pthread_mutex_unlock(&queueMutex);
			continue;
		}
		queue++;
		pthread_mutex_unlock(&queueMutex);
	}
	pthread_exit(NULL);
}

void *consumerFunc(void *t) {
	long my_id = (long)t;

	while(checkFinished()) {
		threadRunCounts[my_id - 1] += 1;
		pthread_mutex_lock(&queueMutex);
		if(queue <= 0) {
			consumerSleep++;
			pthread_mutex_unlock(&queueMutex);
			continue;
		}
		queue--;
		pthread_mutex_unlock(&queueMutex);
	}
	pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
	pthread_t threads[NUM_THREADS];
	long t1 = 1, t2 = 2, t3 = 3;
	int rc;
	long t;
	pthread_attr_t attr;

	// Initialize mutex and condition variable objects
	pthread_mutex_init(&queueMutex, NULL);

	// Create threads
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_create(&threads[0], &attr, consumerFunc, (void *)t1);
	pthread_create(&threads[1], &attr, producerFunc, (void *)t2);
	pthread_create(&threads[2], &attr, producerFunc, (void *)t3);

	// Wait for all threads to complete */
	for (int i=0; i<NUM_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}

	// Create output
	printf("For a queue size of: %d , the total number of times the consumer went to sleep was %d , and the total number of times the producer went to sleep was %d\n", queue, consumerSleep, producerSleep);

	// Clean up and exit 
	pthread_attr_destroy(&attr);
	pthread_mutex_destroy(&queueMutex);
	pthread_exit(NULL);
}
