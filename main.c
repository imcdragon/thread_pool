/*********************************************************************
 * �ļ����ƣ�main.c
 * ����ժҪ��
 * ��    �ߣ�mcdragon
 * ������ڣ�2013-06-01
 **********************************************************************/
#include "thread_pool.h"
#include <unistd.h>

#define MAX_JOB_NUM 5

/* �Զ����߳�ִ�к��� */
void* thread_func1(void *arg)
{  
    printf("consumer<<<<< job1(*)...by Thread  :%u\n",
           (unsigned int)pthread_self());  
    return NULL;
}  
  
/* �Զ����߳�ִ�к��� */
void* thread_func2(void *arg)
{  
    printf("consumer<<<<< job2(%d)...by Thread  :%u\n", 
           (*(int*)arg), (unsigned int)pthread_self());
    return NULL;
}  


/* �����ߺ��� */
void* thread_func_producer(T_Thread_Pool *ptTP)
{
    int     iJobNum = 0;
    T_Job   *ptJob   = NULL;
    
    while(iJobNum < MAX_JOB_NUM)
    {  
        printf("producer>>>>>add a job(job1) to job quene!\n");
        ptJob = CreateJob(thread_func1, NULL, 0);
        AddToJobQue(ptTP, ptJob);  
        sleep(1);  
        printf("producer>>>>>add a job(job2) to job quene!\n");  
        ptJob = CreateJob(thread_func2, (void*)&iJobNum, sizeof(iJobNum));
        AddToJobQue(ptTP, ptJob);
        iJobNum++;
    }

    return NULL;
}  



int main(int argc, char *argv[])
{
    pthread_t       pidProducer;
    int             iPoolSize       = 5;    
    T_Thread_Pool   *ptThreadPool   = NULL;

    ptThreadPool = CreateThreadPool(iPoolSize);
    if(NULL == ptThreadPool)
    {
        TP_Debug("CreateThreadPool Failed!\n");
        goto done;
    }
    TP_Debug("CreateThreadPool Success!\n");
    
    /* ����һ�������߲�������job */
   if(0 != pthread_create(&pidProducer, NULL,
                          (void *)thread_func_producer,
                          (void *)ptThreadPool))
   {
        goto done;
   }
    
    pthread_join(pidProducer,NULL);
    TP_Debug("Producer exit...\n");
    
    DestroyThreadPool(ptThreadPool);
    TP_Debug("DestroyThreadPool Success!\n");
    
done:
    return 0;
}

