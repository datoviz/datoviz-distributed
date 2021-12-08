/*************************************************************************************************/
/*  Renderer                                                                                     */
/*************************************************************************************************/

#ifndef DVZ_HEADER_RENDERER
#define DVZ_HEADER_RENDERER



/*************************************************************************************************/
/*  Includes                                                                                    */
/*************************************************************************************************/

#include "common.h"
#include "context.h"
#include "map.h"
#include "pipe.h"
#include "pipelib.h"
#include "request.h"
#include "workspace.h"



/*************************************************************************************************/
/*  Constants                                                                                    */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Enums                                                                                        */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Typedefs                                                                                     */
/*************************************************************************************************/

typedef struct DvzRenderer DvzRenderer;

// Forward declarations.



/*************************************************************************************************/
/*  Structs                                                                                      */
/*************************************************************************************************/

struct DvzRenderer
{
    DvzObject obj;
    DvzGpu* gpu;
    DvzContext ctx;     // data: the "what"
    DvzPipelib lib;     // GLSL programs: the "how"
    DvzWorkspace space; // boards and canvases: the "where"
    DvzMap* map;        // mapping between uuid and <type, objects>
};



EXTERN_C_ON

/*************************************************************************************************/
/*  Functions                                                                                    */
/*************************************************************************************************/

// TODO: docstrings

DVZ_EXPORT DvzRenderer dvz_renderer_offscreen(DvzGpu* gpu);



DVZ_EXPORT DvzRenderer dvz_renderer_glfw(DvzGpu* gpu);



DVZ_EXPORT DvzId dvz_renderer_request(DvzRenderer* rd, DvzRequest req);



DVZ_EXPORT void dvz_renderer_image(DvzRenderer* rd, DvzId canvas_id, DvzSize size, uint8_t* rgba);



DVZ_EXPORT void dvz_renderer_destroy(DvzRenderer* rd);



EXTERN_C_OFF

#endif
