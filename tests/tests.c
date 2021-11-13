/*************************************************************************************************/
/*  Testing suite                                                                                */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include <stdio.h>

#include "_thread.h"
#include "testing.h"
#include "tests.h"



/*************************************************************************************************/
/*  Macros                                                                                       */
/*************************************************************************************************/

#define TEST(x) tst_suite_add(&suite, #x, dvz_test_##x, NULL);



/*************************************************************************************************/
/*  Tests                                                                                        */
/*************************************************************************************************/

static int _thread_callback(void* user_data)
{
    ASSERT(user_data != NULL);
    dvz_sleep(10);
    *((int*)user_data) = 42;
    log_debug("from thread");
    return 0;
}

int dvz_test_thread_1(TstSuite* suite)
{
    ASSERT(suite != NULL);
    int data = 0;
    DvzThread thread = dvz_thread(_thread_callback, &data);
    AT(data == 0);
    dvz_thread_join(&thread);
    AT(data == 42);
    return 0;
}



static int _cond_callback(void* user_data)
{
    ASSERT(user_data != NULL);
    DvzCond* cond = (DvzCond*)user_data;
    dvz_sleep(10);
    dvz_cond_signal(cond);
    return 0;
}

int dvz_test_cond_1(TstSuite* suite)
{
    ASSERT(suite != NULL);
    DvzCond cond = dvz_cond();
    DvzMutex mutex = dvz_mutex();

    DvzThread thread = dvz_thread(_cond_callback, &cond);
    dvz_cond_wait(&cond, &mutex);

    dvz_thread_join(&thread);
    dvz_mutex_destroy(&mutex);
    dvz_cond_destroy(&cond);
    return 0;
}



/*************************************************************************************************/
/*  Entry-point                                                                                  */
/*************************************************************************************************/

int dvz_run_tests()
{
    TstSuite suite = tst_suite();

    TEST(thread_1)
    TEST(cond_1)

    tst_suite_run(&suite, NULL);
    tst_suite_destroy(&suite);
    return 0;
}
