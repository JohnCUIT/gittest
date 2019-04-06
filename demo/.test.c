#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "mutexwatch.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char* argv[])
{
#if 0
	pthread_t tidA;
	pthread_t tidB;
	pthread_create();
#endif
	int i = 0;
	pthread_mutex_lock(&mutex);
	i++;
	pthread_mutex_unlock(&mutex);
	return 0;
}

void *threadA(void)
{
	while(1)
	{
		sleep(3);
		printf("====DEBUG[Johnny][%s][%s][%d]====\n", __FILE__,  __func__, __LINE__);
	}
}
