/*************************************************************************************************/
/*  Testing scene                                                                                */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include "test_scene.h"
#include "canvas.h"
#include "scene/app.h"
#include "scene/scene.h"
#include "scene/viewset.h"
#include "scene/visuals/pixel.h"
#include "test.h"
#include "testing.h"
#include "testing_utils.h"



/*************************************************************************************************/
/*  Constants                                                                                    */
/*************************************************************************************************/

#define WIDTH  800
#define HEIGHT 600



/*************************************************************************************************/
/*  Test utils                                                                                   */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Scene tests                                                                                  */
/*************************************************************************************************/

int test_scene_1(TstSuite* suite)
{
    ANN(suite);

    // Create app object.
    DvzApp* app = dvz_app();
    DvzRequester* rqr = dvz_app_requester(app);

    // Create a scene.
    DvzScene* scene = dvz_scene(rqr);

    // Create a figure.
    DvzFigure* figure = dvz_figure(scene, WIDTH, HEIGHT, DVZ_CANVAS_FLAGS_VSYNC);

    // Create a panel.
    DvzPanel* panel = dvz_panel_default(figure);

    // Panel contains.
    AT(dvz_panel_contains(panel, (vec2){0, 0}));
    AT(!dvz_panel_contains(panel, (vec2){WIDTH, HEIGHT}));
    ASSERT(dvz_panel_at(figure, (vec2){WIDTH / 2, HEIGHT / 2}) == panel);
    ASSERT(dvz_panel_at(figure, (vec2){WIDTH / 2, -1}) == NULL);

    // Panzoom.
    DvzPanzoom* pz = dvz_panel_panzoom(app, panel);
    ANN(pz);

    // Create a visual.
    DvzVisual* pixel = dvz_pixel(rqr, 0);
    const uint32_t n = 10000;
    dvz_pixel_create(pixel, n);

    // Add the visual to the panel AFTER it has been created.
    dvz_panel_visual(panel, pixel);


    // Position.
    vec3* pos = (vec3*)calloc(n, sizeof(vec3));
    for (uint32_t i = 0; i < n; i++)
    {
        pos[i][0] = .25 * dvz_rand_normal();
        pos[i][1] = .25 * dvz_rand_normal();
    }
    dvz_pixel_position(pixel, 0, n, pos, 0);

    // Color.
    cvec4* color = (cvec4*)calloc(n, sizeof(cvec4));
    for (uint32_t i = 0; i < n; i++)
    {
        dvz_colormap(DVZ_CMAP_HSV, i % n, color[i]);
        color[i][3] = 128;
    }
    dvz_pixel_color(pixel, 0, n, color, 0);

    // Important: upload the data to the GPU.
    dvz_visual_update(pixel);


    // Run the app.
    dvz_scene_run(scene, app, N_FRAMES);

    // Cleanup.
    dvz_panel_destroy(panel);
    dvz_figure_destroy(figure);
    dvz_scene_destroy(scene);
    dvz_app_destroy(app);
    FREE(pos);
    FREE(color);
    return 0;
}



int test_scene_2(TstSuite* suite)
{
    ANN(suite);

    // Create app object.
    DvzApp* app = dvz_app();
    DvzRequester* rqr = dvz_app_requester(app);

    // Create a scene.
    DvzScene* scene = dvz_scene(rqr);

    // Create a figure.
    DvzFigure* figure = dvz_figure(scene, WIDTH, HEIGHT, DVZ_CANVAS_FLAGS_VSYNC);

    // Create a visual.
    DvzVisual* pixel = dvz_pixel(rqr, 0);
    const uint32_t n = 100000;
    dvz_pixel_create(pixel, n);


    // Position.
    vec3* pos = (vec3*)calloc(n, sizeof(vec3));
    for (uint32_t i = 0; i < n; i++)
    {
        pos[i][0] = .25 * dvz_rand_normal();
        pos[i][1] = .25 * dvz_rand_normal();
    }
    dvz_pixel_position(pixel, 0, n, pos, 0);

    // Color.
    cvec4* color = (cvec4*)calloc(n, sizeof(cvec4));
    for (uint32_t i = 0; i < n; i++)
    {
        dvz_colormap(DVZ_CMAP_HSV, i % n, color[i]);
        color[i][3] = 128;
    }
    dvz_pixel_color(pixel, 0, n, color, 0);

    // Important: upload the data to the GPU.
    dvz_visual_update(pixel);


    // Create two panels.
    DvzPanel* panel_0 = dvz_panel(figure, 0, 0, WIDTH / 2, HEIGHT);
    DvzPanel* panel_1 = dvz_panel(figure, WIDTH / 2, 0, WIDTH / 2, HEIGHT);

    // Transforms.

    // Panzoom.
    DvzPanzoom* pz = dvz_panel_panzoom(app, panel_0);
    ANN(pz);

    dvz_panel_transform(panel_1, panel_0->transform);

    // Second visual.
    DvzVisual* pixel_1 = dvz_pixel(rqr, 0);
    dvz_pixel_create(pixel_1, n / 10);
    dvz_pixel_position(pixel_1, 0, n / 10, pos, 0);
    dvz_pixel_color(pixel_1, 0, n / 10, color, 0);
    dvz_visual_update(pixel_1);


    // Add the visuals to the panel.
    dvz_panel_visual(panel_0, pixel);
    dvz_panel_visual(panel_1, pixel_1);

    // Run the app.
    dvz_scene_run(scene, app, N_FRAMES);

    // Cleanup.
    dvz_panel_destroy(panel_0);
    dvz_panel_destroy(panel_1);
    dvz_figure_destroy(figure);
    dvz_scene_destroy(scene);
    dvz_app_destroy(app);
    FREE(pos);
    FREE(color);
    return 0;
}
