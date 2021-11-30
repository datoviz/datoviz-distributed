/*************************************************************************************************/
/*  Testing suite                                                                                */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include <stdio.h>

#include "_thread.h"
#include "test.h"
#include "test_alloc.h"
#include "test_fifo.h"
#include "test_obj.h"
#include "test_resources.h"
#include "test_thread.h"
#include "test_transfers.h"
#include "test_vklite.h"
#include "test_window.h"
#include "testing.h"



/*************************************************************************************************/
/*  Entry-point                                                                                  */
/*************************************************************************************************/

int dvz_run_tests(const char* match)
{
    TstSuite suite = tst_suite();
    DvzTestCtx ctx = {0};
    suite.context = &ctx;

    // Testing thread utils.
    TEST(test_utils_thread_1)
    TEST(test_utils_mutex_1)
    TEST(test_utils_cond_1)
    TEST(test_utils_atomic_1)

    // Testing obj.
    TEST(test_utils_obj_1)

    // Testing FIFO.
    TEST(test_utils_obj_1)
    TEST(test_utils_fifo_1)
    TEST(test_utils_fifo_2)
    TEST(test_utils_fifo_resize)
    TEST(test_utils_fifo_discard)
    TEST(test_utils_fifo_first)
    TEST(test_utils_deq_1)
    TEST(test_utils_deq_2)

    // Testing alloc.
    TEST(test_utils_alloc_1)
    TEST(test_utils_alloc_2)

    // Testing window.
    TEST(test_window_1)

    // Testing vklite.
    TEST(test_vklite_host)



    // Setup the host fixture.
    SETUP(setup_host)

    TEST(test_vklite_commands)
    TEST(test_vklite_buffer_1)
    TEST(test_vklite_buffer_resize)
    TEST(test_vklite_load_shader)
    TEST(test_vklite_compute)
    TEST(test_vklite_push)
    TEST(test_vklite_images)
    TEST(test_vklite_sampler)
    TEST(test_vklite_barrier_buffer)
    TEST(test_vklite_barrier_image)
    TEST(test_vklite_submit)
    TEST(test_vklite_offscreen)
    TEST(test_vklite_shader)
    TEST(test_vklite_window)
    TEST(test_vklite_swapchain)
    TEST(test_vklite_graphics)
    TEST(test_vklite_canvas_blank)
    TEST(test_vklite_canvas_triangle)

    // Teardown the host fixture.
    TEARDOWN(teardown_host)



    // Setup the gpu fixture.
    SETUP(setup_gpu)

    // Testing resources.
    TEST(test_resources_1)
    TEST(test_resources_dat_1)
    TEST(test_resources_tex_1)

    // Testing transfers.
    TEST(test_transfers_buffer_mappable)
    TEST(test_transfers_buffer_large)
    TEST(test_transfers_buffer_copy)
    TEST(test_transfers_image_buffer)
    TEST(test_transfers_direct_buffer)
    TEST(test_transfers_direct_image)
    TEST(test_transfers_dups_util)
    TEST(test_transfers_dups_upload)
    TEST(test_transfers_dups_copy)

    // Teardown the gpu fixture.
    TEARDOWN(teardown_gpu)



    tst_suite_run(&suite, match);
    tst_suite_destroy(&suite);
    return 0;
}
