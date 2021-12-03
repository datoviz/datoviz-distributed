/*************************************************************************************************/
/*  Testing pipelib                                                                              */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include "test_pipelib.h"
#include "pipelib.h"
#include "test.h"
#include "test_resources.h"
#include "testing.h"



/*************************************************************************************************/
/*  Pipelib tests                                                                                */
/*************************************************************************************************/

int test_pipelib_1(TstSuite* suite)
{
    ASSERT(suite != NULL);
    DvzGpu* gpu = get_gpu(suite);
    ASSERT(gpu != NULL);

    DvzPipelib pipelib = dvz_pipelib(gpu);

    dvz_pipelib_destroy(&pipelib);
    return 0;
}
