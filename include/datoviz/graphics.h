/*************************************************************************************************/
/*  Collection of builtin graphics pipelines                                                     */
/*************************************************************************************************/

#ifndef DVZ_HEADER_GRAPHICS
#define DVZ_HEADER_GRAPHICS



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include "_macros.h"
#include "array.h"
// #include "fileio.h"
#include "vklite.h"



/*************************************************************************************************/
/*  Constants and macros                                                                         */
/*************************************************************************************************/

// Number of common bindings
// NOTE: must correspond to the same constant in common.glsl
#define DVZ_USER_BINDING 2

#define DVZ_MAX_GLYPHS_PER_TEXT 256



/*************************************************************************************************/
/*  Enums                                                                                        */
/*************************************************************************************************/

// Viewport type.
// NOTE: must correspond to values in common.glsl
typedef enum
{
    DVZ_VIEWPORT_FULL,
    DVZ_VIEWPORT_INNER,
    DVZ_VIEWPORT_OUTER,
    DVZ_VIEWPORT_OUTER_BOTTOM,
    DVZ_VIEWPORT_OUTER_LEFT,
} DvzViewportClip;



// Graphics flags.
typedef enum
{
    DVZ_GRAPHICS_FLAGS_DEPTH_TEST = 0x0100,
    DVZ_GRAPHICS_FLAGS_PICK = 0x0200,
} DvzGraphicsFlags;



// Graphics builtins
typedef enum
{
    DVZ_GRAPHICS_NONE,
    DVZ_GRAPHICS_POINT,

    DVZ_GRAPHICS_LINE,
    DVZ_GRAPHICS_LINE_STRIP,
    DVZ_GRAPHICS_TRIANGLE,
    DVZ_GRAPHICS_TRIANGLE_STRIP,
    DVZ_GRAPHICS_TRIANGLE_FAN,

    DVZ_GRAPHICS_MARKER,

    DVZ_GRAPHICS_SEGMENT,
    DVZ_GRAPHICS_ARROW,
    DVZ_GRAPHICS_PATH,
    DVZ_GRAPHICS_TEXT,

    DVZ_GRAPHICS_IMAGE,
    DVZ_GRAPHICS_IMAGE_CMAP,

    DVZ_GRAPHICS_VOLUME_SLICE,
    DVZ_GRAPHICS_MESH,

    DVZ_GRAPHICS_FAKE_SPHERE,
    DVZ_GRAPHICS_VOLUME,

    DVZ_GRAPHICS_COUNT,
    DVZ_GRAPHICS_CUSTOM,
} DvzGraphicsType;



// Marker type.
// NOTE: the numbers need to correspond to markers.glsl at the bottom.
typedef enum
{
    DVZ_MARKER_DISC = 0,
    DVZ_MARKER_ASTERISK = 1,
    DVZ_MARKER_CHEVRON = 2,
    DVZ_MARKER_CLOVER = 3,
    DVZ_MARKER_CLUB = 4,
    DVZ_MARKER_CROSS = 5,
    DVZ_MARKER_DIAMOND = 6,
    DVZ_MARKER_ARROW = 7,
    DVZ_MARKER_ELLIPSE = 8,
    DVZ_MARKER_HBAR = 9,
    DVZ_MARKER_HEART = 10,
    DVZ_MARKER_INFINITY = 11,
    DVZ_MARKER_PIN = 12,
    DVZ_MARKER_RING = 13,
    DVZ_MARKER_SPADE = 14,
    DVZ_MARKER_SQUARE = 15,
    DVZ_MARKER_TAG = 16,
    DVZ_MARKER_TRIANGLE = 17,
    DVZ_MARKER_VBAR = 18,
    DVZ_MARKER_COUNT,
} DvzMarkerType;



// Cap type.
typedef enum
{
    DVZ_CAP_TYPE_NONE = 0,
    DVZ_CAP_ROUND = 1,
    DVZ_CAP_TRIANGLE_IN = 2,
    DVZ_CAP_TRIANGLE_OUT = 3,
    DVZ_CAP_SQUARE = 4,
    DVZ_CAP_BUTT = 5,
    DVZ_CAP_COUNT,
} DvzCapType;



// Joint type.
typedef enum
{
    DVZ_JOIN_SQUARE = false,
    DVZ_JOIN_ROUND = true,
} DvzJoinType;



// Path topology.
typedef enum
{
    DVZ_PATH_OPEN,
    DVZ_PATH_CLOSED,
} DvzPathTopology;



/*************************************************************************************************/
/*  Typedefs                                                                                     */
/*************************************************************************************************/

typedef struct DvzVertex DvzVertex;
typedef struct DvzMVP DvzMVP;
typedef struct DvzViewport DvzViewport;

typedef struct DvzGraphicsPointParams DvzGraphicsPointParams;

typedef struct DvzGraphicsMarkerVertex DvzGraphicsMarkerVertex;
typedef struct DvzGraphicsMarkerParams DvzGraphicsMarkerParams;

typedef struct DvzGraphicsSegmentVertex DvzGraphicsSegmentVertex;

typedef struct DvzGraphicsPathVertex DvzGraphicsPathVertex;
typedef struct DvzGraphicsPathParams DvzGraphicsPathParams;

typedef struct DvzGraphicsImageVertex DvzGraphicsImageVertex;
typedef struct DvzGraphicsImageParams DvzGraphicsImageParams;
typedef struct DvzGraphicsImageCmapParams DvzGraphicsImageCmapParams;

typedef struct DvzGraphicsVolumeSliceVertex DvzGraphicsVolumeSliceVertex;
typedef struct DvzGraphicsVolumeSliceParams DvzGraphicsVolumeSliceParams;

typedef struct DvzGraphicsVolumeVertex DvzGraphicsVolumeVertex;
typedef struct DvzGraphicsVolumeParams DvzGraphicsVolumeParams;

typedef struct DvzGraphicsMeshVertex DvzGraphicsMeshVertex;
typedef struct DvzGraphicsMeshParams DvzGraphicsMeshParams;

typedef struct DvzGraphicsTextParams DvzGraphicsTextParams;
typedef struct DvzGraphicsTextVertex DvzGraphicsTextVertex;


// Forward declarations.
typedef struct DvzRenderpass DvzRenderpass;
typedef struct DvzGraphics DvzGraphics;



/*************************************************************************************************/
/*  Structs                                                                                      */
/*************************************************************************************************/

struct DvzVertex
{
    vec3 pos;    /* position */
    cvec4 color; /* color */
};



struct DvzMVP
{
    mat4 model;
    mat4 view;
    mat4 proj;
    float time;
};



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



/*************************************************************************************************/
/*  Graphics point                                                                               */
/*************************************************************************************************/

struct DvzGraphicsPointParams
{
    float point_size; /* point size, in pixels */
};



/*************************************************************************************************/
/*  Graphics marker                                                                              */
/*************************************************************************************************/

struct DvzGraphicsMarkerVertex
{
    vec3 pos;    /* position */
    cvec4 color; /* color */
    float size;  /* marker size, in pixels */
    // in fact a DvzMarkerType but we should control the exact data type for the GPU
    uint8_t marker;    /* marker type enum */
    uint8_t angle;     /* angle, between 0 (0) included and 256 (M_2PI) excluded */
    uint8_t transform; /* transform enum */
};

struct DvzGraphicsMarkerParams
{
    vec4 edge_color;  /* edge color RGBA */
    float edge_width; /* line width, in pixels */
};



/*************************************************************************************************/
/*  Graphics segment                                                                             */
/*************************************************************************************************/

struct DvzGraphicsSegmentVertex
{
    vec3 P0;           /* start position */
    vec3 P1;           /* end position */
    vec4 shift;        /* shift of start (xy) and end (zw) positions, in pixels */
    cvec4 color;       /* color */
    float linewidth;   /* line width, in pixels */
    DvzCapType cap0;   /* start cap enum */
    DvzCapType cap1;   /* end cap enum */
    uint8_t transform; /* transform enum */
};



/*************************************************************************************************/
/*  Graphics segment                                                                             */
/*************************************************************************************************/

struct DvzGraphicsPathVertex
{
    vec3 p0;     /* previous position */
    vec3 p1;     /* current position */
    vec3 p2;     /* next position */
    vec3 p3;     /* next next position */
    cvec4 color; /* point color */
};

struct DvzGraphicsPathParams
{
    float linewidth;    /* line width in pixels */
    float miter_limit;  /* miter limit for joins */
    int32_t cap_type;   /* type of the ends of the path */
    int32_t round_join; /* whether to use round joins */
};



/*************************************************************************************************/
/*  Graphics text                                                                                */
/*************************************************************************************************/

struct DvzGraphicsTextVertex
{
    vec3 pos;          /* position */
    vec2 shift;        /* shift, in pixels */
    cvec4 color;       /* color */
    vec2 glyph_size;   /* glyph size, in pixels */
    vec2 anchor;       /* character anchor, in normalized coordinates */
    float angle;       /* string angle */
    usvec4 glyph;      /* glyph: char code, char index, string length, string index */
    uint8_t transform; /* transform enum */
};

struct DvzGraphicsTextParams
{
    ivec2 grid_size; /* font atlas grid size (rows, columns) */
    ivec2 tex_size;  /* font atlas texture size, in pixels */
};



/*************************************************************************************************/
/*  Graphics image                                                                               */
/*************************************************************************************************/

struct DvzGraphicsImageVertex
{
    vec3 pos; /* position */
    vec2 uv;  /* tex coordinates */
};

struct DvzGraphicsImageParams
{
    vec4 tex_coefs; /* blending coefficients for the four images */
};

struct DvzGraphicsImageCmapParams
{
    vec2 vrange; /* value range */
    int cmap;    /* colormap number */
};



/*************************************************************************************************/
/*  Graphics volume slice                                                                        */
/*************************************************************************************************/

struct DvzGraphicsVolumeSliceVertex
{
    vec3 pos; /* position */
    vec3 uvw; /* 3D tex coords */
};

struct DvzGraphicsVolumeSliceParams
{
    vec4 x_cmap; /* x values of the color transfer function */
    vec4 y_cmap; /* y values of the color transfer function */

    vec4 x_alpha; /* x values of the alpha transfer function */
    vec4 y_alpha; /* y values of the alpha transfer function */

    int32_t cmap; /* colormap */
    float scale;  /* scaling factor for the fetched volume values */
};



/*************************************************************************************************/
/*  Graphics volume                                                                              */
/*************************************************************************************************/

struct DvzGraphicsVolumeVertex
{
    vec3 pos; /* position */
};

struct DvzGraphicsVolumeParams
{
    vec4 box_size;        /* size of the box containing the volume, in NDC */
    vec4 uvw0;            /* texture coordinates of the 2 corner points */
    vec4 uvw1;            /* texture coordinates of the 2 corner points */
    vec4 clip;            /* plane normal vector for volume slicing */
    vec2 transfer_xrange; /* x coords of the endpoints of the transfer function */
    float color_coef;     /* scaling coefficient when fetching voxel color */
    // int32_t cmap;         /* colormap */
};



/*************************************************************************************************/
/*  Graphics mesh                                                                                */
/*************************************************************************************************/

struct DvzGraphicsMeshVertex
{
    // HACK: use vec4 for alignment when accessing from compute shader (need std140 on GPU)
    vec4 pos;      /* position */
    vec4 normal;   /* normal vector */
    vec2 uv;       /* tex coords */
    uint8_t alpha; /* transparency value */
};

struct DvzGraphicsMeshParams
{
    mat4 lights_pos_0;    /* positions of each of the maximum four lights */
    mat4 lights_params_0; /* ambient, diffuse, specular coefs for each light */
    vec4 tex_coefs;       /* blending coefficients for the four textures */
    vec4 clip_coefs;      /* clip coefficients */
};

static DvzGraphicsMeshParams default_graphics_mesh_params(vec3 eye)
{
    DvzGraphicsMeshParams params = {0};
    params.lights_params_0[0][0] = 0.2;  // ambient coefficient
    params.lights_params_0[0][1] = 0.5;  // diffuse coefficient
    params.lights_params_0[0][2] = 0.3;  // specular coefficient
    params.lights_params_0[0][3] = 32.0; // specular exponent
    params.lights_pos_0[0][0] = -1;      // light position
    params.lights_pos_0[0][1] = 1;       //
    params.lights_pos_0[0][2] = +10;     //
    params.tex_coefs[0] = 1;             // texture blending coefficients
    return params;
}



EXTERN_C_ON

/*************************************************************************************************/
/*  Functions                                                                                    */
/*************************************************************************************************/

/**
 * Create a new graphics pipeline of a given builtin type.
 *
 * @param renderpass the renderpass
 * @param graphics the graphics to create
 * @param type the graphics type
 * @param flags the creation flags for the graphics
 */
DVZ_EXPORT void dvz_graphics_builtin(
    DvzRenderpass* renderpass, DvzGraphics* graphics, DvzGraphicsType type, int flags);



/**
 * Return a default viewport
 *
 * @param width the viewport width, in framebuffer pixels
 * @param height the viewport height, in framebuffer pixels
 * @returns the viewport
 */
DVZ_EXPORT DvzViewport dvz_viewport_default(uint32_t width, uint32_t height);



EXTERN_C_OFF

#endif
