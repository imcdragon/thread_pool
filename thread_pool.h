/*********************************************************************
 * �ļ����ƣ�thread_pool.h
 * ����ժҪ��
 * ��    �ߣ�mcdragon
 * ������ڣ�2013-06-01
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


/* �̵߳��Ⱥ��� */
typedef void* (*FUNC)(void *pArg); 

/* ���� */
typedef struct job_t
{
    FUNC                jobFunc;    /* �������� */
    void                *pFuncArg;  /* ������������ */
    struct list_head    jobList;    /* �������� */
}T_Job;

/* ������� */
typedef struct job_que_t
{
    pthread_mutex_t     mutex;      /* ���л����� */
    struct list_head    jobList;    /* ������� */
}T_Job_Que;

/* �̳߳� */
typedef struct thread_pool_t
{
    pthread_t   *pThreadArr;        /* �߳����飬����̺߳� */
    int         iThreadNum;         /* �߳��� */
    T_Job_Que   *ptJobQue;          /* ������� */
    sem_t       *ptSem;             /* �̳߳��ź��� */
}T_Thread_Pool;

/* �̳߳ش������� */
T_Thread_Pool* CreateThreadPool(int iPoolSize);

/* �̳߳����ٺ��� */
void DestroyThreadPool(T_Thread_Pool *ptThreadPool);

/* ���̵߳Ĺ������������һ������job */
int AddToJobQue(T_Thread_Pool *ptThreadPool, T_Job *ptJob);

/* ����һ��job */
T_Job* CreateJob(FUNC jobFunc, void *pFuncArg, int iArgLen);


#endif
