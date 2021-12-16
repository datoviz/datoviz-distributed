/*************************************************************************************************/
/*  Testing renderer                                                                             */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include "test_runner.h"
#include "_glfw.h"
#include "context.h"
#include "renderer.h"
#include "request.h"
#include "runner.h"
#include "test.h"
#include "test_resources.h"
#include "test_vklite.h"
#include "testing.h"
#include "window.h"



/*************************************************************************************************/
/*  Renderer tests                                                                               */
/*************************************************************************************************/

int test_runner_1(TstSuite* suite)
{
    ASSERT(suite != NULL);
    DvzHost* host = get_host(suite);

    DvzGpu* gpu = make_gpu(host);
    ASSERT(gpu != NULL);

    // Renderer.
    DvzRenderer* rd = dvz_renderer(gpu);

    // Requester.
    DvzRequester rqr = dvz_requester();
    DvzRequest req = {0};

    // Runner.
    DvzRunner* runner = dvz_runner(rd);

    // Create a request.
    req = dvz_create_canvas(&rqr, WIDTH, HEIGHT, 0);
    dvz_runner_request(runner, req);

    // Runner loop.
    dvz_runner_loop(runner, N_FRAMES);

    // Destruction
    dvz_runner_destroy(runner);
    dvz_requester_destroy(&rqr);
    dvz_renderer_destroy(rd);
    return 0;
}
