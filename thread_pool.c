/*********************************************************************
 * �ļ����ƣ�thread_pool.c
 * ����ժҪ��
 * ��    �ߣ�mcdragon
 * ������ڣ�2013-06-01
 **********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "thread_pool.h"

static int  JobQueInit(T_Job_Que *ptJobQue);
static void FreeAllJob(T_Job_Que *ptJobQue);
static void DestroyJob(T_Job *ptJob);
static void *ThreadFunc(T_Thread_Pool *ptTP);


/*********************************************************************
 * �������ƣ�CreateJob
 * ����������
 * �����������
 * �����������
 * �� �� ֵ����
 *
 * ����˵������
 * �޸�����        �汾��    �޸���    �޸�����
 * ---------------------------------------------------------------------
 * 2013-06-01        V1.0    mcdragon    ����
 ***********************************************************************/
T_Job* CreateJob(FUNC jobFunc, void *pFuncArg, int iArgLen)
{
    T_Job   *ptJob = NULL;
    void    *pArg  = NULL;

    if(NULL == jobFunc)
    {
        TP_Debug("Invalid Pointer!\n");
        return NULL;
    }

    ptJob = (T_Job *)malloc(sizeof(T_Job));
    if(NULL == ptJob)
    {
        TP_Debug("Memory Alloc for T_Job Failed!\n");
        return NULL;
    }
    memset(ptJob, 0, sizeof(T_Job));

    /* ����в��� */
    if( NULL != pFuncArg)
    {
        /* ��������Ϊ0�����쳣 */
        if(iArgLen <= 0)
        {
            TP_Debug("The length of argument is invalid %d!\n", iArgLen);
            TP_Free(ptJob);
            return NULL;
        }
        pArg = (void *)malloc(iArgLen);
        if(NULL == pArg)
        {
            TP_Debug("Memory Alloc for pArg Failed!\n");
            TP_Free(ptJob);
            return NULL;
        }
        /* ���Ʋ��� */
        memcpy(pArg, pFuncArg, iArgLen);
    }

    ptJob->jobFunc  = jobFunc;
    ptJob->pFuncArg = pArg;

    /* ��ʼ������ */
    INIT_LIST_HEAD(&ptJob->jobList);

    return ptJob;

}

/*********************************************************************
 * �������ƣ�DestroyJob
 * ����������
 * �����������
 * �����������
 * �� �� ֵ����
 *
 * ����˵������
 * �޸�����        �汾��    �޸���    �޸�����
 * ---------------------------------------------------------------------
 * 2013-06-01        V1.0    mcdragon    ����
 ***********************************************************************/
static void DestroyJob(T_Job *ptJob)
{
    if(NULL == ptJob)
    {
        TP_Debug("Invalid Pointer!\n");
        return;
    }

    TP_Free(ptJob->pFuncArg);
    return;
}

/*********************************************************************
 * �������ƣ�FreeAllJob
 * ����������
 * �����������
 * �����������
 * �� �� ֵ����
 *
 * ����˵������
 * �޸�����        �汾��    �޸���    �޸�����
 * ---------------------------------------------------------------------
 * 2013-06-01        V1.0    mcdragon    ����
 ***********************************************************************/
static void FreeAllJob(T_Job_Que *ptJobQue)
{
    T_Job *ptJob        = NULL;
    T_Job *ptNextJob    = NULL;

    if(NULL == ptJobQue)
    {
        return;
    }

    /* ��������Ϊ��,ֱ�ӷ��� */
    if(list_empty(&ptJobQue->jobList))
    {
        return;
    }

    /* ������ɾ���������� */
    list_for_each_entry_safe(ptJob, ptNextJob, &ptJobQue->jobList, jobList)
    {
        DestroyJob(ptJob);
        TP_Free(ptJob);
    }

    return;
}


/*********************************************************************
 * �������ƣ�JobQueInit
 * ����������
 * �����������
 * �����������
 * �� �� ֵ����
 *
 * ����˵������
 * �޸�����        �汾��    �޸���    �޸�����
 * ---------------------------------------------------------------------
 * 2013-06-01        V1.0    mcdragon    ����
 ***********************************************************************/
static int JobQueInit(T_Job_Que *ptJobQue)
{
    if(NULL == ptJobQue)
    {
        TP_Debug("T_Job_Que is NULL!\n");
        return FAIL;
    }

   if(0 != pthread_mutex_init(&ptJobQue->mutex, NULL))
   {
        TP_Debug("pthread_mutex_init failed!\n");
        return FAIL;
   }

   INIT_LIST_HEAD(&ptJobQue->jobList);

   return SUCCESS;
}


/*********************************************************************
 * �������ƣ�tp_thread_func
 * ����������
 * �����������
 * �����������
 * �� �� ֵ����
 *
 * ����˵������
 * �޸�����        �汾��    �޸���    �޸�����
 * ---------------------------------------------------------------------
 * 2013-06-01        V1.0    mcdragon    ����
 ***********************************************************************/
static void *ThreadFunc(T_Thread_Pool *ptTP)
{
    T_Job *ptJob = NULL;

    while(1)
    {
        /* �߳�����,�ȴ��ź��� */
        if(0 != sem_wait(ptTP->ptSem))
        {
            TP_Debug("thread waiting for semaphore....\n");
            exit(1);
        }

        /* �������� */
        pthread_mutex_lock(&ptTP->ptJobQue->mutex);

        /* ����һ��,������ȡ���� */
        if(list_empty(&ptTP->ptJobQue->jobList))
        {
            /* ����, �ٴ�˯�ߵȴ� */
            pthread_mutex_unlock(&ptTP->ptJobQue->mutex);
            continue;
        }

        /* ������, ȡ��һ���������� */
        ptJob = list_entry(ptTP->ptJobQue->jobList.next,
                           T_Job, jobList);
        list_del(&ptJob->jobList);
        pthread_mutex_unlock(&ptTP->ptJobQue->mutex);
        ptJob->jobFunc(ptJob->pFuncArg);
        /* ��������������� */
        DestroyJob(ptJob);
    }
    return NULL;
}


/*********************************************************************
 * �������ƣ�AddToJobQue
 * ����������
 * �����������
 * �����������
 * �� �� ֵ����
 *
 * ����˵������
 * �޸�����        �汾��    �޸���    �޸�����
 * ---------------------------------------------------------------------
 * 2013-06-01        V1.0    mcdragon    ����
 ***********************************************************************/
int AddToJobQue(T_Thread_Pool *ptThreadPool, T_Job *ptJob)
{
    if(NULL == ptThreadPool || NULL == ptJob)
    {
        TP_Debug("Invalid Pointer!\n");
        return FAIL;
    }

    pthread_mutex_lock(&ptThreadPool->ptJobQue->mutex);
    list_add(&ptJob->jobList, &ptThreadPool->ptJobQue->jobList);
    pthread_mutex_unlock(&ptThreadPool->ptJobQue->mutex);

    sem_post(ptThreadPool->ptSem);

    return SUCCESS;
}

/*********************************************************************
 * �������ƣ�CreateThreadPool
 * ����������
 * �����������
 * �����������
 * �� �� ֵ����
 *
 * ����˵������
 * �޸�����        �汾��    �޸���    �޸�����
 * ---------------------------------------------------------------------
 * 2013-06-01        V1.0    mcdragon    ����
 ***********************************************************************/
T_Thread_Pool* CreateThreadPool(int iPoolSize)
{
    int i = 0;
    T_Thread_Pool *ptTP = NULL;

    /* ��μ�� */
    if(iPoolSize <= 0 || iPoolSize > MAX_POOL_SIZE)
    {
        TP_Debug("Invalid Thread Pool Size %d! Proper Value: [0 ~ %d]\n",
                iPoolSize, MAX_POOL_SIZE);
        return NULL;
    }

    ptTP = (T_Thread_Pool *)malloc(sizeof(T_Thread_Pool));
    if(NULL == ptTP)
    {
        TP_Debug("Memory Alloc For Thread Pool Failed!\n");
        return NULL;
    }
    memset(ptTP, 0, sizeof(T_Thread_Pool));

    ptTP->pThreadArr = (pthread_t *)malloc(iPoolSize * sizeof(pthread_t));
    if(NULL == ptTP->pThreadArr)
    {
        TP_Debug("Memory Alloc For Thread Array Failed!\n");
        goto done;
    }

    /* �����ź��� */
    ptTP->ptSem = (sem_t *)malloc(sizeof(sem_t));
    if(NULL == ptTP->ptSem)
    {
        TP_Debug("Memory Alloc For Semaphore Failed!\n");
        goto done;
    }

    /* ��ʼ���ź��� */
    sem_init(ptTP->ptSem, 0, 0);

    ptTP->ptJobQue = (T_Job_Que *)malloc(sizeof(T_Job_Que));
    if(NULL == ptTP->ptJobQue)
    {
        TP_Debug("Memory Alloc For Job Queue Failed!\n");
        goto done;
    }

    /* ��ʼ��������� */
    if(SUCCESS !=  JobQueInit(ptTP->ptJobQue))
    {
         TP_Debug("JobQueInit Failed!\n");
         goto done;
    }


    /* ��ʼ���߳� */
    for(i = 0;i < iPoolSize; i++)
    {
        if (0 == pthread_create(&(ptTP->pThreadArr[i]), NULL, 
                                (void *)ThreadFunc,
                                (void *)ptTP))
        {
            ptTP->iThreadNum++;
        }
    }


    TP_Debug("Threads should created: %d! \n"
             "Threads actual created: %d! \n", iPoolSize, ptTP->iThreadNum);
    return ptTP;

done:
    if(NULL != ptTP->ptJobQue)
        TP_Free(ptTP->ptJobQue);
    if(NULL != ptTP->ptSem)
        TP_Free(ptTP->ptSem);
    if(NULL != ptTP->pThreadArr)
        TP_Free(ptTP->pThreadArr);
    if(NULL != ptTP)
        TP_Free(ptTP);
    return NULL;
}

/*********************************************************************
 * �������ƣ�DestroyThreadPool
 * ����������
 * �����������
 * �����������
 * �� �� ֵ����
 *
 * ����˵������
 * �޸�����        �汾��    �޸���    �޸�����
 * ---------------------------------------------------------------------
 * 2013-06-01        V1.0    mcdragon    ����
 ***********************************************************************/
void DestroyThreadPool(T_Thread_Pool *ptThreadPool)
{
    int i = 0;

    if(NULL == ptThreadPool)
    {
        TP_Debug("T_Thread_Pool is NULL!\n");
        return;
    }

    for(i = 0; i < ptThreadPool->iThreadNum; i++)
    {
        pthread_cancel(ptThreadPool->pThreadArr[i]);
        pthread_join(ptThreadPool->pThreadArr[i], NULL);
    }

    TP_Free(ptThreadPool->pThreadArr);

    if(0 != sem_destroy(ptThreadPool->ptSem))
    {
        TP_Debug("sem_destroy failed!\n");
    }

    /* �ͷų��ж���������� */
    pthread_mutex_lock(&ptThreadPool->ptJobQue->mutex);
    FreeAllJob(ptThreadPool->ptJobQue);
    pthread_mutex_unlock(&ptThreadPool->ptJobQue->mutex);

    /* �ͷų��ж��� */
    TP_Free(ptThreadPool->ptJobQue);

    return;

}
/* End of file */
