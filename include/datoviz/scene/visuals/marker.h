/*************************************************************************************************/
/* Marker                                                                                        */
/*************************************************************************************************/

#ifndef DVZ_HEADER_MARKER
#define DVZ_HEADER_MARKER



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include "../viewport.h"
#include "../visual.h"



/*************************************************************************************************/
/*  Typedefs                                                                                     */
/*************************************************************************************************/

typedef struct DvzMarkerVertex DvzMarkerVertex;
typedef struct DvzMarkerParams DvzMarkerParams;

// Forward declarations.
typedef struct DvzBatch DvzBatch;
typedef struct DvzVisual DvzVisual;



/*************************************************************************************************/
/*  Enums                                                                                        */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Structs                                                                                      */
/*************************************************************************************************/

struct DvzMarkerVertex
{
    vec3 pos;    /* position */
    float size;  /* size */
    float angle; /* angle */
    cvec4 color; /* color */
};



struct DvzMarkerParams
{
    vec4 edge_color;
    float edge_width;
    float tex_scale;
};



EXTERN_C_ON

/*************************************************************************************************/
/*  Functions                                                                                    */
/*************************************************************************************************/

/**
 *
 */
DVZ_EXPORT DvzVisual* dvz_marker(DvzBatch* batch, int flags);



/**
 *
 */
DVZ_EXPORT void dvz_marker_mode(DvzVisual* visual, DvzMarkerMode mode);



/**
 *
 */
DVZ_EXPORT void dvz_marker_aspect(DvzVisual* visual, DvzMarkerAspect aspect);



/**
 *
 */
DVZ_EXPORT void dvz_marker_shape(DvzVisual* visual, DvzMarkerShape shape);



/**
 *
 */
DVZ_EXPORT void
dvz_marker_position(DvzVisual* marker, uint32_t first, uint32_t count, vec3* values, int flags);



/**
 *
 */
DVZ_EXPORT void
dvz_marker_size(DvzVisual* marker, uint32_t first, uint32_t count, float* values, int flags);



/**
 *
 */
DVZ_EXPORT void
dvz_marker_angle(DvzVisual* marker, uint32_t first, uint32_t count, float* values, int flags);



/**
 *
 */
DVZ_EXPORT void
dvz_marker_color(DvzVisual* marker, uint32_t first, uint32_t count, cvec4* values, int flags);



/**
 *
 */
DVZ_EXPORT void dvz_marker_edge_color(DvzVisual* visual, cvec4 value);



/**
 *
 */
DVZ_EXPORT void dvz_marker_edge_width(DvzVisual* visual, float value);



/**
 *
 */
DVZ_EXPORT void dvz_marker_tex(DvzVisual* visual, DvzId tex, DvzId sampler);



/**
 *
 */
DVZ_EXPORT void dvz_marker_tex_scale(DvzVisual* visual, float value);



/**
 *
 */
DVZ_EXPORT void dvz_marker_alloc(DvzVisual* marker, uint32_t item_count);



EXTERN_C_OFF

#endif
