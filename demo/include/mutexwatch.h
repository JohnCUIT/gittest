/*===============================================================
*   Copyright (C) 2018 All rights reserved.
*   
*   File Name:	mutexwatch.h
*   Creater:		Shengjiang He
*   Create Date:	2018-06-06
*   Detail:		
*
*   Update Log:	
*
================================================================*/
#ifndef __MUTEXWATCH_H_
#define __MUTEXWATCH_H_

#include <pthread.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
//#include "memwatch.h"
//#include "lstLib.h"
#include "lstLib.h"

#define gettid() syscall(__NR_gettid)

typedef	struct deadlock_info
{
	int is_deadlock;
}DEADLOCK_INFO;

typedef	struct mutex_maneger
{
	LIST mutexList;
	void (*mutex_check_lock)(struct mutex_maneger* this, pthread_mutex_t* mutex);
	void (*mutex_get_lock)(struct mutex_maneger* this, pthread_mutex_t* mutex);
	void (*mutex_unlock)(struct mutex_maneger* this, pthread_mutex_t* mutex);
	pthread_spinlock_t mmaneger_lock;
//	pthread_mutex_t mmaneger_lock;
	int is_deadlock;
}MUTEX_MANEGER;

MUTEX_MANEGER* getMutexManeger();
//#define pthread_mutex_lock(x)	hik_pthread_mutex_lock(x)

#if 1
#define pthread_mutex_lock(x)	\
do{																										\
	struct timeval tv_begin, tv_end;																	\
	long int waitTime_s = 0;\
	long int waitTime_us = 0;\
	MUTEX_MANEGER* mng = getMutexManeger();\
\
	pthread_spin_lock(&(mng->mmaneger_lock));\
	mng->mutex_check_lock(mng, x);\
	pthread_spin_unlock(&(mng->mmaneger_lock));\
	gettimeofday(&tv_begin, NULL);\
	printf("thread %ld waiting for %s.\n",gettid(), #x);\
	pthread_mutex_lock(x);\
	gettimeofday(&tv_end, NULL);\
	pthread_spin_lock(&(mng->mmaneger_lock));\
	mng->mutex_get_lock(mng, x);\
	pthread_spin_unlock(&(mng->mmaneger_lock));\
	waitTime_us = (tv_end.tv_usec - tv_begin.tv_usec >= 0)?(tv_end.tv_usec - tv_begin.tv_usec):(1000*1000 + tv_end.tv_usec - tv_begin.tv_usec);\
	waitTime_s = (tv_end.tv_usec - tv_begin.tv_usec >= 0)?(tv_end.tv_sec - tv_begin.tv_sec):(tv_end.tv_sec - tv_begin.tv_sec - 1);\
\
	printf("====DEBUG[Johnny][%s][%d][thread:%ld waiting:%s for %ld.%06lds]====\n", __FILE__, __LINE__, gettid(), #x, waitTime_s, waitTime_us);\
\
}while(0);
#endif

//#define pthread_mutex_unlock(x)	hik_pthread_mutex_unlock(x)
#if 1
#define pthread_mutex_unlock(x)	\
do{\
	MUTEX_MANEGER* mng = getMutexManeger();\
	pthread_mutex_unlock(x);\
	pthread_spin_lock(&(mng->mmaneger_lock));\
	mng->mutex_unlock(mng, x);\
	pthread_spin_unlock(&(mng->mmaneger_lock));\
	printf("====DEBUG[Johnny][%s][%d][thread:%ld unlocked:%s]====\n", __FILE__, __LINE__, gettid(), #x);\
}while(0);
#endif


#endif //__MUTEXWATCH_H_

