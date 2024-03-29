/*************************************************************************************************/
/* Glyph                                                                                         */
/*************************************************************************************************/

#ifndef DVZ_HEADER_GLYPH
#define DVZ_HEADER_GLYPH



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include "../graphics.h"
#include "../viewport.h"
#include "../visual.h"



/*************************************************************************************************/
/*  Typedefs                                                                                     */
/*************************************************************************************************/

typedef struct DvzGlyphVertex DvzGlyphVertex;
typedef struct DvzGlyphParams DvzGlyphParams;

// Forward declarations.
typedef struct DvzBatch DvzBatch;
typedef struct DvzAtlas DvzAtlas;
typedef struct DvzVisual DvzVisual;



/*************************************************************************************************/
/*  Enums                                                                                        */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Structs                                                                                      */
/*************************************************************************************************/

struct DvzGlyphVertex
{
    vec3 pos;         /* 0: position */
    vec3 axis;        /* 1: axis */
    vec2 size;        /* 2: size */
    vec2 anchor;      /* 3: anchor */
    vec2 shift;       /* 4: shift */
    vec2 uv;          /* 5: texture coordinates */
    float angle;      /* 6: angle */
    cvec4 color;      /* 7: color */
    float group_size; /* 8: group_size */
};



struct DvzGlyphParams
{
    vec2 size;    /* glyph size in pixels */
    vec4 bgcolor; /* background color for glyph antialiasing*/
};



EXTERN_C_ON

/*************************************************************************************************/
/*  Functions                                                                                    */
/*************************************************************************************************/

/**
 *
 */
DVZ_EXPORT DvzVisual* dvz_glyph(DvzBatch* batch, int flags);



/**
 *
 */
DVZ_EXPORT void dvz_glyph_alloc(DvzVisual* visual, uint32_t item_count);



/**
 *
 */
DVZ_EXPORT void
dvz_glyph_position(DvzVisual* visual, uint32_t first, uint32_t count, vec3* values, int flags);



/**
 *
 */
DVZ_EXPORT void
dvz_glyph_axis(DvzVisual* visual, uint32_t first, uint32_t count, vec3* values, int flags);



/**
 *
 */
DVZ_EXPORT void
dvz_glyph_size(DvzVisual* visual, uint32_t first, uint32_t count, vec2* values, int flags);



/**
 *
 */
DVZ_EXPORT void
dvz_glyph_anchor(DvzVisual* visual, uint32_t first, uint32_t count, vec2* values, int flags);



/**
 *
 */
DVZ_EXPORT void
dvz_glyph_shift(DvzVisual* visual, uint32_t first, uint32_t count, vec2* values, int flags);



/**
 *
 */
DVZ_EXPORT void
dvz_glyph_texcoords(DvzVisual* visual, uint32_t first, uint32_t count, vec4* coords, int flags);



/**
 *
 */
DVZ_EXPORT void
dvz_glyph_angle(DvzVisual* visual, uint32_t first, uint32_t count, float* values, int flags);



/**
 *
 */
DVZ_EXPORT void
dvz_glyph_color(DvzVisual* visual, uint32_t first, uint32_t count, cvec4* values, int flags);



/**
 *
 */
DVZ_EXPORT void
dvz_glyph_groupsize(DvzVisual* visual, uint32_t first, uint32_t count, float* values, int flags);



/**
 *
 */
DVZ_EXPORT void dvz_glyph_bgcolor(DvzVisual* visual, vec4 bgcolor);



/**
 *
 */
DVZ_EXPORT void dvz_glyph_texture(DvzVisual* visual, DvzId tex);



/**
 *
 */
DVZ_EXPORT void dvz_glyph_atlas(DvzVisual* visual, DvzAtlas* atlas);



/**
 *
 */
DVZ_EXPORT void dvz_glyph_unicode(DvzVisual* visual, uint32_t count, uint32_t* codepoints);



/**
 *
 */
DVZ_EXPORT void dvz_glyph_ascii(DvzVisual* visual, const char* string);



/**
 *
 */
DVZ_EXPORT void dvz_glyph_xywh(
    DvzVisual* visual, uint32_t first, uint32_t count, vec4* values, vec2 offset, int flags);



EXTERN_C_OFF

#endif
