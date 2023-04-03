/*************************************************************************************************/
/* Baker                                                                                         */
/*************************************************************************************************/

#ifndef DVZ_HEADER_BAKER
#define DVZ_HEADER_BAKER



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include "_enums.h"
#include "_log.h"
#include "_math.h"
#include "scene/dual.h"



/*************************************************************************************************/
/*  Typedefs                                                                                     */
/*************************************************************************************************/

typedef struct DvzBaker DvzBaker;
typedef struct DvzBakerVertex DvzBakerVertex;
typedef struct DvzBakerAttr DvzBakerAttr;
typedef struct DvzBakerDescriptor DvzBakerDescriptor;

// Forward declarations.
typedef struct DvzRequester DvzRequester;
typedef struct DvzArray DvzArray;



/*************************************************************************************************/
/*  Enums                                                                                        */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Structs                                                                                      */
/*************************************************************************************************/

struct DvzBakerVertex
{
    uint32_t binding_idx;
    DvzSize stride;
    DvzDual dual;
};

struct DvzBakerAttr
{
    uint32_t attr_idx;
    uint32_t binding_idx;
    DvzSize offset;
    DvzSize item_size;
};

struct DvzBakerDescriptor
{
    uint32_t slot_idx;
    DvzSize item_size;
    DvzDual dual;
};



struct DvzBaker
{
    DvzRequester* rqr;
    int flags;

    uint32_t binding_count;
    uint32_t attr_count;
    uint32_t slot_count;

    DvzBakerAttr vertex_attrs[DVZ_MAX_VERTEX_ATTRS];
    DvzBakerVertex vertex_bindings[DVZ_MAX_VERTEX_BINDINGS];
    DvzBakerDescriptor descriptors[DVZ_MAX_BINDINGS];

    // TODO:
    // DvzDual indexed;
    // DvzDual indirect;
};



EXTERN_C_ON

/*************************************************************************************************/
/*  Functions                                                                                    */
/*************************************************************************************************/

// 00xx: which attributes should be in a different buf (8 max)
// xx00: which attributes should be constants
DVZ_EXPORT DvzBaker* dvz_baker(DvzRequester* rqr, int flags);



// declare a vertex binding
DVZ_EXPORT void dvz_baker_vertex(DvzBaker* baker, uint32_t binding_idx, DvzSize stride);



// declare a GLSL attribute
DVZ_EXPORT void dvz_baker_attr(
    DvzBaker* baker, uint32_t attr_idx, uint32_t binding_idx, DvzSize offset, DvzSize item_size);



DVZ_EXPORT void dvz_baker_indexed(DvzBaker* baker);



DVZ_EXPORT void dvz_baker_indirect(DvzBaker* baker);



// declare a descriptor slot
DVZ_EXPORT void dvz_baker_slot(DvzBaker* baker, uint32_t slot_idx, DvzSize item_size);



// Internal function, used to instantiate the DvzDual instances.
DVZ_EXPORT void dvz_baker_create(DvzBaker* baker, uint32_t vertex_count);



DVZ_EXPORT void
dvz_baker_data(DvzBaker* baker, uint32_t attr_idx, uint32_t first, uint32_t count, void* data);



DVZ_EXPORT void dvz_baker_resize(DvzBaker* baker, uint32_t vertex_count);



DVZ_EXPORT void dvz_baker_repeat(
    DvzBaker* baker, uint32_t attr_idx, uint32_t first, uint32_t count, uint32_t repeats,
    void* data);



DVZ_EXPORT void dvz_baker_quads(
    DvzBaker* baker, uint32_t attr_idx, vec2 quad_size, uint32_t count, vec2* positions);



DVZ_EXPORT void dvz_baker_uniform(DvzBaker* baker, uint32_t binding_idx, DvzSize size, void* data);



// emit the dat update commands to synchronize the dual arrays on the GPU
DVZ_EXPORT void dvz_baker_update(DvzBaker* baker);



DVZ_EXPORT void dvz_baker_destroy(DvzBaker* baker);



EXTERN_C_OFF

#endif
