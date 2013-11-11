/*********************************************************************
 * 文件名称：testmain.c
 * 内容摘要：
 * 作    者：mcdragon
 * 完成日期：2013-06-08
 **********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
 
int main( int argc, char *argv[] )
{
    if(CU_initialize_registry()){
             fprintf(stderr, " Initialization of Test Registry failed. ");
             exit(EXIT_FAILURE);
     }else{
             AddTests();
             CU_set_output_filename("TestMax");
             CU_list_tests_to_file();
             CU_automated_run_tests();
             CU_cleanup_registry();
     }
     return 0;
}

