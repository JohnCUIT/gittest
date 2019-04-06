/*===============================================================
*   Copyright (C) 2018 All rights reserved.
*   
*   File Name:	mutexwatch.c
*   Creater:		Shengjiang He
*   Create Date:	2018-06-05
*   Detail:		
*
*   Update Log:	
*
================================================================*/
#include <stdlib.h>
#include "mutexwatch.h"

typedef	enum
{
	UNLOCK = 0,
	LOCKED,
	WAITTING,
}MUTEX_STATU;

typedef	struct
{
	NODE node;
	pthread_mutex_t* mutex;
	MUTEX_STATU lockStatu;
	int lockThread;
	int curThread;
}MUTEXINFO_NODE;

static MUTEX_MANEGER* mmaneger = NULL;


char isDeadLock(MUTEX_MANEGER* this, pthread_mutex_t* mutex);
void mutexCheckLock(MUTEX_MANEGER* this, pthread_mutex_t* mutex)
{
	MUTEXINFO_NODE* mutex_node = (MUTEXINFO_NODE*)malloc(sizeof(MUTEXINFO_NODE));
	mutex_node->mutex = mutex;
	mutex_node->lockStatu = WAITTING;
	mutex_node->lockThread = gettid();

	lstAdd(&(this->mutexList), &(mutex_node->node));

	if (isDeadLock(this, mutex))
	{
		printf("[ERROR]mutex:0x%x maybe cause a deadLock!\n", (unsigned int)mutex);
	}
}

void mutexGetLock(MUTEX_MANEGER* this, pthread_mutex_t* mutex)
{
	MUTEXINFO_NODE* mutexinfo_node = NULL;
	for(mutexinfo_node = (MUTEXINFO_NODE*)lstFirst(&(this->mutexList)); mutexinfo_node != NULL; mutexinfo_node = (MUTEXINFO_NODE*)lstNext(&(mutexinfo_node->node)))
	{
		if (mutexinfo_node->mutex == mutex && mutexinfo_node->lockThread == gettid())//
		{
			mutexinfo_node->lockStatu = LOCKED;
			break;
		}
	}
}

void mutexUnlock(MUTEX_MANEGER* this, pthread_mutex_t* mutex)
{
	MUTEXINFO_NODE* mutexinfo_node = NULL;
	for(mutexinfo_node = (MUTEXINFO_NODE*)lstFirst(&(this->mutexList)); mutexinfo_node != NULL; mutexinfo_node = (MUTEXINFO_NODE*)lstNext(&(mutexinfo_node->node)))
	{
		if (mutexinfo_node->mutex == mutex && mutexinfo_node->lockThread == gettid())//
		{
			mutexinfo_node->lockStatu = UNLOCK;
			break;
		}
	}
	lstDelete(&(this->mutexList), &(mutexinfo_node->node));
	free(mutexinfo_node);
}

int mutexLockedInThread(MUTEX_MANEGER* this, pthread_mutex_t* mutex)
{
	MUTEXINFO_NODE* mutexinfo_node = NULL;
	if (NULL == mutex)
	{
		return 0;
	}
	for(mutexinfo_node = (MUTEXINFO_NODE*)lstFirst(&(this->mutexList)); mutexinfo_node != NULL; mutexinfo_node = (MUTEXINFO_NODE*)lstNext(&(mutexinfo_node->node)))
	{
		if (mutexinfo_node->mutex == mutex && mutexinfo_node->lockStatu == LOCKED)//如果锁已被占用
		{
			return mutexinfo_node->lockThread;
		}
	}
	return 0;
}

pthread_mutex_t* threadWaitFoMutex(MUTEX_MANEGER* this, int tid)
{
	MUTEXINFO_NODE* mutexinfo_node = NULL;
	for(mutexinfo_node = (MUTEXINFO_NODE*)lstFirst(&(this->mutexList)); mutexinfo_node != NULL; mutexinfo_node = (MUTEXINFO_NODE*)lstNext(&(mutexinfo_node->node)))
	{
		if (mutexinfo_node->lockThread == tid && mutexinfo_node->lockStatu == WAITTING)//如果锁已被占用
		{
			return mutexinfo_node->mutex;
		}
	}
	return NULL;
}

char isDeadLock(MUTEX_MANEGER* this, pthread_mutex_t* mutex)
{
	pthread_mutex_t* tmp_mutex = mutex;
	long int tid = 0;
	while (1)
	{
		tid = mutexLockedInThread(this, tmp_mutex);
		if (0 == tid)		//该锁没有被占用，则一定不会死锁
		{
			printf("tid=0.\n");
			return 0;
		}
		else if (gettid() == tid)			//该锁被本线程占用，则一定会死锁
		{
			printf("tid=%ld.\n", tid);
			return 1;
		}
		tmp_mutex = threadWaitFoMutex(this, tid);	//该锁被其他线程占用。继续判断占用该锁的线程(tid)被哪个锁阻塞。
		printf("loop,tid=%ld.\n", tid);
	}
	return 0;
}

MUTEX_MANEGER* getMutexManeger()
{
	if (mmaneger != NULL)
	{
		return mmaneger;
	}
	mmaneger = (MUTEX_MANEGER*)malloc(sizeof(MUTEX_MANEGER));
	mmaneger->mutex_check_lock = mutexCheckLock;
	mmaneger->mutex_get_lock = mutexGetLock;
	mmaneger->mutex_unlock = mutexUnlock;
	pthread_spin_init(&(mmaneger->mmaneger_lock), 0);
//	pthread_mutex_init(&(mmaneger->mmaneger_lock), 0);
	lstInit(&(mmaneger->mutexList));
	mmaneger->is_deadlock = 0;

	return mmaneger;
}

void freeMutexManeger()
{
	if (NULL != mmaneger)
	{
		pthread_spin_destroy(&(mmaneger->mmaneger_lock));
//		pthread_mutex_destroy(&(mmaneger->mmaneger_lock));
		free(mmaneger);
		mmaneger = NULL;
	}
}


#if 0
int hik_pthread_mutex_lock (pthread_mutex_t *mutex)
{
	struct timeval tv_begin, tv_end;
	long int waitTime_s = 0;
	long int waitTime_us = 0;
	MUTEX_MANEGER* mng = getMutexManeger();

	printf("====DEBUG[Johnny][%s][%s][%d]====\n", __FILE__, __func__, __LINE__);
//	pthread_mutex_lock(&(mng->mmaneger_lock));
	mng->mutex_check_lock(mng, mutex);
//	pthread_mutex_unlock(&(mng->mmaneger_lock));
	gettimeofday(&tv_begin, NULL);
//	printf("thread %ld waiting for %s.\n",gettid(), #x);
	pthread_mutex_lock(mutex);
	gettimeofday(&tv_end, NULL);
//	pthread_mutex_lock(&(mng->mmaneger_lock));
	mng->mutex_get_lock(mng, mutex);
//	pthread_mutex_unlock(&(mng->mmaneger_lock));
	waitTime_us = (tv_end.tv_usec - tv_begin.tv_usec >= 0)?(tv_end.tv_usec - tv_begin.tv_usec):(1000*1000 + tv_end.tv_usec - tv_begin.tv_usec);
	waitTime_s = (tv_end.tv_usec - tv_begin.tv_usec >= 0)?(tv_end.tv_sec - tv_begin.tv_sec):(tv_end.tv_sec - tv_begin.tv_sec - 1);

//	printf("====DEBUG[Johnny][%s][%d][thread:%ld waiting:%s for %ld.%06lds]====\n", __FILE__, __LINE__, gettid(), #x, waitTime_s, waitTime_us);

}


int hik_pthread_mutex_unlock (pthread_mutex_t *mutex)
{
	MUTEX_MANEGER* mng = getMutexManeger();
	pthread_mutex_unlock(mutex);
//	pthread_mutex_lock(&(mng->mmaneger_lock));
	mng->mutex_unlock(mng, mutex);
//	pthread_mutex_unlock(&(mng->mmaneger_lock));
	printf("====DEBUG[Johnny][%s][%s][%d]====\n", __FILE__, __func__, __LINE__);
//	printf("====DEBUG[Johnny][%s][%d][thread:%ld unlocked:%s]====\n", __FILE__, __LINE__, gettid(), #x);
}
#endif
