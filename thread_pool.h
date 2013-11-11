/*********************************************************************
 * 文件名称：thread_pool.h
 * 内容摘要：
 * 作    者：mcdragon
 * 完成日期：2013-06-01
 **********************************************************************/
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include "list.h"

#define DEBUG

#ifdef DEBUG
#define TP_Debug(...) printf(__VA_ARGS__);
#else
#define TP_Debug(...) 
#endif

#define MAX_POOL_SIZE   10

#define SUCCESS         0
#define FAIL            1

#define TP_Free(ptr)\
    free(ptr);\
    ptr = NULL;


/* 线程调度函数 */
typedef void* (*FUNC)(void *pArg); 

/* 任务 */
typedef struct job_t
{
    FUNC                jobFunc;    /* 任务处理函数 */
    void                *pFuncArg;  /* 任务处理函数参数 */
    struct list_head    jobList;    /* 任务链表 */
}T_Job;

/* 任务队列 */
typedef struct job_que_t
{
    pthread_mutex_t     mutex;      /* 队列互斥锁 */
    struct list_head    jobList;    /* 任务队列 */
}T_Job_Que;

/* 线程池 */
typedef struct thread_pool_t
{
    pthread_t   *pThreadArr;        /* 线程数组，存放线程号 */
    int         iThreadNum;         /* 线程数 */
    T_Job_Que   *ptJobQue;          /* 任务队列 */
    sem_t       *ptSem;             /* 线程池信号量 */
}T_Thread_Pool;

/* 线程池创建函数 */
T_Thread_Pool* CreateThreadPool(int iPoolSize);

/* 线程池销毁函数 */
void DestroyThreadPool(T_Thread_Pool *ptThreadPool);

/* 向线程的工作队列中添加一个任务job */
int AddToJobQue(T_Thread_Pool *ptThreadPool, T_Job *ptJob);

/* 创建一个job */
T_Job* CreateJob(FUNC jobFunc, void *pFuncArg, int iArgLen);


#endif
