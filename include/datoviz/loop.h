/*************************************************************************************************/
/*  Simple loop for single canvases                                                              */
/*************************************************************************************************/

#ifndef DVZ_HEADER_LOOP
#define DVZ_HEADER_LOOP



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include "canvas.h"
#include "common.h"
#include "surface.h"
#include "window.h"



/*************************************************************************************************/
/*  Typedefs                                                                                     */
/*************************************************************************************************/

typedef struct DvzLoop DvzLoop;

// Callbacks.
typedef void (*DvzLoopOverlay)(DvzLoop* loop, void* user_data);

// Forward declarations.
typedef struct DvzGui DvzGui;
typedef struct DvzGuiWindow DvzGuiWindow;



/*************************************************************************************************/
/*  Structs                                                                                      */
/*************************************************************************************************/

struct DvzLoop
{
    DvzGpu* gpu;
    int flags;

    uint64_t frame_idx;
    DvzRenderpass renderpass;
    DvzSurface surface;
    DvzCanvas canvas;
    DvzWindow window;

    DvzGui* gui;
    DvzGuiWindow* gui_window;

    DvzLoopOverlay overlay;
    void* overlay_data;
};



EXTERN_C_ON

/*************************************************************************************************/
/*  Functions                                                                                    */
/*************************************************************************************************/

/**
 * Create a loop object.
 *
 * @param gpu the GPU
 * @param flags the flags
 */
DVZ_EXPORT DvzLoop* dvz_loop(DvzGpu* gpu, uint32_t width, uint32_t height, int flags);

/**
 */
DVZ_EXPORT void dvz_loop_refill(DvzLoop* loop, DvzCanvasRefill callback, void* user_data);

/**
 */
DVZ_EXPORT void dvz_loop_overlay(DvzLoop* loop, DvzLoopOverlay callback, void* user_data);

/**
 */
DVZ_EXPORT int dvz_loop_frame(DvzLoop* loop);

/**
 */
DVZ_EXPORT void dvz_loop_run(DvzLoop* loop, uint64_t n_frames);

/**
 */
DVZ_EXPORT void dvz_loop_destroy(DvzLoop* loop);



EXTERN_C_OFF

#endif
