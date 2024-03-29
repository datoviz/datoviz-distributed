/*************************************************************************************************/
/* Mesh                                                                                          */
/*************************************************************************************************/

#ifndef DVZ_HEADER_MESH
#define DVZ_HEADER_MESH



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include "../viewport.h"
#include "../visual.h"



/*************************************************************************************************/
/*  Typedefs                                                                                     */
/*************************************************************************************************/

typedef struct DvzMeshColorVertex DvzMeshColorVertex;
typedef struct DvzMeshTexturedVertex DvzMeshTexturedVertex;
typedef struct DvzMeshParams DvzMeshParams;

// Forward declarations.
typedef struct DvzBatch DvzBatch;
typedef struct DvzVisual DvzVisual;
typedef struct DvzShape DvzShape;



/*************************************************************************************************/
/*  Enums                                                                                        */
/*************************************************************************************************/

typedef enum
{
    DVZ_MESH_FLAGS_NONE = 0x0000,
    DVZ_MESH_FLAGS_TEXTURED = 0x0001,
    DVZ_MESH_FLAGS_LIGHTING = 0x0002,
} DvzMeshFlags;



/*************************************************************************************************/
/*  Structs                                                                                      */
/*************************************************************************************************/

struct DvzMeshColorVertex
{
    // HACK: use vec4 for alignment when accessing from compute shader (need std140 on GPU)
    vec3 pos;    /* position */
    vec3 normal; /* normal vector */
    cvec4 color; /* color */
};

struct DvzMeshTexturedVertex
{
    // HACK: use vec4 for alignment when accessing from compute shader (need std140 on GPU)
    vec3 pos;       /* position */
    vec3 normal;    /* normal vector */
    vec4 texcoords; /* u, v, *, a */
};



struct DvzMeshParams
{
    vec4 light_pos;    /* light position */
    vec4 light_params; /* ambient, diffuse, specular coefs */
};



EXTERN_C_ON

/*************************************************************************************************/
/*  Functions                                                                                    */
/*************************************************************************************************/

/**
 *
 */
DVZ_EXPORT DvzVisual* dvz_mesh(DvzBatch* batch, int flags);



/**
 *
 */
DVZ_EXPORT void
dvz_mesh_position(DvzVisual* mesh, uint32_t first, uint32_t count, vec3* values, int flags);



/**
 *
 */
DVZ_EXPORT void
dvz_mesh_color(DvzVisual* mesh, uint32_t first, uint32_t count, cvec4* values, int flags);



/**
 * vec4: u, v, <unused>, a
 */
DVZ_EXPORT void
dvz_mesh_texcoords(DvzVisual* mesh, uint32_t first, uint32_t count, vec4* values, int flags);



/**
 *
 */
DVZ_EXPORT
void dvz_mesh_normal(DvzVisual* mesh, uint32_t first, uint32_t count, vec3* values, int flags);



/**
 *
 */
DVZ_EXPORT void dvz_mesh_texture(
    DvzVisual* visual, DvzId tex, DvzFilter filter, DvzSamplerAddressMode address_mode);



/**
 *
 */
DVZ_EXPORT void dvz_mesh_index(DvzVisual* mesh, uint32_t first, uint32_t count, DvzIndex* values);



/**
 *
 */
DVZ_EXPORT void dvz_mesh_alloc(DvzVisual* mesh, uint32_t vertex_count, uint32_t index_count);



/**
 *
 */
DVZ_EXPORT void dvz_mesh_light_pos(DvzVisual* mesh, vec4 pos);



/**
 *
 */
DVZ_EXPORT void dvz_mesh_light_params(DvzVisual* mesh, vec4 params);



/**
 *
 */
DVZ_EXPORT DvzVisual* dvz_mesh_shape(DvzBatch* batch, DvzShape* shape, int flags);



EXTERN_C_OFF

#endif
