/*************************************************************************************************/
/*  Viewset                                                                                      */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include "scene/viewset.h"
#include "_list.h"
#include "_map.h"
#include "common.h"
#include "request.h"
#include "scene/baker.h"
#include "scene/transform.h"
#include "scene/visual.h"



/*************************************************************************************************/
/*  Macros                                                                                       */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Util functions                                                                               */
/*************************************************************************************************/

static inline void
_normalize_pos(vec2* pos, vec2 offset, vec2 shape, float content_scale, DvzMouseReference ref)
{
    ANN(pos);

    float x0 = offset[0];
    float y0 = offset[1];
    float w = shape[0];
    float h = shape[1];

    float xc = x0 + w * .5;
    float yc = y0 + h * .5;

    switch (ref)
    {
    case DVZ_MOUSE_REFERENCE_GLOBAL:
        // Do nothing.
        break;

    case DVZ_MOUSE_REFERENCE_LOCAL:
        // Just subtract the offset.
        pos[0][0] -= x0;
        pos[0][1] -= y0;
        break;

    case DVZ_MOUSE_REFERENCE_SCALED:
        // Subtract the offset.
        pos[0][0] -= xc;
        pos[0][1] -= yc;

        // Divide by the viewport size.
        if (w > 0)
            pos[0][0] /= (w * .5);
        if (h > 0)
            pos[0][1] /= (h * .5);

        // NOTE: inverse y axis in SCALED reference (y=-1 is at the bottom, not the top)
        // This is because window coordinate convention is y down, while mathematical convention is
        // y up.
        pos[0][1] = -pos[0][1];
        break;

    default:
        log_error("unknown mouse reference %d", ref);
        break;
    }

    // Content scaling.
    pos[0][0] *= content_scale;
    pos[0][1] *= content_scale;
}



/*************************************************************************************************/
/*  Viewset                                                                                      */
/*************************************************************************************************/

DvzViewset* dvz_viewset(DvzRequester* rqr, DvzId canvas_id)
{
    ANN(rqr);
    log_trace("create viewset");

    DvzViewset* viewset = (DvzViewset*)calloc(1, sizeof(DvzViewset));
    viewset->rqr = rqr;
    viewset->status = dvz_atomic();
    viewset->canvas_id = canvas_id;
    viewset->views = dvz_list();

    return viewset;
}



void dvz_viewset_clear(DvzViewset* viewset)
{
    ANN(viewset);
    ANN(viewset->views);
    log_trace("clear viewset");

    uint64_t count = dvz_list_count(viewset->views);
    DvzView* view = NULL;
    for (uint64_t i = 0; i < count; i++)
    {
        view = (DvzView*)dvz_list_get(viewset->views, i).p;
        ANN(view);
        dvz_view_destroy(view);
    }
    dvz_list_clear(viewset->views);
}



void dvz_viewset_build(DvzViewset* viewset)
{
    // this function recreates the command buffer
    ANN(viewset);
    ANN(viewset->views);
    log_trace("build viewset");

    DvzId canvas_id = viewset->canvas_id;
    ASSERT(canvas_id != DVZ_ID_NONE);

    // TODO
    // a viewset flag indicates whether the command buffer needs to be rebuilt
    // it is set to true whenever a view_*() function is called
    // at every frame, dvz_viewset_build() is called (no op if !is_dirty)

    DvzRequester* rqr = viewset->rqr;
    dvz_record_begin(rqr, canvas_id);

    uint64_t view_count = dvz_list_count(viewset->views);
    uint64_t count = 0;
    DvzView* view = NULL;
    // bool indirect = false;
    DvzVisual* visual = NULL;

    // for each view
    for (uint64_t i = 0; i < view_count; i++)
    {
        view = (DvzView*)dvz_list_get(viewset->views, i).p;
        ANN(view);
        ANN(view->visuals);

        // Set the current viewport, corresponding to the current view.
        dvz_record_viewport(rqr, canvas_id, view->offset, view->shape);

        // For each visual in the view
        count = dvz_list_count(view->visuals);
        for (uint64_t j = 0; j < count; j++)
        {
            visual = (DvzVisual*)dvz_list_get(view->visuals, i).p;
            ANN(visual);

            if (!visual->is_visible)
            {
                log_debug("skipping invisible visual");
                continue;
            }

            // Call the visual draw callback with the parameters stored in the visual.
            dvz_visual_record(visual, canvas_id);
        }
    }

    dvz_record_end(rqr, canvas_id);

    // HACK
    dvz_requester_end(rqr, NULL);
}



void dvz_viewset_destroy(DvzViewset* viewset)
{
    ANN(viewset);
    log_trace("destroy viewset");

    // Destroy through all views.
    dvz_viewset_clear(viewset);
    dvz_atomic_destroy(viewset->status);
    dvz_list_destroy(viewset->views);
    FREE(viewset);
}



/*************************************************************************************************/
/*  View                                                                                         */
/*************************************************************************************************/

DvzView* dvz_view(DvzViewset* viewset, vec2 offset, vec2 shape)
{
    ANN(viewset);
    log_trace("create view");

    DvzView* view = (DvzView*)calloc(1, sizeof(DvzView));
    view->viewset = viewset;
    view->visuals = dvz_list();
    dvz_view_resize(view, offset, shape);
    dvz_list_append(viewset->views, (DvzListItem){.p = view});
    return view;
}



void dvz_view_add(
    DvzView* view, DvzVisual* visual,                 //
    uint32_t first, uint32_t count,                   // items
    uint32_t first_instance, uint32_t instance_count, // instances
    DvzTransform* transform, int viewport_flags)      // transform and viewport flags
{
    ANN(view);
    ANN(visual);

    ASSERT(count > 0);
    ASSERT(instance_count > 0);

    visual->draw_first = first;
    visual->draw_count = count;
    visual->first_instance = first_instance;
    visual->instance_count = instance_count;

    dvz_list_append(view->visuals, (DvzListItem){.p = visual});

    // MVP.
    if (transform == NULL)
    {
        log_debug("creating default transform");
        transform = dvz_transform(visual->rqr);
    }
    ANN(transform);
    // TODO: use a #define macro instead of hard-coded value 0 here.
    // NOTE: bind the transform's dual to slot idx 0 (=MVP)
    dvz_bind_dat(visual->rqr, visual->graphics_id, 0, transform->dual.dat, 0);

    // Viewport.
    DvzViewport viewport = dvz_viewport_default(view->shape[0], view->shape[1]);
    dvz_visual_viewport(visual, viewport);
}



DvzMouseEvent
dvz_view_mouse(DvzView* view, DvzMouseEvent ev, float content_scale, DvzMouseReference ref)
{
    ANN(view);

    _normalize_pos(&ev.pos, view->offset, view->shape, content_scale, ref);

    switch (ev.type)
    {
    case DVZ_MOUSE_EVENT_DRAG_START:
    case DVZ_MOUSE_EVENT_DRAG_STOP:
    case DVZ_MOUSE_EVENT_DRAG:
        _normalize_pos(&ev.content.d.cur_pos, view->offset, view->shape, content_scale, ref);
        break;
    default:
        break;
    }

    return ev;
}



void dvz_view_clear(DvzView* view)
{
    ANN(view);
    ANN(view->visuals);
    log_trace("clear view");
    dvz_list_clear(view->visuals);
}



void dvz_view_resize(DvzView* view, vec2 offset, vec2 shape)
{
    ANN(view);
    glm_vec2_copy(offset, view->offset);
    glm_vec2_copy(shape, view->shape);
}



void dvz_view_destroy(DvzView* view)
{
    ANN(view);
    ANN(view->viewset);
    log_trace("destroy view");

    dvz_list_destroy(view->visuals);
    dvz_list_remove_pointer(view->viewset->views, view);
    FREE(view);
}
