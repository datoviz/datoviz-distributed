/*************************************************************************************************/
/*  Recorder                                                                                     */
/*************************************************************************************************/

#ifndef DVZ_HEADER_RECORDER
#define DVZ_HEADER_RECORDER



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include "_enums.h"
#include "_log.h"
#include "_macros.h"
#include "_math.h"
#include "_obj.h"



/*************************************************************************************************/
/*  Constants                                                                                    */
/*************************************************************************************************/

#define DVZ_RECORDER_COMMAND_COUNT 16
#define DVZ_MAX_SWAPCHAIN_IMAGES   4



/*************************************************************************************************/
/*  Enums                                                                                        */
/*************************************************************************************************/

typedef enum
{
    DVZ_RECORDER_NONE,
    DVZ_RECORDER_BEGIN,
    DVZ_RECORDER_DRAW_DIRECT,
    DVZ_RECORDER_DRAW_DIRECT_INDEXED,
    DVZ_RECORDER_DRAW_INDIRECT,
    DVZ_RECORDER_DRAW_INDIRECT_INDEXED,
    DVZ_RECORDER_VIEWPORT,
    DVZ_RECORDER_END,
} DvzRecorderCommandType;



/*************************************************************************************************/
/*  Typedefs                                                                                     */
/*************************************************************************************************/

typedef struct DvzRecorderCommand DvzRecorderCommand;
typedef struct DvzRecorder DvzRecorder;

// Forward declarations.
typedef uint64_t DvzId;
typedef struct DvzCanvas DvzCanvas;
typedef struct DvzPipe DvzPipe;
typedef struct DvzCommands DvzCommands;
typedef struct DvzRenderer DvzRenderer;



/*************************************************************************************************/
/*  Structs                                                                                      */
/*************************************************************************************************/

struct DvzRecorderCommand
{
    DvzRecorderCommandType type;
    DvzId canvas_id;
    union
    {
        // // Begin recording for canvas.
        // struct
        // {
        //     DvzCanvas* canvas;
        // } c;

        // Viewport.
        struct
        {
            vec2 offset, shape; // in framebuffer pixels
        } v;

        struct
        {
            DvzId pipe_id;
            uint32_t first_vertex, vertex_count;
        } draw_direct;

        struct
        {
            DvzId pipe_id;
            uint32_t first_index, vertex_offset, index_count;
        } draw_direct_indexed;

        struct
        {
            DvzId pipe_id;
            DvzId dat_indirect_id;
        } draw_indirect;
    } contents;
};



struct DvzRecorder
{
    uint32_t img_count;
    uint32_t capacity;
    uint32_t count; // number of commands
    DvzRecorderCommand* commands;
    bool dirty[DVZ_MAX_SWAPCHAIN_IMAGES]; // all true initially
};



EXTERN_C_ON

/*************************************************************************************************/
/*  Recorder functions                                                                           */
/*************************************************************************************************/

DVZ_EXPORT DvzRecorder* dvz_recorder(uint32_t img_count);

DVZ_EXPORT void dvz_recorder_clear(DvzRecorder* recorder);

DVZ_EXPORT void dvz_recorder_append(DvzRecorder* recorder, DvzRecorderCommand rc);

DVZ_EXPORT uint32_t dvz_recorder_size(DvzRecorder* recorder);

DVZ_EXPORT void
dvz_recorder_set(DvzRecorder* recorder, DvzRenderer* rnd, DvzCommands* cmds, uint32_t img_idx);

DVZ_EXPORT bool dvz_recorder_is_dirty(DvzRecorder* recorder, uint32_t img_idx);

DVZ_EXPORT void dvz_recorder_need_refill(DvzRecorder* recorder);

DVZ_EXPORT void dvz_recorder_destroy(DvzRecorder* recorder);



EXTERN_C_OFF

#endif
