/*************************************************************************************************/
/*  Viewport                                                                                     */
/*************************************************************************************************/

#ifndef DVZ_HEADER_VIEWPORT
#define DVZ_HEADER_VIEWPORT



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include "vklite.h"



/*************************************************************************************************/
/*  Typedefs                                                                                     */
/*************************************************************************************************/

typedef struct DvzViewport DvzViewport;



/*************************************************************************************************/
/*  Structs                                                                                      */
/*************************************************************************************************/

// NOTE: must correspond to the shader structure in common.glsl
struct DvzViewport
{
    VkViewport viewport; // Vulkan viewport
    vec4 margins;

    // Position and size of the viewport in screen coordinates.
    uvec2 offset_screen;
    uvec2 size_screen;

    // Position and size of the viewport in framebuffer coordinates.
    uvec2 offset_framebuffer;
    uvec2 size_framebuffer;

    // Options
    // Viewport clipping.
    DvzViewportClip clip; // used by the GPU for viewport clipping

    // Used to discard transform on one axis
    int32_t interact_axis;

    // TODO: aspect ratio
};



#endif