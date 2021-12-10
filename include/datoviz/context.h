/*************************************************************************************************/
/*  GPU context holding the Resources, DatAlloc, Transfers instances                             */
/*************************************************************************************************/

#ifndef DVZ_HEADER_CONTEXT
#define DVZ_HEADER_CONTEXT



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include "common.h"
#include "datalloc.h"
#include "resources.h"
#include "transfers.h"
#include "vklite.h"



/*************************************************************************************************/
/*  Typedefs                                                                                     */
/*************************************************************************************************/

typedef struct DvzContext DvzContext;



/*************************************************************************************************/
/*  Structs                                                                                      */
/*************************************************************************************************/

struct DvzContext
{
    DvzObject obj;
    DvzGpu* gpu;

    DvzResources res;
    DvzDatAlloc datalloc;
    DvzTransfers transfers;
};



EXTERN_C_ON

/*************************************************************************************************/
/*  Context                                                                                      */
/*************************************************************************************************/

/**
 * Create a context associated to a GPU.
 *
 * !!! note
 *     The GPU must have been created beforehand.
 *
 * @param gpu the GPU
 */
DVZ_EXPORT DvzContext* dvz_context(DvzGpu* gpu);



/**
 * Wait for all pending transfers to finish.
 *
 * @param ctx the Context
 */
DVZ_EXPORT void dvz_context_wait(DvzContext* ctx);



/**
 * Destroy a context.
 *
 * @param context the context
 */
DVZ_EXPORT void dvz_context_destroy(DvzContext* context);



/*************************************************************************************************/
/*  Default initializers                                                                         */
/*************************************************************************************************/

/**
 * Initialize a default GPU for offscreen rendering.
 *
 * @param returns the GPU
 */
DVZ_EXPORT DvzGpu* dvz_init_offscreen(void);



/**
 * Initialize a default GPU for GLFW rendering.
 *
 * @param returns the GPU
 */
DVZ_EXPORT DvzGpu* dvz_init_glfw(void);



EXTERN_C_OFF

#endif
