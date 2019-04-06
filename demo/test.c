#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "mutexwatch.h"

pthread_mutex_t lockA = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lockB = PTHREAD_MUTEX_INITIALIZER;

void *threadA(void* args);
void *threadB(void* args);
void *threadC(void* args);

int main(int argc, char* argv[])
{
	pthread_t tidA;
	pthread_t tidB;
	pthread_t tidC;
	pthread_mutex_init(&lockA, NULL);
	pthread_mutex_init(&lockB, NULL);
	pthread_create(&tidA, NULL, threadA, (void*)NULL);
	pthread_create(&tidB, NULL, threadB, (void*)NULL);
	pthread_create(&tidC, NULL, threadC, (void*)NULL);

	pthread_join(tidA, NULL);
	pthread_join(tidB, NULL);
	pthread_join(tidC, NULL);

	return 0;
}

void *threadA(void* args)
{
	while(1)
	{
		sleep(3);
		pthread_mutex_lock(&lockA);
		sleep(3);
		pthread_mutex_lock(&lockB);
		sleep(3);
		pthread_mutex_unlock(&lockB);
		sleep(3);
		pthread_mutex_unlock(&lockA);
	}
}

void *threadB(void* args)
{
	while(1)
	{
		pthread_mutex_lock(&lockB);
		sleep(1);
		pthread_mutex_lock(&lockA);
		sleep(1);
		pthread_mutex_unlock(&lockA);
		sleep(1);
		pthread_mutex_unlock(&lockB);
		sleep(3);
	}
}

void *threadC(void* args)
{
	while(1)
	{
		sleep(2);
#if 0
		pthread_mutex_lock(&lockA);
		sleep(1);
		pthread_mutex_lock(&lockB);
		sleep(2);
		pthread_mutex_unlock(&lockA);
		sleep(1);
		pthread_mutex_unlock(&lockB);
#endif
	}
}
