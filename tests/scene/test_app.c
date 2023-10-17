/*************************************************************************************************/
/*  Testing app                                                                                  */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include "test_app.h"
#include "canvas.h"
#include "presenter.h"
#include "scene/animation.h"
#include "scene/app.h"
#include "scene/arcball.h"
#include "scene/camera.h"
#include "scene/panzoom.h"
#include "scene/transform.h"
#include "scene/viewset.h"
#include "scene/visual.h"
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

typedef struct PanzoomStruct PanzoomStruct;
struct PanzoomStruct
{
    DvzApp* app;
    DvzId mvp_id;
    DvzMVP mvp;
    DvzPanzoom* pz;
    DvzTransform* tr;
    GraphicsWrapper* wrapper;
};



typedef struct ArcballStruct ArcballStruct;
struct ArcballStruct
{
    DvzApp* app;
    DvzBatch* batch;
    DvzId mvp_id;
    DvzMVP mvp;
    DvzArcball* arcball;
    DvzCamera* cam;
};



typedef struct AnimStruct AnimStruct;
struct AnimStruct
{
    DvzApp* app;
    DvzBatch* batch;
    DvzId dat_id;
    DvzSize size;
    uint32_t n;
    void* data;
};



/*************************************************************************************************/
/*  App tests                                                                                    */
/*************************************************************************************************/

static void _scatter_mouse(DvzClient* client, DvzClientEvent ev)
{
    ANN(client);

    PanzoomStruct* ps = (PanzoomStruct*)ev.user_data;
    ANN(ps);

    DvzPanzoom* pz = ps->pz;
    ANN(pz);

    DvzApp* app = ps->app;
    ANN(app);

    DvzMVP* mvp = &ps->mvp;
    ANN(mvp);

    DvzId mvp_id = ps->mvp_id;

    // Mouse event.
    if (!dvz_panzoom_mouse(pz, ev.content.m))
        return;

    // Update the MVP matrices.
    *mvp = dvz_mvp_default();
    dvz_panzoom_mvp(pz, mvp);

    // This batch will be destroyed automatically in the event loop by the presenter.
    DvzBatch* batch = dvz_batch();
    ANN(batch);

    // Submit a dat upload request with the new MVP matrices.
    dvz_upload_dat(batch, mvp_id, 0, sizeof(DvzMVP), mvp);

    dvz_presenter_submit(app->prt, batch);
}

static void _scatter_resize(DvzClient* client, DvzClientEvent ev)
{
    ANN(client);

    PanzoomStruct* ps = (PanzoomStruct*)ev.user_data;
    ANN(ps);

    DvzPanzoom* pz = ps->pz;
    ANN(pz);

    DvzApp* app = ps->app;
    ANN(app);

    // This batch will be destroyed automatically in the event loop by the presenter.
    DvzBatch* batch = dvz_batch();
    ANN(batch);

    uint32_t width = ev.content.w.screen_width;
    uint32_t height = ev.content.w.screen_height;
    log_info("window 0x%" PRIx64 " resized to %dx%d", ev.window_id, width, height);

    dvz_panzoom_resize(pz, width, height);

    // Emit updated recording commands.
    // dvz_requester_begin(batch);
    graphics_commands(batch, ps->wrapper);

    dvz_presenter_submit(app->prt, batch);
}

int test_app_scatter(TstSuite* suite)
{
    ANN(suite);

    // Create app objects.
    DvzApp* app = dvz_app(0);
    DvzBatch* batch = dvz_app_batch(app);
    ANN(batch);

    const uint32_t n = 52;
    GraphicsWrapper wrapper = {0};
    graphics_request(batch, n, &wrapper, DVZ_CANVAS_FLAGS_FPS);
    void* data = graphics_scatter(batch, wrapper.dat_id, n);

    // Panzoom callback.
    DvzPanzoom* pz = dvz_panzoom(WIDTH, HEIGHT, 0);
    PanzoomStruct ps = {.mvp_id = wrapper.mvp_id, .app = app, .pz = pz, .wrapper = &wrapper};
    dvz_app_onmouse(app, _scatter_mouse, &ps);
    dvz_app_onresize(app, _scatter_resize, &ps);

    dvz_app_run(app, N_FRAMES);

    dvz_panzoom_destroy(pz);
    dvz_app_destroy(app);

    FREE(data);
    return 0;
}



static void _arcball_mouse(DvzClient* client, DvzClientEvent ev)
{
    ANN(client);

    ArcballStruct* arc = (ArcballStruct*)ev.user_data;
    ANN(arc);

    DvzArcball* arcball = arc->arcball;
    ANN(arcball);

    DvzApp* app = arc->app;
    ANN(app);

    // DvzBatch* batch = arc->batch;
    // ANN(batch);

    DvzMVP* mvp = &arc->mvp;
    ANN(mvp);

    DvzId mvp_id = arc->mvp_id;

    // Dragging: pan.
    if (ev.content.m.type == DVZ_MOUSE_EVENT_DRAG)
    {
        if (ev.content.m.content.d.button == DVZ_MOUSE_BUTTON_LEFT)
        {
            float width = arcball->viewport_size[0];
            float height = arcball->viewport_size[1];

            vec2 cur_pos, last_pos;
            cur_pos[0] = -1 + 2 * ev.content.m.content.d.cur_pos[0] / width;
            cur_pos[1] = +1 - 2 * ev.content.m.content.d.cur_pos[1] / height;
            last_pos[0] = -1 + 2 * ev.content.m.pos[0] / width; // press position
            last_pos[1] = +1 - 2 * ev.content.m.pos[1] / height;

            dvz_arcball_rotate(arcball, cur_pos, last_pos);
        }
        // else if (ev.content.m.content.d.button == DVZ_MOUSE_BUTTON_RIGHT)
        // {
        // }
    }

    // Stop dragging.
    if (ev.content.m.type == DVZ_MOUSE_EVENT_DRAG_STOP)
    {
        dvz_arcball_end(arcball);
    }

    // // Mouse wheel.
    // if (ev.content.m.type == DVZ_MOUSE_EVENT_WHEEL)
    // {
    //     dvz_panzoom_zoom_wheel(pz, ev.content.m.content.w.dir, ev.content.m.content.w.pos);
    // }

    // Double-click
    if (ev.content.m.type == DVZ_MOUSE_EVENT_DOUBLE_CLICK)
    {
        dvz_arcball_reset(arcball);
    }

    // Update the MVP matrices.
    dvz_arcball_mvp(arcball, mvp); // set the model matrix

    // Submit a dat upload request with the new MVP matrices.

    // This batch will be destroyed automatically in the event loop by the presenter.
    DvzBatch* batch = dvz_batch();
    ANN(batch);

    dvz_upload_dat(batch, mvp_id, 0, sizeof(DvzMVP), mvp);

    dvz_presenter_submit(app->prt, batch);
}

static void _arcball_resize(DvzClient* client, DvzClientEvent ev)
{
    ANN(client);

    uint32_t width = ev.content.w.screen_width;
    uint32_t height = ev.content.w.screen_height;
    log_info("window 0x%" PRIx64 " resized to %dx%d", ev.window_id, width, height);

    ANN(client);

    ArcballStruct* arc = (ArcballStruct*)ev.user_data;
    ANN(arc);

    DvzApp* app = arc->app;
    ANN(app);

    // DvzBatch* batch = arc->batch;
    // ANN(batch);

    DvzMVP* mvp = &arc->mvp;
    ANN(mvp);

    DvzId mvp_id = arc->mvp_id;

    DvzCamera* camera = arc->cam;
    ANN(camera);
    dvz_camera_resize(camera, width, height);

    DvzArcball* arcball = arc->arcball;
    ANN(arcball);
    dvz_arcball_resize(arcball, width, height);

    // Update the MVP matrices.
    dvz_camera_mvp(camera, mvp); // set the model matrix

    // This batch will be destroyed automatically in the event loop by the presenter.
    DvzBatch* batch = dvz_batch();
    ANN(batch);

    // Submit a dat upload request with the new MVP matrices.
    dvz_upload_dat(batch, mvp_id, 0, sizeof(DvzMVP), mvp);

    dvz_presenter_submit(app->prt, batch);
}

int test_app_arcball(TstSuite* suite)
{
    ANN(suite);

    // Create app objects.
    DvzApp* app = dvz_app(0);
    DvzBatch* batch = dvz_app_batch(app);

    const uint32_t n = 1000;
    GraphicsWrapper wrapper = {0};
    graphics_request(batch, n, &wrapper, DVZ_CANVAS_FLAGS_FPS);

    // Upload the data.
    DvzGraphicsPointVertex* data =
        (DvzGraphicsPointVertex*)calloc(n, sizeof(DvzGraphicsPointVertex));
    double t = 0;
    for (uint32_t i = 0; i < n; i++)
    {
        t = i / (double)(n);
        data[i].pos[0] = .25 * dvz_rand_normal();
        data[i].pos[1] = .25 * dvz_rand_normal();
        data[i].pos[2] = .25 * dvz_rand_normal();

        glm_vec3_normalize(data[i].pos);

        data[i].size = 4;

        dvz_colormap(DVZ_CMAP_HSV, TO_BYTE(t), data[i].color);
        data[i].color[3] = 128;
    }
    dvz_upload_dat(batch, wrapper.dat_id, 0, n * sizeof(DvzGraphicsPointVertex), data);

    // Arcball callback.
    DvzArcball* arcball = dvz_arcball(WIDTH, HEIGHT, 0);
    // dvz_arcball_constrain(arcball, (vec3){0, 1, 0});
    DvzCamera* camera = dvz_camera(WIDTH, HEIGHT, 0);
    ArcballStruct arc = {
        .app = app,
        .mvp_id = wrapper.mvp_id,
        .arcball = arcball,
        .cam = camera,
        .batch = batch,
        .mvp = dvz_mvp_default(),
    };
    dvz_camera_mvp(camera, &arc.mvp); // set the view and proj matrices

    // Submit a dat upload request with the new MVP matrices.
    dvz_upload_dat(batch, arc.mvp_id, 0, sizeof(DvzMVP), &arc.mvp);

    dvz_app_onmouse(app, _arcball_mouse, &arc);
    dvz_app_onresize(app, _arcball_resize, &arc);

    dvz_app_run(app, N_FRAMES);

    dvz_camera_destroy(camera);
    dvz_arcball_destroy(arcball);
    dvz_app_destroy(app);

    FREE(data);
    return 0;
}



static void _anim_timer(DvzClient* client, DvzClientEvent ev)
{
    ANN(client);

    AnimStruct* anim = (AnimStruct*)ev.user_data;
    ANN(anim);

    // DvzBatch* batch = anim->batch;
    // ANN(batch);

    DvzApp* app = anim->app;
    ANN(app);

    DvzGraphicsPointVertex* data = (DvzGraphicsPointVertex*)anim->data;
    ANN(data);

    uint32_t n = anim->n;
    ASSERT(n > 0);

    const double dur = 2.0;
    double t = fmod(ev.content.t.time / dur, 1);
    for (uint32_t i = 0; i < n; i++)
    {
        data[i].pos[1] = .9 * (-1 + 2 * dvz_easing((DvzEasing)i, t));
    }

    // This batch will be destroyed automatically in the event loop by the presenter.
    DvzBatch* batch = dvz_batch();
    ANN(batch);

    dvz_upload_dat(batch, anim->dat_id, 0, anim->size, data);
    dvz_presenter_submit(app->prt, batch);
}

int test_app_anim(TstSuite* suite)
{
    ANN(suite);

    // Create app objects.
    DvzApp* app = dvz_app(0);
    DvzBatch* batch = dvz_app_batch(app);

    const uint32_t n = (uint32_t)DVZ_EASING_COUNT;
    GraphicsWrapper wrapper = {0};
    graphics_request(batch, n, &wrapper, DVZ_CANVAS_FLAGS_FPS);

    // Upload the data.
    DvzGraphicsPointVertex* data =
        (DvzGraphicsPointVertex*)calloc(n, sizeof(DvzGraphicsPointVertex));
    double t = 0;
    for (uint32_t i = 0; i < n; i++)
    {
        t = i / (double)(n - 1);
        data[i].pos[0] = .9 * (-1 + 2 * t);
        data[i].pos[1] = 0;

        data[i].size = 50;

        dvz_colormap(DVZ_CMAP_HSV, TO_BYTE(t), data[i].color);
        data[i].color[3] = 128;
    }

    DvzSize size = n * sizeof(DvzGraphicsPointVertex);
    dvz_upload_dat(batch, wrapper.dat_id, 0, size, data);

    AnimStruct anim = {
        .app = app, .data = data, .batch = batch, .dat_id = wrapper.dat_id, .n = n, .size = size};
    dvz_app_timer(app, 0, 1. / 60., 0);
    dvz_app_ontimer(app, _anim_timer, &anim);

    dvz_app_run(app, N_FRAMES);

    dvz_app_destroy(app);
    FREE(data);
    return 0;
}



int test_app_pixel(TstSuite* suite)
{
    ANN(suite);

    // Create app objects.
    DvzApp* app = dvz_app(0);
    DvzBatch* batch = dvz_app_batch(app);

    // Create the visual.
    DvzVisual* pixel = dvz_pixel(batch, 0);
    const uint32_t n = 10000;
    dvz_pixel_alloc(pixel, n);

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


    // Manual setting of common bindings.

    // MVP.
    DvzMVP mvp = dvz_mvp_default();
    dvz_visual_mvp(pixel, &mvp);

    // Viewport.
    DvzViewport viewport = dvz_viewport_default(WIDTH, HEIGHT);
    dvz_visual_viewport(pixel, &viewport);


    // Create a board.
    DvzRequest req = dvz_create_canvas(batch, WIDTH, HEIGHT, DVZ_DEFAULT_CLEAR_COLOR, 0);
    DvzId canvas_id = req.id;

    // Record commands.
    dvz_record_begin(batch, canvas_id);
    dvz_record_viewport(batch, canvas_id, DVZ_DEFAULT_VIEWPORT, DVZ_DEFAULT_VIEWPORT);
    dvz_visual_instance(pixel, canvas_id, 0, 0, n, 0, 1);
    dvz_record_end(batch, canvas_id);

    // Make screenshot.
    dvz_app_run(app, 3);
    char imgpath[1024];
    snprintf(imgpath, sizeof(imgpath), "%s/app_pixel.png", ARTIFACTS_DIR);
    dvz_app_screenshot(app, canvas_id, imgpath);

    // Run the app.
    // dvz_app_run(app, N_FRAMES);

    // Cleanup
    dvz_app_destroy(app);
    FREE(pos);
    FREE(color);
    return 0;
}



static void _viewset_mouse(DvzClient* client, DvzClientEvent ev)
{
    ANN(client);

    PanzoomStruct* ps = (PanzoomStruct*)ev.user_data;
    ANN(ps);

    DvzPanzoom* pz = ps->pz;
    ANN(pz);

    // DvzBatch* batch = ps->app->batch;
    // ANN(batch);

    DvzApp* app = ps->app;
    ANN(app);

    DvzTransform* tr = ps->tr;
    ANN(tr);

    // Mouse event.
    dvz_panzoom_mouse(pz, ev.content.m);

    // Update the MVP matrices.
    DvzMVP* mvp = dvz_transform_mvp(tr);
    dvz_panzoom_mvp(pz, mvp);

    // This batch will be destroyed automatically in the event loop by the presenter.
    DvzBatch* batch = dvz_batch();
    ANN(batch);
    dvz_transform_update(tr, *mvp);
    dvz_presenter_submit(app->prt, batch);
}

int test_app_viewset(TstSuite* suite)
{
    ANN(suite);

    // TODO: this will fail because transform depends on batch, but batch is destroyed by presenter
    return 0;

    // Create app objects.
    DvzApp* app = dvz_app(0);
    DvzBatch* batch = dvz_app_batch(app);

    // Create a canvas.
    DvzRequest req =
        dvz_create_canvas(batch, WIDTH, HEIGHT, DVZ_DEFAULT_CLEAR_COLOR, DVZ_CANVAS_FLAGS_FPS);
    DvzId canvas_id = req.id;

    // Create a viewset.
    DvzViewset* viewset = dvz_viewset(batch, canvas_id);

    // Create a view.
    DvzView* view = dvz_view(viewset, DVZ_DEFAULT_VIEWPORT, DVZ_DEFAULT_VIEWPORT);


    // Upload the data.
    DvzVisual* pixel = dvz_pixel(batch, 0);
    const uint32_t n = 10000;
    dvz_pixel_alloc(pixel, n);


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


    // MVP transform.
    DvzTransform* tr = dvz_transform(batch);

    // Add the visual to the view.
    dvz_view_add(view, pixel, 0, n, 0, 1, tr, 0);

    // Build the viewset.
    dvz_viewset_build(viewset);

    // Panzoom callback.
    DvzPanzoom* pz = dvz_panzoom(WIDTH, HEIGHT, 0);
    // PanzoomStruct ps = {.mvp_id = tr->dual.dat, .app = app, .pz = pz, .wrapper = &wrapper};
    PanzoomStruct ps = {
        .app = app,
        .pz = pz,
        .tr = tr,
    };
    dvz_app_onmouse(app, _viewset_mouse, &ps);

    // TODO
    // dvz_app_onresize(app, _scatter_resize, &ps);

    // Run the app.
    dvz_app_run(app, N_FRAMES);

    // Cleanup.
    dvz_transform_destroy(tr);
    dvz_visual_destroy(pixel);
    dvz_viewset_destroy(viewset);
    dvz_app_destroy(app);
    FREE(pos);
    FREE(color);
    return 0;
}
