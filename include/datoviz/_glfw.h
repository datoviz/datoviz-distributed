/*************************************************************************************************/
/*  Vklite utils                                                                                 */
/*************************************************************************************************/

#ifndef DVZ_HEADER_GLFW
#define DVZ_HEADER_GLFW



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#ifndef HAS_GLFW
#define HAS_GLFW 0
#endif

#if HAS_GLFW
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#else
typedef struct GLFWwindow GLFWwindow;
#include <vulkan/vulkan.h>
#endif

#include "common.h"
#include "window.h"



/*************************************************************************************************/
/*  Backend-specific code                                                                        */
/*************************************************************************************************/

static const char** backend_extensions(DvzBackend backend, uint32_t* required_extension_count)
{
    const char** required_extensions = NULL;

    // Backend initialization and required extensions.
    switch (backend)
    {
    case DVZ_BACKEND_GLFW:
#if HAS_GLFW
        ASSERT(glfwVulkanSupported() != 0);
        required_extensions = glfwGetRequiredInstanceExtensions(required_extension_count);
        log_trace("%d extension(s) required by backend GLFW", *required_extension_count);
#endif
        break;
    default:
        break;
    }

    return required_extensions;
}



static void
_glfw_esc_callback(GLFWwindow* backend_window, int key, int scancode, int action, int mods)
{
#if HAS_GLFW
    // WARNING: this callback is only valid for DvzWindows that are not wrapped inside a DvzCanvas
    // This is because the DvzCanvas has its own glfw keyboard callback, and there can be only 1.
    DvzWindow* window = (DvzWindow*)glfwGetWindowUserPointer(backend_window);
    ASSERT(window != NULL);
    if (window->close_on_esc && action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
    {
        window->obj.status = DVZ_OBJECT_STATUS_NEED_DESTROY;
    }
#endif
}



static void* backend_window(
    VkInstance instance, DvzBackend backend, uint32_t width, uint32_t height, //
    DvzWindow* window, VkSurfaceKHR* surface)
{
    log_trace("create window with size %dx%d", width, height);

    switch (backend)
    {
    case DVZ_BACKEND_GLFW:
#if HAS_GLFW
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        ASSERT(width > 0);
        ASSERT(height > 0);
        GLFWwindow* bwin = glfwCreateWindow((int)width, (int)height, APPLICATION_NAME, NULL, NULL);
        ASSERT(bwin != NULL);
        VkResult res = glfwCreateWindowSurface(instance, bwin, NULL, surface);
        if (res != VK_SUCCESS)
            log_error("error creating the GLFW surface, result was %d", res);

        glfwSetWindowUserPointer(bwin, window);

        // Callback that marks the window to close if ESC is pressed, but only if
        // DvzWindow.close_on_esc=true
        glfwSetKeyCallback(bwin, _glfw_esc_callback);

        return bwin;
#endif
        break;
    default:
        break;
    }

    return NULL;
}



static void backend_poll_events(DvzBackend backend, DvzWindow* window)
{
    switch (backend)
    {
    case DVZ_BACKEND_GLFW:
#if HAS_GLFW
        glfwPollEvents();
#endif
        break;
    default:
        break;
    }
}



static void backend_wait(DvzBackend backend, DvzWindow* window)
{
    switch (backend)
    {
    case DVZ_BACKEND_GLFW:
#if HAS_GLFW
        glfwWaitEvents();
#endif
        break;
    default:
        break;
    }
}



static void
backend_window_destroy(VkInstance instance, DvzBackend backend, void* window, VkSurfaceKHR surface)
{
    log_trace("starting destruction of backend window...");
    // NOTE TODO: need to vkDeviceWaitIdle(device) on all devices before calling this

    switch (backend)
    {
    case DVZ_BACKEND_GLFW:
#if HAS_GLFW
        glfwPollEvents();
        ASSERT(window != NULL);
        log_trace("destroy GLFW window");
        glfwDestroyWindow((GLFWwindow*)window);
#endif
        break;
    default:
        break;
    }

    if (surface != VK_NULL_HANDLE)
    {
        log_trace("destroy surface");
        vkDestroySurfaceKHR(instance, surface, NULL);
    }

    log_trace("backend window destroyed");
}



static void backend_window_get_size(
    DvzBackend backend, void* window,                //
    uint32_t* window_width, uint32_t* window_height, //
    uint32_t* framebuffer_width, uint32_t* framebuffer_height)
{
    log_trace("determining the size of backend window...");

    switch (backend)
    {
    case DVZ_BACKEND_GLFW:;
#if HAS_GLFW
        int w, h;

        // Get window size.
        glfwGetWindowSize((GLFWwindow*)window, &w, &h);
        while (w == 0 || h == 0)
        {
            log_trace("waiting for end of window resize event");
            glfwGetWindowSize((GLFWwindow*)window, &w, &h);
            glfwWaitEvents();
        }
        ASSERT(w > 0);
        ASSERT(h > 0);
        *window_width = (uint32_t)w;
        *window_height = (uint32_t)h;
        log_trace("window size is %dx%d", w, h);

        // Get framebuffer size.
        glfwGetFramebufferSize((GLFWwindow*)window, &w, &h);
        while (w == 0 || h == 0)
        {
            log_trace("waiting for end of framebuffer resize event");
            glfwGetFramebufferSize((GLFWwindow*)window, &w, &h);
            glfwWaitEvents();
        }
        ASSERT(w > 0);
        ASSERT(h > 0);
        *framebuffer_width = (uint32_t)w;
        *framebuffer_height = (uint32_t)h;
        log_trace("framebuffer size is %dx%d", w, h);
#endif
        break;

    default:
        break;
    }
}



static void
backend_window_set_size(DvzBackend backend, void* window, uint32_t width, uint32_t height)
{
    log_trace("setting the size of backend window...");

    switch (backend)
    {
    case DVZ_BACKEND_GLFW:;
#if HAS_GLFW
        int w = (int)width, h = (int)height;
        log_trace("set window size to %dx%d", w, h);
        glfwSetWindowSize((GLFWwindow*)window, w, h);
#endif
        break;

    default:
        break;
    }
}



static bool backend_window_should_close(DvzBackend backend, void* window)
{
    switch (backend)
    {
    case DVZ_BACKEND_GLFW:;
#if HAS_GLFW
        if (window != NULL)
            return glfwWindowShouldClose((GLFWwindow*)window);
#endif
        break;
    default:
        break;
    }
    return false;
}



#endif
