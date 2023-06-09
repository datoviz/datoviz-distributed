/*************************************************************************************************/
/*  Point                                                                                        */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include "scene/visuals/point.h"
#include "fileio.h"
#include "request.h"
#include "scene/graphics.h"
#include "scene/viewset.h"
#include "scene/visual.h"



/*************************************************************************************************/
/*  Macros                                                                                       */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Internal functions                                                                           */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Functions                                                                                    */
/*************************************************************************************************/

DvzVisual* dvz_point(DvzRequester* rqr, int flags)
{
    ANN(rqr);

    DvzVisual* point = dvz_visual(rqr, DVZ_PRIMITIVE_TOPOLOGY_POINT_LIST, flags);
    ANN(point);

    // Visual shaders.
    dvz_visual_shader(point, "graphics_basic");

    // Vertex attributes.
    dvz_visual_attr(point, 0, DVZ_FORMAT_R32G32B32_SFLOAT, 0); // pos
    dvz_visual_attr(point, 1, DVZ_FORMAT_R8G8B8A8_UNORM, 0);   // color

    // Uniforms.
    dvz_visual_dat(point, 0, sizeof(DvzMVP));
    dvz_visual_dat(point, 1, sizeof(DvzViewport));

    return point;
}



void dvz_point_alloc(DvzVisual* point, uint32_t item_count)
{
    ANN(point);
    log_debug("allocating the point visual");

    DvzRequester* rqr = point->rqr;
    ANN(rqr);

    // Create the visual.
    dvz_visual_alloc(point, item_count, item_count);
}



void dvz_point_position(DvzVisual* point, uint32_t first, uint32_t count, vec3* values, int flags)
{
    ANN(point);
    // _auto_create(point, first, count);
    dvz_visual_data(point, 0, first, count, (void*)values);
}



void dvz_point_color(DvzVisual* point, uint32_t first, uint32_t count, cvec4* values, int flags)
{
    ANN(point);
    // _auto_create(point, first, count);
    dvz_visual_data(point, 1, first, count, (void*)values);
}
