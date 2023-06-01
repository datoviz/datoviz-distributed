/*************************************************************************************************/
/*  Canvas                                                                                       */
/*************************************************************************************************/

#ifndef DVZ_HEADER_CANVAS
#define DVZ_HEADER_CANVAS



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include "_enums.h"
#include "surface.h"
#include "vklite.h"



/*************************************************************************************************/
/*  Constants                                                                                    */
/*************************************************************************************************/

// #define DVZ_PICK_IMAGE_FORMAT    VK_FORMAT_R32G32B32A32_SINT
// #define DVZ_PICK_STAGING_SIZE    8

#define DVZ_MIN_SWAPCHAIN_IMAGE_COUNT 3
#define DVZ_SEMAPHORE_IMG_AVAILABLE   0
#define DVZ_SEMAPHORE_RENDER_FINISHED 1
#define DVZ_FENCE_RENDER_FINISHED     0
#define DVZ_FENCES_FLIGHT             1
#define DVZ_DEFAULT_COMMANDS_TRANSFER 0
#define DVZ_DEFAULT_COMMANDS_RENDER   1
#define DVZ_MAX_FRAMES_IN_FLIGHT      2



/*************************************************************************************************/
/*  Enums                                                                                        */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Typedefs                                                                                     */
/*************************************************************************************************/

typedef struct DvzCanvas DvzCanvas;
typedef struct DvzRender DvzRender;
typedef struct DvzSync DvzSync;

typedef void (*DvzCanvasRefill)(
    DvzCanvas* canvas, DvzCommands* cmds, uint32_t idx, void* user_data);

// Forward declarations.
typedef struct DvzRecorder DvzRecorder;



/*************************************************************************************************/
/*  Structs                                                                                      */
/*************************************************************************************************/

struct DvzRender
{
    DvzSwapchain swapchain;
    DvzImages depth;
    DvzImages staging;

    // TODO: picking, screencast
    // DvzImages pick_image;
    // DvzImages pick_staging;

    // DvzBuffer screencast_staging;
    // DvzImages* screencast_img;

    DvzFramebuffers framebuffers;
    DvzRenderpass* renderpass;

    DvzSubmit submit;
};



struct DvzSync
{
    DvzSemaphores sem_img_available;
    DvzSemaphores sem_render_finished;
    DvzSemaphores* present_semaphores;
    DvzFences fences_render_finished;
    DvzFences fences_flight;
};



/*************************************************************************************************/
/*  Canvas struct                                                                                */
/*************************************************************************************************/

struct DvzCanvas
{
    DvzObject obj;
    DvzGpu* gpu;
    int flags;

    DvzSurface surface;
    DvzFormat format;
    // cvec4 clear_color;
    uint32_t width, height;

    DvzSize size; // width*height*3
    uint8_t* rgb; // GPU buffer storing the image

    DvzCommands cmds;
    DvzCanvasRefill refill; // refill callback, only used in conjunction with dvz_loop()
    void* refill_data;

    DvzRender render;
    DvzSync sync;

    // Frames.
    uint32_t cur_frame; // current frame within the images in flight
    uint64_t frame_idx;
    bool resized;

    DvzRecorder* recorder; // used to record command buffer when using the presenter
    void* user_data;
};



EXTERN_C_ON

/*************************************************************************************************/
/*  Canvas functions                                                                             */
/*************************************************************************************************/

/**
 * Initialize a canvas.
 *
 * @returns a canvas
 */
DVZ_EXPORT DvzCanvas
dvz_canvas(DvzGpu* gpu, DvzRenderpass* renderpass, uint32_t width, uint32_t height, int flags);



/**
 * Create a canvas.
 *
 * @param canvas a canvas
 */
DVZ_EXPORT void dvz_canvas_create(DvzCanvas* canvas, DvzSurface surface);



/**
 * Reset a canvas
 *
 * @param canvas a canvas
 */
DVZ_EXPORT void dvz_canvas_reset(DvzCanvas* canvas);



/**
 * Recreate a canvas
 *
 * @param canvas a canvas
 */
DVZ_EXPORT void dvz_canvas_recreate(DvzCanvas* canvas);



/**
 * Register a refill callback.
 *
 * Only to be used in conjunction with dvz_loop().
 *
 * @param canvas a canvas
 * @param refill refill callback
 */
DVZ_EXPORT void dvz_canvas_refill(DvzCanvas* canvas, DvzCanvasRefill refill, void* user_data);



/**
 * Start rendering to the canvas in a command buffer.
 *
 * @param canvas the canvas
 * @param cmds the commands instance
 * @param idx the command buffer index with the commands instance
 */
DVZ_EXPORT void dvz_canvas_begin(DvzCanvas* canvas, DvzCommands* cmds, uint32_t idx);



/**
 * Set the viewport when filling a command buffer.
 *
 * @param canvas the canvas
 * @param cmds the commands instance
 * @param idx the command buffer index with the commands instance
 * @param offset the viewport offset (x, y)
 * @param size the viewport size (w, h)
 */
DVZ_EXPORT void dvz_canvas_viewport( //
    DvzCanvas* canvas, DvzCommands* cmds, uint32_t idx, vec2 offset, vec2 size);



/**
 * Stop rendering to the canvas in a command buffer.
 *
 * @param canvas the canvas
 * @param cmds the commands instance
 * @param idx the command buffer index with the commands instance
 */
DVZ_EXPORT void dvz_canvas_end(DvzCanvas* canvas, DvzCommands* cmds, uint32_t idx);



/**
 * Destroy a canvas.
 *
 * @param canvas the canvas to destroy
 */
DVZ_EXPORT void dvz_canvas_destroy(DvzCanvas* canvas);



EXTERN_C_OFF

#endif
