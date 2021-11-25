/*************************************************************************************************/
/*  Window API                                                                                   */
/*************************************************************************************************/

#ifndef DVZ_HEADER_WINDOW
#define DVZ_HEADER_WINDOW



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include <vulkan/vulkan.h>

// #include "_glfw.h"
#include "common.h"



/*************************************************************************************************/
/*  Typedefs                                                                                     */
/*************************************************************************************************/

typedef struct DvzHost DvzHost;
typedef struct DvzWindow DvzWindow;
typedef struct DvzGpu DvzGpu;

// Forward declaration.
typedef struct DvzHost DvzHost;



/*************************************************************************************************/
/*  Structs                                                                                      */
/*************************************************************************************************/

struct DvzHost
{
    DvzObject obj;
    uint32_t n_errors;

    // Backend
    DvzBackend backend;

    // Global clock
    DvzClock clock;
    // bool is_running;

    // Vulkan objects.
    VkInstance instance;
    VkDebugUtilsMessengerEXT debug_messenger;

    // Containers.
    DvzContainer gpus;
    DvzContainer windows;
    // DvzContainer canvases; // to remove

    // DvzRun* run;
};



struct DvzWindow
{
    DvzObject obj;
    DvzHost* host;

    void* backend_window;
    uint32_t width, height; // in screen coordinates

    bool close_on_esc;
    VkSurfaceKHR surface;
};



/*************************************************************************************************/
/*  Host                                                                                         */
/*************************************************************************************************/

/**
 * Create a host.
 *
 * This object represents a computer with one or multiple GPUs.
 * It holds the Vulkan instance and it is responsible for discovering the available GPUs.
 *
 * @param backend the backend
 * @returns a pointer to the created host
 */
DVZ_EXPORT DvzHost* dvz_host(DvzBackend backend);

/**
 * Full synchronization on all GPUs.
 *
 * This function waits on all queues of all GPUs. The strongest, least efficient of the
 * synchronization methods.
 *
 * @param host the host
 */
DVZ_EXPORT void dvz_host_wait(DvzHost* host);

/**
 * Destroy the host.
 *
 * This function automatically destroys all objects created within the host.
 *
 * @param host the host to destroy
 */
DVZ_EXPORT int dvz_host_destroy(DvzHost* host);

/**
 * Destroy the run.
 *
 * !!! important
 *     This function should never be called by the user. It is always called automatically by
 *     dvz_app_destroy() at the last moment, AFTER all canvases have been destroyed. Otherwise,
 *     canvas callbacks may try to access the run before the canvases are destroyed, but after the
 *     run has been destroyed, resulting in a segmentation fault.
 *
 * @param the run instance
 */
// DVZ_EXPORT void dvz_run_destroy(DvzRun* run);



/*************************************************************************************************/
/*  Window                                                                                       */
/*************************************************************************************************/

/**
 * Create a blank window.
 *
 * This function is rarely used on its own. A bare window offers
 * no functionality that allows one to render to it with Vulkan. One needs a swapchain, an event
 * loop, and so on, which are provided instead at the level of the Canvas.
 *
 * @param host the host
 * @param width the window width, in pixels
 * @param height the window height, in pixels
 * @returns the window
 */
DVZ_EXPORT DvzWindow* dvz_window(DvzHost* host, uint32_t width, uint32_t height);

/**
 * Get the window size, in pixels.
 *
 * @param window the window
 * @param[out] framebuffer_width the width, in pixels
 * @param[out] framebuffer_height the height, in pixels
 */
DVZ_EXPORT void
dvz_window_get_size(DvzWindow* window, uint32_t* framebuffer_width, uint32_t* framebuffer_height);

/**
 * Set the window size, in pixels.
 *
 * @param window the window
 * @param width the width, in pixels
 * @param height the height, in pixels
 */
DVZ_EXPORT void dvz_window_set_size(DvzWindow* window, uint32_t width, uint32_t height);

/**
 * Process the pending windowing events by the backend (glfw by default).
 *
 * @param window the window
 */
DVZ_EXPORT void dvz_window_poll_events(DvzWindow* window);

/**
 * Destroy a window.
 *
 * !!! warning
 *     This function must be imperatively called *after* `dvz_swapchain_destroy()`.
 *
 * @param window the window
 */
DVZ_EXPORT void dvz_window_destroy(DvzWindow* window);

/**
 * Destroy a canvas.
 *
 * @param canvas the canvas
 */
// DVZ_EXPORT void dvz_canvas_destroy(DvzCanvas* canvas);

/**
 * Destroy all canvases.
 *
 * @param canvases the container with the canvases.
 */
// DVZ_EXPORT void dvz_canvases_destroy(DvzContainer* canvases);



#endif
