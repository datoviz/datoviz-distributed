/*************************************************************************************************/
/*  Holds graphics and computes pipes                                                            */
/*************************************************************************************************/

#ifndef DVZ_HEADER_PIPELIB
#define DVZ_HEADER_PIPELIB



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include "common.h"
#include "graphics.h"
#include "pipe.h"



/*************************************************************************************************/
/*  Constants                                                                                    */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Enums                                                                                        */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Typedefs                                                                                     */
/*************************************************************************************************/

typedef struct DvzPipelib DvzPipelib;



/*************************************************************************************************/
/*  Structs                                                                                      */
/*************************************************************************************************/

struct DvzPipelib
{
    DvzObject obj;
    DvzGpu* gpu;
    DvzContainer graphics;
    DvzContainer computes;
};



EXTERN_C_ON

/*************************************************************************************************/
/*  Functions                                                                                    */
/*************************************************************************************************/

/**
 * Create a library of pipes (computes/graphics).
 *
 * @param gpu the GPU
 * @returns the pipelib
 */
DVZ_EXPORT DvzPipelib dvz_pipelib(DvzGpu* gpu);



/**
 * Create a new builtin graphics pipe.
 *
 * @param lib the pipelib instance
 * @param renderpass the renderpass
 * @param type the builtin graphics type
 * @param flags the graphics creation flags
 * @returns the pipe
 */
DVZ_EXPORT DvzPipe* dvz_pipelib_graphics(
    DvzPipelib* lib, DvzRenderpass* renderpass, uint32_t img_count, //
    uvec2 size, DvzGraphicsType type, int flags);



/**
 * Create a new compute from a compiled SPIRV .spv shader file.
 *
 * @param lib the pipelib instance
 * @param shader_path the path to the .spv shader file
 * @returns the pipe
 */
DVZ_EXPORT DvzPipe* dvz_pipelib_compute_file(DvzPipelib* lib, const char* shader_path);



/**
 * Destroy a pipe created by the pipelib.
 *
 * @param lib the pipelib instance
 * @param pipe the pipe to destroy
 */
DVZ_EXPORT void dvz_pipelib_pipe_destroy(DvzPipelib* lib, DvzPipe* pipe);



/**
 * Destroy the pipelib.
 *
 * @param lib the pipelib instance
 */
DVZ_EXPORT void dvz_pipelib_destroy(DvzPipelib* lib);



EXTERN_C_OFF

#endif
