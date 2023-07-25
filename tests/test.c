/*************************************************************************************************/
/*  Testing suite                                                                                */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include <stdio.h>

#include "_thread.h"
#include "fileio.h"
#include "scene/test_animation.h"
#include "scene/test_app.h"
#include "scene/test_arcball.h"
#include "scene/test_array.h"
#include "scene/test_baker.h"
#include "scene/test_camera.h"
#include "scene/test_colormaps.h"
#include "scene/test_dual.h"
#include "scene/test_graphics.h"
#include "scene/test_panzoom.h"
#include "scene/test_scene.h"
#include "scene/test_shape.h"
#include "scene/test_viewset.h"
#include "scene/test_visual.h"
#include "scene/visuals/test_basic.h"
#include "scene/visuals/test_image.h"
#include "scene/visuals/test_mesh.h"
#include "scene/visuals/test_pixel.h"
#include "scene/visuals/test_point.h"
#include "scene/visuals/test_segment.h"
#include "test.h"
#include "test_alloc.h"
#include "test_board.h"
#include "test_canvas.h"
#include "test_client.h"
#include "test_client_input.h"
#include "test_datalloc.h"
#include "test_fifo.h"
#include "test_fileio.h"
#include "test_gui.h"
#include "test_input.h"
#include "test_keyboard.h"
#include "test_list.h"
#include "test_loop.h"
#include "test_map.h"
#include "test_mouse.h"
#include "test_obj.h"
#include "test_pipe.h"
#include "test_pipelib.h"
#include "test_presenter.h"
#include "test_renderer.h"
#include "test_request.h"
#include "test_resources.h"
#include "test_thread.h"
#include "test_timer.h"
#include "test_transfers.h"
#include "test_vklite.h"
#include "test_window.h"
#include "test_workspace.h"
#include "testing.h"



/*************************************************************************************************/
/*  Entry-point                                                                                  */
/*************************************************************************************************/

int dvz_run_tests(const char* match)
{
    TstSuite suite = tst_suite();
    DvzTestCtx ctx = {0};
    suite.context = &ctx;

    /*********************************************************************************************/
    /*  Utils                                                                                    */
    /*********************************************************************************************/

    // Testing thread utils.
    TEST(test_thread_1)
    TEST(test_mutex_1)
    TEST(test_cond_1)
    TEST(test_atomic_1)

    // Testing obj.
    TEST(test_obj_1)

    // Testing file IO.
    TEST(test_png_1)

    // Testing FIFO.
    TEST(test_fifo_1)
    TEST(test_fifo_2)
    TEST(test_fifo_resize)
    TEST(test_fifo_discard)
    TEST(test_fifo_first)
    TEST(test_deq_1)
    TEST(test_deq_2)
    TEST(test_deq_3)


    // Testing alloc.
    TEST(test_alloc_1)
    TEST(test_alloc_2)
    TEST(test_alloc_3)


    // Testing map.
    TEST(test_map_1)
    TEST(test_map_2)

    // Testing list.
    TEST(test_list_1)

    // Testing keyboard.
    TEST(test_keyboard_1)
    TEST(test_keyboard_2)

    // Testing mouse.
    TEST(test_mouse_move)
    TEST(test_mouse_press)
    TEST(test_mouse_wheel)
    TEST(test_mouse_drag)

    // Testing timer.
    TEST(test_timer_1)
    TEST(test_timer_2)

    // Testing input.
    TEST(test_input_mouse)
    TEST(test_input_keyboard)

    // Testing client input.
    TEST(test_client_input)


    /*********************************************************************************************/
    /*  Renderer                                                                                 */
    /*********************************************************************************************/

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
    TEST(test_vklite_swapchain)
    TEST(test_vklite_graphics)
    TEST(test_vklite_indirect)
    TEST(test_vklite_indexed)
    TEST(test_vklite_instanced)
    TEST(test_vklite_vertex_bindings)
    TEST(test_vklite_constattr)


    // Testing canvas.
    TEST(test_canvas_1)

    // Testing simple loop.
    TEST(test_loop_1)
    TEST(test_loop_2)
    TEST(test_loop_cube)
    TEST(test_loop_gui)

    // Testing GUI.
    TEST(test_gui_offscreen)

    // Testing presenter.
    TEST(test_presenter_1)
    TEST(test_presenter_2)
    TEST(test_presenter_thread)
    // TEST(test_presenter_deserialize) // used for debugging with a requests.dvz file
    TEST(test_presenter_gui)
    TEST(test_presenter_multi)

    // Teardown the host fixture.
    TEARDOWN(teardown_host)

    //

    // Setup the gpu fixture (offscreen only).
    SETUP(setup_gpu)

    // Testing resources.
    TEST(test_resources_1)
    TEST(test_resources_dat_1)
    TEST(test_resources_tex_1)
    TEST(test_datalloc_1)

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

    // Testing resources transfers.
    TEST(test_resources_dat_transfers)
    TEST(test_resources_dat_resize)
    TEST(test_resources_tex_transfers)
    TEST(test_resources_tex_resize)

    // Testing board.
    TEST(test_board_1)

    // Testing pipe.
    TEST(test_pipe_1)

    // Testing pipelib.
    TEST(test_pipelib_1)

    // Testing workspace.
    TEST(test_workspace_1)

    // Testing renderer.
    TEST(test_renderer_1)
    TEST(test_renderer_graphics)
    TEST(test_renderer_resize)
    TEST(test_renderer_image)

    // Test visuals.
    TEST(test_visual_1)
    TEST(test_viewset_1)
    TEST(test_viewset_mouse)

    TEST(test_basic_1)
    TEST(test_pixel_1)
    TEST(test_point_1)
    TEST(test_segment_1)
    TEST(test_mesh_1)
    TEST(test_image_1)

    // Teardown the gpu fixture.
    TEARDOWN(teardown_gpu)


    // OLD TESTS
    // Testing builtin graphics.
    TEST(test_graphics_point)
    TEST(test_graphics_triangle)
    TEST(test_graphics_line_list)
    TEST(test_graphics_raster)
    TEST(test_graphics_image_1)



    /*********************************************************************************************/
    /*  Client                                                                                   */
    /*********************************************************************************************/

    // Testing window.
    TEST(test_window_1)

    // Testing client.
    TEST(test_client_1)
    TEST(test_client_2)
    TEST(test_client_thread)

    // Testing request.
    TEST(test_request_1)
    TEST(test_request_2)



    /*********************************************************************************************/
    /*  Scene                                                                                    */
    /*********************************************************************************************/

    // Testing array.
    TEST(test_array_1)
    TEST(test_array_2)
    TEST(test_array_3)
    TEST(test_array_4)
    TEST(test_array_5)
    TEST(test_array_6)
    TEST(test_array_7)
    TEST(test_array_cast)
    TEST(test_array_mvp)
    TEST(test_array_3D)

    // Testing dual.
    TEST(test_dual_1)
    TEST(test_dual_2)

    // Testing baker.
    TEST(test_baker_1)
    TEST(test_baker_2)
    TEST(test_baker_3)

    // Testing colormaps.
    TEST(test_colormaps_idx)
    TEST(test_colormaps_uv)
    TEST(test_colormaps_extent)
    TEST(test_colormaps_default)
    TEST(test_colormaps_scale)
    TEST(test_colormaps_packuv)
    TEST(test_colormaps_array)

    // Testing scene elements.
    TEST(test_panzoom_1)
    TEST(test_arcball_1)
    TEST(test_camera_1)
    TEST(test_animation_1)
    TEST(test_shape_1)

    // Testing app.
    TEST(test_app_scatter)
    TEST(test_app_arcball)
    TEST(test_app_anim)
    TEST(test_app_pixel)
    TEST(test_app_viewset)

    // Testing scene.
    TEST(test_scene_1)
    TEST(test_scene_2)
    TEST(test_scene_3)


    tst_suite_run(&suite, match);
    tst_suite_destroy(&suite);
    return 0;
}
