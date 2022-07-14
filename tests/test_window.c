/*************************************************************************************************/
/*  Testing window                                                                               */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include <stdio.h>

#include "_glfw.h"
#include "host.h"
#include "test_window.h"
#include "testing.h"
#include "window.h"



/*************************************************************************************************/
/*  Tests                                                                                        */
/*************************************************************************************************/

int test_window_1(TstSuite* suite)
{
    ASSERT(suite != NULL);
    DvzBackend backend = DVZ_BACKEND_GLFW;

    DvzWindow window = dvz_window(backend, 100, 100, 0);
    DvzWindow window2 = dvz_window(backend, 100, 100, 0);

    dvz_window_destroy(&window);
    dvz_window_destroy(&window2);

    glfwTerminate();
    return 0;
}
