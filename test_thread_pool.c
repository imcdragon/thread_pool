/*********************************************************************
 * 文件名称：test_thread_pool.c
 * 内容摘要：
 * 作    者：mcdragon
 * 完成日期：2013-06-08
 **********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "CUnit/CUnit.h"
#include "CUnit/Automated.h"
#include "thread_pool.h"


/*********************************************************************
 *                          待测试函数                               *
 *********************************************************************/

extern T_Thread_Pool* CreateThreadPool(int iPoolSize);

/* 线程池销毁函数 */
extern void DestroyThreadPool(T_Thread_Pool *ptThreadPool);

/* 向线程的工作队列中添加一个任务job */
extern int AddToJobQue(T_Thread_Pool *ptThreadPool, T_Job *ptJob);

/* 创建一个job */
extern T_Job* CreateJob(FUNC jobFunc, void *pFuncArg, int iArgLen);


/*********************************************************************
 *                          Test Cases                               *
 *********************************************************************/
void myfun(void* a)
{ return;}
void TestCreateTP(void)
{
    T_Thread_Pool *ptTP = NULL;
    
    CU_ASSERT_PTR_NULL(ptTP = CreateThreadPool(-1));
    CU_ASSERT_PTR_NULL(CreateThreadPool(0));
    CU_ASSERT_PTR_NOT_NULL(ptTP = CreateThreadPool(1));
    CU_PASS(DestroyThreadPool(ptTP));
    CU_ASSERT_PTR_NULL(ptTP = CreateThreadPool(10000));   
}


void TestCreateJob(void)
{
    int a;
    FUNC b;
    
    CU_ASSERT_PTR_NULL(CreateJob(NULL, NULL, 0));
    CU_ASSERT_PTR_NULL(CreateJob(NULL, NULL, 1));
    CU_ASSERT_PTR_NULL(CreateJob(NULL, &a, 1));
    CU_ASSERT_PTR_NULL(CreateJob(b, &a, -1));
    CU_ASSERT_PTR_NULL(CreateJob(b, &a, 0));
    CU_ASSERT_PTR_NOT_NULL(CreateJob(b, NULL, 1));
    CU_ASSERT_PTR_NOT_NULL(CreateJob(b, &a, 1));
    CU_ASSERT_PTR_NOT_NULL(CreateJob(b, &a, sizeof(a)));
}

void TestAddToJobQue(void)
{
    T_Thread_Pool *ptTP  = NULL;
    T_Job         *ptJob = NULL;
    FUNC          b;
    int           *a;

    a = (int*)malloc(sizeof(int));
    
    CU_ASSERT_PTR_NOT_NULL(ptTP = CreateThreadPool(1));
    CU_ASSERT_PTR_NOT_NULL(ptJob = CreateJob((FUNC)(myfun), a, sizeof(int)));
    CU_ASSERT_EQUAL(AddToJobQue(NULL, NULL), FAIL);
    CU_ASSERT_EQUAL(AddToJobQue(NULL, ptJob), FAIL);
    CU_ASSERT_EQUAL(AddToJobQue(ptTP, NULL), FAIL);
    CU_ASSERT_EQUAL(AddToJobQue(ptTP, ptJob), SUCCESS);
    CU_PASS(DestroyThreadPool(ptTP));
    free(a);
}


CU_TestInfo testcases[] = {
        {"Testing CreateThreadPool:", TestCreateTP},
        {"Testing CreateJob:", TestCreateJob},
        {"Testing AddToJobQue:", TestAddToJobQue},
        CU_TEST_INFO_NULL
};


/*********************************************************************
 *                          Test Suites                              *
 *********************************************************************/
 
int suite_success_init(void) { return 0; }
int suite_success_clean(void) { return 0; } 
CU_SuiteInfo suites[] = 
{
    {"Testing Thread Pool:", suite_success_init, suite_success_clean, testcases},
    CU_SUITE_INFO_NULL
};


/*********************************************************************
 *                      Setting Enviroment                           *
 *********************************************************************/
void AddTests(void)
{
        assert(NULL != CU_get_registry());
        assert(!CU_is_test_running());
        /**//* shortcut regitry */

        if(CUE_SUCCESS != CU_register_suites(suites)){
                fprintf(stderr, "Register suites failed - %s ", CU_get_error_msg());
                exit(EXIT_FAILURE);
        }
}


