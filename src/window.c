/*************************************************************************************************/
/*  Window API                                                                                   */
/*************************************************************************************************/

#include "window.h"
#include "_glfw.h"
#include "common.h"
#include "vklite.h"



/*************************************************************************************************/
/*  Window                                                                                       */
/*************************************************************************************************/

DvzWindow* dvz_window(DvzHost* host, uint32_t width, uint32_t height)
{
    // NOTE: an offscreen canvas has NO DvzWindow, so this function should NEVER be called with an
    // offscreen backend, or for an offscreen canvas.

    ASSERT(host != NULL);

    DvzWindow* window = dvz_container_alloc(&host->windows);
    ASSERT(window != NULL);

    ASSERT(window->obj.type == DVZ_OBJECT_TYPE_WINDOW);
    ASSERT(window->obj.status == DVZ_OBJECT_STATUS_ALLOC);
    window->host = host;
    ASSERT(host->backend != DVZ_BACKEND_NONE && host->backend != DVZ_BACKEND_OFFSCREEN);

    window->width = width;
    window->height = height;
    window->close_on_esc = true;

    // Create the window, depending on the backend.
    window->backend_window =
        backend_window(host->instance, host->backend, width, height, window, &window->surface);

    if (window->surface == NULL)
    {
        log_error("could not create window surface");
        dvz_window_destroy(window);
        return NULL;
    }

    return window;
}



void dvz_window_get_size(
    DvzWindow* window, uint32_t* framebuffer_width, uint32_t* framebuffer_height)
{
    ASSERT(window != NULL);
    ASSERT(window->host != NULL);
    backend_window_get_size(
        window->host->backend, window->backend_window, //
        &window->width, &window->height,               //
        framebuffer_width, framebuffer_height);
}



void dvz_window_set_size(DvzWindow* window, uint32_t width, uint32_t height)
{
    ASSERT(window != NULL);
    ASSERT(window->host != NULL);
    backend_window_set_size(window->host->backend, window->backend_window, width, height);
}



void dvz_window_poll_events(DvzWindow* window)
{
    ASSERT(window != NULL);
    ASSERT(window->host != NULL);
    backend_poll_events(window->host->backend, window);
}



void dvz_window_destroy(DvzWindow* window)
{
    if (window == NULL || window->obj.status == DVZ_OBJECT_STATUS_DESTROYED)
    {
        log_trace("skip destruction of already-destroyed window");
        return;
    }
    ASSERT(window != NULL);
    ASSERT(window->host != NULL);
    backend_window_destroy(
        window->host->instance, window->host->backend, //
        window->backend_window, window->surface);
    dvz_obj_destroyed(&window->obj);
}
