/*************************************************************************************************/
/*  Scene                                                                                        */
/*************************************************************************************************/


/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include "scene/scene.h"
#include "_list.h"
#include "common.h"
#include "request.h"
#include "scene/app.h"
#include "scene/arcball.h"
#include "scene/graphics.h"
#include "scene/panzoom.h"
#include "scene/transform.h"
#include "scene/viewset.h"
#include "scene/visuals/pixel.h"



/*************************************************************************************************/
/*  Macros                                                                                       */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Constants                                                                                    */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Scene                                                                                        */
/*************************************************************************************************/

DvzScene* dvz_scene(DvzRequester* rqr)
{
    ANN(rqr);
    DvzScene* scene = (DvzScene*)calloc(1, sizeof(DvzScene));
    scene->rqr = rqr;
    scene->figures = dvz_list();
    return scene;
}



void dvz_scene_destroy(DvzScene* scene)
{
    ANN(scene);
    dvz_list_destroy(scene->figures);
    FREE(scene);
}



/*************************************************************************************************/
/*  Figure                                                                                       */
/*************************************************************************************************/

DvzFigure* dvz_figure(DvzScene* scene, uint32_t width, uint32_t height, int flags)
{
    ANN(scene);
    ASSERT(width > 0);
    ASSERT(height > 0);

    // Initialize the structure.
    DvzFigure* fig = (DvzFigure*)calloc(1, sizeof(DvzFigure));
    fig->scene = scene;
    fig->flags = flags;

    // NOTE: the size is in screen coordinates, not framebuffer coordinates.
    fig->shape[0] = width;
    fig->shape[1] = height;
    fig->shape_init[0] = width;
    fig->shape_init[1] = height;

    // Panels.
    fig->panels = dvz_list();

    // Requester.
    DvzRequester* rqr = scene->rqr;
    ANN(rqr);

    // Create the canvas.
    DvzRequest req = dvz_create_canvas(rqr, width, height, DVZ_DEFAULT_CLEAR_COLOR, flags);
    fig->canvas_id = req.id;

    // Create the viewset;
    fig->viewset = dvz_viewset(rqr, fig->canvas_id);

    // Append the figure to the scene's figures.
    dvz_list_append(scene->figures, (DvzListItem){.p = (void*)fig});

    return fig;
}



void dvz_figure_resize(DvzFigure* fig, uint32_t width, uint32_t height)
{
    ANN(fig);
    log_debug("resize figure to %dx%d", width, height);

    fig->shape[0] = width;
    fig->shape[1] = height;

    float width_init = fig->shape_init[0];
    float height_init = fig->shape_init[1];

    float x0 = 0, y0 = 0, w0 = 0, h0 = 0;
    float x = 0, y = 0, w = 0, h = 0;

    // Go through all panels.
    uint32_t n = dvz_list_count(fig->panels);
    DvzPanel* panel = NULL;
    for (uint32_t i = 0; i < n; i++)
    {
        panel = (DvzPanel*)dvz_list_get(fig->panels, i).p;
        ANN(panel);
        ANN(panel->view);

        // NOTE: in framebuffer coordinates.
        x0 = panel->offset_init[0];
        y0 = panel->offset_init[1];
        w0 = panel->shape_init[0];
        h0 = panel->shape_init[1];

        // NOTE: although width(init)/height(init) are in screen coordinates, we use a ratio here,
        // so x and x0 etc remain in framebuffer coordinates.
        x = x0 * width / width_init;
        y = y0 * height / height_init;
        w = w0 * width / width_init;
        h = h0 * height / height_init;

        // Update the view offset and shape.
        dvz_panel_resize(panel, x, y, w, h);
    }
}



DvzFigure* dvz_scene_figure(DvzScene* scene, DvzId id)
{
    // Return a figure from a canvas ID.
    ANN(scene);
    ANN(scene->figures);

    // Go through all figures.
    uint32_t n = dvz_list_count(scene->figures);
    DvzFigure* fig = NULL;
    for (uint32_t i = 0; i < n; i++)
    {
        fig = (DvzFigure*)dvz_list_get(scene->figures, i).p;
        ANN(fig);
        if (fig->canvas_id == id)
            return fig;
    }
    return NULL;
}



void dvz_figure_destroy(DvzFigure* fig)
{
    ANN(fig);

    // Destroy the viewset.
    dvz_viewset_destroy(fig->viewset);

    // Destroy all panels.
    uint32_t n = dvz_list_count(fig->panels);
    for (uint32_t i = 0; i < n; i++)
    {
        dvz_panel_destroy((DvzPanel*)dvz_list_get(fig->panels, i).p);
    }

    // Destroy the list of panels.
    dvz_list_destroy(fig->panels);

    // Remove the figure from the scene's figures.
    dvz_list_remove_pointer(fig->scene->figures, fig);

    // Free the DvzFigure structure.
    FREE(fig);
}



/*************************************************************************************************/
/*  Panel                                                                                        */
/*************************************************************************************************/

DvzPanel* dvz_panel(DvzFigure* fig, float x, float y, float w, float h)
{
    ANN(fig);
    ANN(fig->scene);
    ANN(fig->viewset);

    // Instantiate the structure.
    DvzPanel* panel = (DvzPanel*)calloc(1, sizeof(DvzPanel));
    panel->figure = fig;

    panel->offset_init[0] = x;
    panel->offset_init[1] = y;
    panel->shape_init[0] = w;
    panel->shape_init[1] = h;

    // Create a view.
    panel->view = dvz_view(fig->viewset, (vec2){x, y}, (vec2){w, h});

    // Append the figure to the scene's figures.
    dvz_list_append(fig->panels, (DvzListItem){.p = (void*)panel});

    return panel;
}



DvzPanel* dvz_panel_default(DvzFigure* fig)
{
    ANN(fig);
    return dvz_panel(fig, 0, 0, fig->shape[0], fig->shape[1]);
}



void dvz_panel_transform(DvzPanel* panel, DvzTransform* tr)
{
    ANN(panel);
    ANN(tr);
    panel->transform = tr;
}



void dvz_panel_resize(DvzPanel* panel, float x, float y, float width, float height)
{
    ANN(panel);
    ANN(panel->view);

    if (width == 0 || height == 0)
    {
        log_warn("skip panel_resize of size 0x0");
        return;
    }

    log_debug("resize panel to %.0fx%.0f -> %.0fx%.0f", x, y, width, height);

    dvz_view_resize(panel->view, (vec2){x, y}, (vec2){width, height});

    if (panel->panzoom)
        dvz_panzoom_resize(panel->panzoom, width, height);
}



bool dvz_panel_contains(DvzPanel* panel, vec2 pos)
{
    ANN(panel);
    ANN(panel->view);
    float x0 = panel->view->offset[0];
    float y0 = panel->view->offset[1];
    float w = panel->view->shape[0];
    float h = panel->view->shape[1];
    float x1 = x0 + w;
    float y1 = y0 + h;
    float x = pos[0];
    float y = pos[1];
    return (x0 <= x) && (x < x1) && (y0 <= y) && (y < y1);
}



DvzPanel* dvz_panel_at(DvzFigure* fig, vec2 pos)
{
    ANN(fig);
    ANN(fig->panels);

    // Go through all panels.
    uint32_t n = dvz_list_count(fig->panels);
    DvzPanel* panel = NULL;
    for (uint32_t i = 0; i < n; i++)
    {
        panel = (DvzPanel*)dvz_list_get(fig->panels, i).p;
        if (panel != NULL)
        {
            // Return the first panel that contains the position.
            if (dvz_panel_contains(panel, pos))
                return panel;
        }
    }
    return NULL;
}



void dvz_panel_destroy(DvzPanel* panel)
{
    ANN(panel);
    log_warn("destroy panel");

    // Destroy the transform.
    if (panel->transform != NULL && panel->transform_to_destroy)
    {
        // NOTE: double destruction causes segfault if a transform is shared between different
        // panels, the transform should be destroyed only once.
        dvz_transform_destroy(panel->transform);
        panel->transform = NULL;
    }

    // Destroy the view.
    dvz_view_destroy(panel->view);

    // Remove the figure from the scene's figures.
    dvz_list_remove_pointer(panel->figure->panels, panel);

    FREE(panel);
}



/*************************************************************************************************/
/*  Controllers                                                                                  */
/*************************************************************************************************/

DvzPanzoom* dvz_panel_panzoom(DvzApp* app, DvzPanel* panel)
{
    ANN(app);
    ANN(panel);
    ANN(panel->view);
    ANN(panel->figure);
    ANN(panel->figure->scene);

    ASSERT(panel->view->shape[0] > 0);
    ASSERT(panel->view->shape[1] > 0);

    // NOTE: the size is in screen coordinates, not framebuffer coordinates.
    panel->panzoom = dvz_panzoom(panel->view->shape[0], panel->view->shape[1], 0);

    // If a panel transform has not been already set, we create a MVP transform that will be bound
    // to the PanZoom in the mouse callback set by dvz_scene_run().
    if (panel->transform == NULL)
    {
        log_trace(
            "creating a new panel transform when setting a panzoom on the panel as no transform "
            "has been manually set to the panel so far");
        panel->transform = dvz_transform(panel->figure->scene->rqr);
        panel->transform_to_destroy = true;
    }

    return panel->panzoom;
}



DvzArcball* dvz_panel_arcball(DvzApp* app, DvzPanel* panel)
{
    ANN(app);
    ANN(panel);
    log_error("arcball not implemented yet");
    // TODO: camera
    return NULL;
}



/*************************************************************************************************/
/*  Visuals                                                                                      */
/*************************************************************************************************/

void dvz_panel_visual(DvzPanel* panel, DvzVisual* visual)
{
    ANN(panel);

    DvzView* view = panel->view;
    ANN(view);

    if (visual->item_count == 0)
    {
        log_error("cannot add empty visual, make sure to fill the visual's properties first.");
        return;
    }

    // By default, add all items, using a single instance, and the panel's transform.
    if (panel->transform == NULL)
        log_debug("no panel transform set when adding the view, creating a default one");
    dvz_view_add(view, visual, 0, visual->item_count, 0, 1, panel->transform, 0);
}



/*************************************************************************************************/
/*  Run                                                                                          */
/*************************************************************************************************/

static void _scene_onmouse(DvzClient* client, DvzClientEvent ev)
{
    ANN(client);

    DvzScene* scene = (DvzScene*)ev.user_data;
    ANN(scene);

    DvzRequester* rqr = scene->rqr;
    ANN(rqr);

    DvzFigure* fig = dvz_scene_figure(scene, ev.window_id);
    ANN(fig);

    DvzPanel* panel = dvz_panel_at(fig, ev.content.m.pos);
    if (panel == NULL)
    {
        return;
    }

    // Localize the mouse event (viewport offset).
    DvzMouseEvent mev =
        dvz_view_mouse(panel->view, ev.content.m, ev.content_scale, DVZ_MOUSE_REFERENCE_LOCAL);

    // Panzoom.
    DvzPanzoom* pz = panel->panzoom;
    if (pz != NULL)
    {
        DvzTransform* tr = panel->transform;
        if (tr == NULL)
        {
            log_warn("no transform set in panel");
            return;
        }
        // Pass the mouse event to the panzoom object.
        if (dvz_panzoom_mouse(pz, mev))
        {
            // Update the MVP matrices.
            DvzMVP* mvp = dvz_transform_mvp(tr);
            dvz_panzoom_mvp(pz, mvp);

            dvz_requester_begin(rqr);
            dvz_transform_update(tr, *mvp);
            dvz_requester_end(rqr, NULL);
        }
    }

    // Arcball.
    // TODO
}

static void _scene_onresize(DvzClient* client, DvzClientEvent ev)
{
    ANN(client);

    float w = ev.content.w.screen_width;
    float h = ev.content.w.screen_height;

    log_debug("window 0x%" PRIx64 " resized to %.0fx%.0f", ev.window_id, w, h);

    DvzScene* scene = (DvzScene*)ev.user_data;
    ANN(scene);

    // Retrieve the figure that is being resize, thanks to the id that is the same between the
    // canvas and the window.
    DvzFigure* fig = dvz_scene_figure(scene, ev.window_id);
    ANN(fig);
    ANN(fig->viewset);

    //
    if (dvz_atomic_get(fig->viewset->status) == DVZ_BUILD_DIRTY)
    {
        log_warn("skip figure onresize because the viewset is already dirty");
        return;
    }

    // Resize the figure, compute each panel's new size and resize them.
    dvz_figure_resize(fig, w, h);

    // Mark the viewset as dirty to trigger a command buffer record at the next frame.
    dvz_atomic_set(fig->viewset->status, (int)DVZ_BUILD_DIRTY);
}

static void _scene_onframe(DvzClient* client, DvzClientEvent ev)
{
    ANN(client);

    DvzScene* scene = (DvzScene*)ev.user_data;
    ANN(scene);

    // Go through all figures.
    uint32_t n = dvz_list_count(scene->figures);
    DvzFigure* fig = NULL;
    DvzBuildStatus status = DVZ_BUILD_CLEAR;
    for (uint32_t i = 0; i < n; i++)
    {
        fig = (DvzFigure*)dvz_list_get(scene->figures, i).p;
        ANN(fig);
        ANN(fig->viewset);

        // Build status.
        status = (DvzBuildStatus)dvz_atomic_get(fig->viewset->status);
        // if viewset state == dirty, build viewset, and set the viewset state to clear
        if (status == DVZ_BUILD_DIRTY)
        {
            log_debug("build figure #%d", i);
            dvz_viewset_build(fig->viewset);
            dvz_atomic_set(fig->viewset->status, (int)DVZ_BUILD_CLEAR);
        }
    }
}



void dvz_scene_run(DvzScene* scene, DvzApp* app, uint64_t n_frames)
{
    ANN(scene);
    ANN(app);

    // Go through all figures, mark them as dirty to trigger command buffer recording for all
    // figures.
    uint32_t n = dvz_list_count(scene->figures);
    DvzFigure* fig = NULL;
    for (uint32_t i = 0; i < n; i++)
    {
        fig = (DvzFigure*)dvz_list_get(scene->figures, i).p;
        ANN(fig);
        ANN(fig->viewset);

        // NOTE: the frame callback will record the command buffer at the first frame.
        dvz_atomic_set(fig->viewset->status, (int)DVZ_BUILD_DIRTY);
    }

    // Scene callbacks.
    dvz_app_onmouse(app, _scene_onmouse, scene);
    dvz_app_onresize(app, _scene_onresize, scene);
    dvz_app_onframe(app, _scene_onframe, scene);

    // Run the app.
    dvz_app_run(app, n_frames);
}
