/*************************************************************************************************/
/*  Testing suite                                                                                */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include <stdio.h>

#include "testing.h"
#include "tests.h"



/*************************************************************************************************/
/*  Tests                                                                                        */
/*************************************************************************************************/

int dvz_test_1(TstSuite* suite)
{
    ASSERT(suite != NULL);
    printf("test 1\n");
    return 0;
}



/*************************************************************************************************/
/*  Entry-point                                                                                  */
/*************************************************************************************************/

int dvz_run_tests()
{
    TstSuite suite = tst_suite();

    tst_suite_add(&suite, "test_1", dvz_test_1, NULL);

    tst_suite_run(&suite, NULL);
    tst_suite_destroy(&suite);
    return 0;
}
