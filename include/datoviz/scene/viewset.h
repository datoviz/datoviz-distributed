/*************************************************************************************************/
/* Viewset                                                                                       */
/*************************************************************************************************/

#ifndef DVZ_HEADER_VIEWSET
#define DVZ_HEADER_VIEWSET



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include "../_atomic.h"
#include "../_enums.h"
#include "../_input.h"
#include "../_obj.h"
#include "dual.h"
#include "mvp.h"
#include "viewport.h"



/*************************************************************************************************/
/*  Enums                                                                                        */
/*************************************************************************************************/

// Mouse reference.
typedef enum
{
    DVZ_MOUSE_REFERENCE_GLOBAL, // global coordinate system, (0,0) -> (w,h) in screen coords
    DVZ_MOUSE_REFERENCE_LOCAL,  // (x0, y0) -> (vw, vh) in screen coords, relative to the View
    DVZ_MOUSE_REFERENCE_SCALED, // like local but rescaled in (-1,1)
} DvzMouseReference;



/*************************************************************************************************/
/*  Typedefs                                                                                     */
/*************************************************************************************************/

typedef struct DvzView DvzView;
typedef struct DvzViewset DvzViewset;

// Forward declarations.
typedef struct DvzRequester DvzRequester;
typedef struct DvzVisual DvzVisual;
typedef struct DvzList DvzList;
typedef struct DvzTransform DvzTransform;



/*************************************************************************************************/
/*  Structs                                                                                      */
/*************************************************************************************************/

struct DvzView
{
    DvzViewset* viewset; // reference to the parent viewset
    DvzDual dual;
    vec2 offset, shape; // in framebuffer pixels
    float scale;        // scale (multiplied by the window's content scale)
    DvzList* visuals;   // list of visuals in the view
};



struct DvzViewset
{
    DvzRequester* rqr;
    DvzAtomic status;
    DvzId canvas_id;
    DvzList* views;
};



EXTERN_C_ON

/*************************************************************************************************/
/*  Viewset                                                                                      */
/*************************************************************************************************/

/**
 *
 */
DVZ_EXPORT DvzViewset* dvz_viewset(DvzRequester* rqr, DvzId canvas_id);



/**
 *
 */
DVZ_EXPORT void dvz_viewset_clear(DvzViewset* viewset);



/**
 *
 */
DVZ_EXPORT void dvz_viewset_build(DvzViewset* viewset);



/**
 *
 */
DVZ_EXPORT void dvz_viewset_destroy(DvzViewset* viewset);



/*************************************************************************************************/
/*  View                                                                                         */
/*************************************************************************************************/

/**
 *
 */
DVZ_EXPORT DvzView* dvz_view(DvzViewset* viewset, vec2 offset, vec2 shape);



/**
 *
 */
DVZ_EXPORT void dvz_view_add(
    DvzView* view, DvzVisual* visual,                 //
    uint32_t first, uint32_t count,                   // items
    uint32_t first_instance, uint32_t instance_count, // instances
    DvzTransform* transform, int viewport_flags);     // transform and viewport flags



/**
 *
 */
DVZ_EXPORT DvzMouseEvent
dvz_view_mouse(DvzView* view, DvzMouseEvent ev, float content_scale, DvzMouseReference ref);



/**
 *
 */
DVZ_EXPORT void dvz_view_clear(DvzView* view);



/**
 *
 */
DVZ_EXPORT void dvz_view_resize(DvzView* view, vec2 offset, vec2 shape);



/**
 *
 */
DVZ_EXPORT void dvz_view_destroy(DvzView* view);



EXTERN_C_OFF

#endif
