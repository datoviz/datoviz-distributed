/*************************************************************************************************/
/*  Renderer                                                                                     */
/*************************************************************************************************/

#include <map>

#include "_log.h"
#include "board.h"
#include "canvas.h"
#include "context.h"
#include "graphics.h"
#include "map.h"
#include "pipe.h"
#include "pipelib.h"
#include "renderer.h"
#include "workspace.h"



/*************************************************************************************************/
/*  Macros                                                                                       */
/*************************************************************************************************/

#define ROUTE(action, type, function)                                                             \
    rd->router->router[std::make_pair(DVZ_REQUEST_ACTION_##action, DVZ_REQUEST_OBJECT_##type)] =  \
        function;

#define SET_ID(x)                                                                                 \
    ASSERT(req.id != DVZ_ID_NONE);                                                                \
    (x)->obj.id = req.id;

#define GET_ID(t, n, i)                                                                           \
    t* n = (t*)dvz_map_get(rd->map, i);                                                           \
    if (n == NULL)                                                                                \
    {                                                                                             \
        log_error("%s %" PRIx64 "doesn't exist", #n, i);                                          \
        return NULL;                                                                              \
    }                                                                                             \
    ASSERT(n != NULL);



/*************************************************************************************************/
/*  Typedefs                                                                                     */
/*************************************************************************************************/

typedef void* (*DvzRouterCallback)(DvzRenderer*, DvzRequest);



/*************************************************************************************************/
/*  Structs                                                                                      */
/*************************************************************************************************/

extern "C" struct DvzRouter
{
    std::map<std::pair<DvzRequestAction, DvzRequestObject>, DvzRouterCallback> router;
};



/*************************************************************************************************/
/*  Board                                                                                        */
/*************************************************************************************************/

static void* _board_create(DvzRenderer* rd, DvzRequest req)
{
    ASSERT(rd != NULL);
    log_trace("create board");

    DvzBoard* board = dvz_workspace_board(
        rd->workspace, req.content.board.width, req.content.board.height, req.flags);
    ASSERT(board != NULL);
    SET_ID(board)
    board->rgb = dvz_board_alloc(board);
    return (void*)board;
}



static void* _board_update(DvzRenderer* rd, DvzRequest req)
{
    ASSERT(rd != NULL);
    ASSERT(req.id != 0);
    log_trace("update board");

    GET_ID(DvzBoard, board, req.id)

    dvz_cmd_submit_sync(&board->cmds, DVZ_DEFAULT_QUEUE_RENDER);

    return NULL;
}



static void* _board_delete(DvzRenderer* rd, DvzRequest req)
{
    ASSERT(rd != NULL);
    ASSERT(req.id != 0);
    log_trace("delete board");

    GET_ID(DvzBoard, board, req.id)

    dvz_board_free(board);
    dvz_board_destroy(board);
    return NULL;
}



/*************************************************************************************************/
/*  Canvas                                                                                       */
/*************************************************************************************************/

static void* _canvas_create(DvzRenderer* rd, DvzRequest req)
{
    ASSERT(rd != NULL);
    log_trace("create canvas");

    DvzCanvas* canvas = dvz_workspace_canvas(
        rd->workspace, req.content.canvas.width, req.content.canvas.height, req.flags);
    ASSERT(canvas != NULL);
    SET_ID(canvas)
    return (void*)canvas;
}



static void* _canvas_delete(DvzRenderer* rd, DvzRequest req)
{
    ASSERT(rd != NULL);
    ASSERT(req.id != 0);
    log_trace("delete canvas");

    GET_ID(DvzCanvas, canvas, req.id)

    ASSERT(canvas != NULL);
    dvz_canvas_destroy(canvas);
    return NULL;
}



/*************************************************************************************************/
/*  Graphics                                                                                     */
/*************************************************************************************************/

static void* _graphics_create(DvzRenderer* rd, DvzRequest req)
{
    ASSERT(rd != NULL);

    // Get the board.
    DvzId id = req.content.graphics.parent;
    ASSERT(id != DVZ_ID_NONE);

    DvzRequestObject type = (DvzRequestObject)dvz_map_type(rd->map, id);
    log_trace("create graphics for parent %" PRIx64 " with type %d", id, type);
    ASSERT(type == DVZ_REQUEST_OBJECT_BOARD || type == DVZ_REQUEST_OBJECT_CANVAS);

    DvzPipe* pipe = NULL;

    if (type == DVZ_REQUEST_OBJECT_BOARD)
    {
        DvzBoard* board = (DvzBoard*)dvz_map_get(rd->map, id);
        ASSERT(board != NULL);

        // Create the pipe.
        uvec2 size = {board->width, board->height};
        pipe = dvz_pipelib_graphics(
            rd->pipelib, rd->ctx, &board->renderpass, board->images.count, //
            size, req.content.graphics.type, req.flags);
        ASSERT(pipe != NULL);
        SET_ID(pipe)
    }

    else if (type == DVZ_REQUEST_OBJECT_CANVAS)
    {
        DvzCanvas* canvas = (DvzCanvas*)dvz_map_get(rd->map, id);
        ASSERT(canvas != NULL);

        // Get the canvas framebuffer size.
        uvec2 size = {0};
        dvz_canvas_size(canvas, DVZ_CANVAS_SIZE_FRAMEBUFFER, size);

        // Create the pipe.
        pipe = dvz_pipelib_graphics(
            rd->pipelib, rd->ctx, &canvas->render.renderpass, canvas->render.swapchain.img_count,
            size, req.content.graphics.type, req.flags);
        ASSERT(pipe != NULL);
        SET_ID(pipe)
    }
    ASSERT(pipe != NULL);

    return (void*)pipe;
}



static void* _graphics_vertex(DvzRenderer* rd, DvzRequest req)
{
    ASSERT(rd != NULL);
    ASSERT(req.id != 0);

    // Get the graphics pipe.
    GET_ID(DvzPipe, pipe, req.id)

    // Get the dat with the vertex data.
    GET_ID(DvzDat, dat, req.content.set_vertex.dat);

    // Link the two.
    pipe->dat_vertex = dat;

    return NULL;
}



/*************************************************************************************************/
/*  Computes                                                                                     */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Pipes                                                                                        */
/*************************************************************************************************/

static void* _pipe_dat(DvzRenderer* rd, DvzRequest req)
{
    ASSERT(rd != NULL);
    ASSERT(req.id != 0);

    // Get the graphics pipe.
    GET_ID(DvzPipe, pipe, req.id)

    // Get the dat data.
    GET_ID(DvzDat, dat, req.content.set_dat.dat);

    // Link the dat.
    // pipe->dats[req.content.set_dat.slot_idx] = dat;

    dvz_pipe_dat(pipe, req.content.set_dat.slot_idx, dat);
    if (dvz_pipe_complete(pipe))
        dvz_bindings_update(&pipe->bindings);

    return NULL;
}



static void* _pipe_tex(DvzRenderer* rd, DvzRequest req)
{
    ASSERT(rd != NULL);
    ASSERT(req.id != 0);

    // Get the graphics pipe.
    GET_ID(DvzPipe, pipe, req.id)

    // Get the tex.
    GET_ID(DvzTex, tex, req.content.set_tex.tex);

    // Get the sampler.
    GET_ID(DvzSampler, sampler, req.content.set_tex.sampler);
    ASSERT(tex != NULL);

    // Link the tex.
    // pipe->texs[req.content.set_binding.slot_idx] = tex;

    dvz_pipe_tex(pipe, req.content.set_tex.slot_idx, tex, sampler);
    if (dvz_pipe_complete(pipe))
        dvz_bindings_update(&pipe->bindings);

    return NULL;
}



static void* _pipe_delete(DvzRenderer* rd, DvzRequest req)
{
    ASSERT(rd != NULL);
    ASSERT(req.id != 0);
    log_trace("delete pipe");

    GET_ID(DvzPipe, pipe, req.id)

    dvz_pipe_destroy(pipe);
    return NULL;
}



/*************************************************************************************************/
/*  Resources                                                                                    */
/*************************************************************************************************/

static void* _dat_create(DvzRenderer* rd, DvzRequest req)
{
    ASSERT(rd != NULL);
    log_trace("create dat");

    DvzDat* dat = dvz_dat(rd->ctx, req.content.dat.type, req.content.dat.size, req.flags);
    ASSERT(dat != NULL);
    SET_ID(dat)

    return (void*)dat;
}



static void* _dat_upload(DvzRenderer* rd, DvzRequest req)
{
    ASSERT(rd != NULL);
    ASSERT(req.id != 0);

    GET_ID(DvzDat, dat, req.id)
    ASSERT(dat->br.buffer != NULL);

    log_trace(
        "uploading %s to dat (buffer type %d region offset %d)",
        pretty_size(req.content.dat_upload.size), dat->br.buffer->type, dat->br.offsets[0]);

    dvz_dat_upload(
        dat,                           //
        req.content.dat_upload.offset, //
        req.content.dat_upload.size,   //
        req.content.dat_upload.data,   //
        true);                         // TODO: do not wait? try false

    return NULL;
}



static void* _dat_delete(DvzRenderer* rd, DvzRequest req)
{
    ASSERT(rd != NULL);
    ASSERT(req.id != 0);
    log_trace("delete dat");

    GET_ID(DvzDat, dat, req.id)

    dvz_dat_destroy(dat);
    return NULL;
}



static void* _sampler_create(DvzRenderer* rd, DvzRequest req)
{

    ASSERT(rd != NULL);
    log_trace("create sampler");

    DvzSampler* sampler =
        dvz_resources_sampler(&rd->ctx->res, req.content.sampler.filter, req.content.sampler.mode);
    ASSERT(sampler != NULL);
    SET_ID(sampler)

    return (void*)sampler;
}



static void* _sampler_delete(DvzRenderer* rd, DvzRequest req)
{
    ASSERT(rd != NULL);
    ASSERT(req.id != 0);
    log_trace("delete sampler");

    GET_ID(DvzSampler, sampler, req.id)

    dvz_sampler_destroy(sampler);
    return NULL;
}



/*************************************************************************************************/
/*  Command buffer setting                                                                       */
/*************************************************************************************************/

static void* _set_begin(DvzRenderer* rd, DvzRequest req)
{
    ASSERT(rd != NULL);

    GET_ID(DvzBoard, board, req.id)

    dvz_cmd_reset(&board->cmds, 0);
    dvz_board_begin(board, &board->cmds, 0);

    return NULL;
}



static void* _set_viewport(DvzRenderer* rd, DvzRequest req)
{
    ASSERT(rd != NULL);

    GET_ID(DvzBoard, board, req.id)

    dvz_board_viewport(
        board, &board->cmds, 0, //
        req.content.set_viewport.offset, req.content.set_viewport.shape);

    return NULL;
}



static void* _set_draw(DvzRenderer* rd, DvzRequest req)
{
    ASSERT(rd != NULL);

    GET_ID(DvzBoard, board, req.id)

    GET_ID(DvzPipe, pipe, req.content.set_draw.graphics);

    dvz_pipe_draw(
        pipe, &board->cmds, 0, //
        req.content.set_draw.first_vertex, req.content.set_draw.vertex_count);

    return NULL;
}



static void* _set_end(DvzRenderer* rd, DvzRequest req)
{
    ASSERT(rd != NULL);

    GET_ID(DvzBoard, board, req.id)

    dvz_board_end(board, &board->cmds, 0);

    return NULL;
}



/*************************************************************************************************/
/*  Utils                                                                                        */
/*************************************************************************************************/

static void _init_renderer(DvzRenderer* rd)
{
    ASSERT(rd != NULL);
    ASSERT(rd->gpu != NULL);
    rd->ctx = dvz_context(rd->gpu);
    rd->pipelib = dvz_pipelib(rd->ctx);
    rd->workspace = dvz_workspace(rd->gpu);
    rd->map = dvz_map();

    dvz_obj_init(&rd->obj);
}



static void _setup_router(DvzRenderer* rd)
{
    ASSERT(rd != NULL);

    rd->router = new DvzRouter();
    rd->router->router =
        std::map<std::pair<DvzRequestAction, DvzRequestObject>, DvzRouterCallback>();

    // Board.
    ROUTE(CREATE, BOARD, _board_create)
    ROUTE(UPDATE, BOARD, _board_update)
    ROUTE(DELETE, BOARD, _board_delete)

    // Canvas.
    ROUTE(CREATE, CANVAS, _canvas_create)
    ROUTE(DELETE, CANVAS, _canvas_delete)

    // Graphics.
    ROUTE(CREATE, GRAPHICS, _graphics_create)
    ROUTE(SET, VERTEX, _graphics_vertex)
    ROUTE(BIND, DAT, _pipe_dat)
    ROUTE(BIND, TEX, _pipe_tex)
    ROUTE(DELETE, GRAPHICS, _pipe_delete)

    // TODO: computes.

    // Resources.
    ROUTE(CREATE, DAT, _dat_create)
    ROUTE(UPLOAD, DAT, _dat_upload)
    ROUTE(DELETE, DAT, _dat_delete)

    ROUTE(CREATE, SAMPLER, _sampler_create)
    ROUTE(DELETE, SAMPLER, _sampler_delete)

    // Command buffer setting.
    ROUTE(SET, BEGIN, _set_begin)
    ROUTE(SET, VIEWPORT, _set_viewport)
    ROUTE(SET, DRAW, _set_draw)
    ROUTE(SET, END, _set_end)
}



static void _update_mapping(DvzRenderer* rd, DvzRequest req, void* obj)
{
    ASSERT(rd != NULL);

    // Handle the id-object mapping.
    switch (req.action)
    {
        // Creation.
    case DVZ_REQUEST_ACTION_CREATE:
        ASSERT(obj != NULL);
        ASSERT(req.id != DVZ_ID_NONE);

        log_trace("adding object type %d id %" PRIx64 " to mapping", req.type, req.id);

        if (dvz_map_get(rd->map, req.id) != NULL)
        {
            log_error("error while creating the object, this ID already exists");
            break;
        }

        // Register the id with the created object
        dvz_map_add(rd->map, req.id, req.type, obj);

        break;

        // Deletion.
    case DVZ_REQUEST_ACTION_DELETE:

        ASSERT(req.id != DVZ_ID_NONE);

        log_trace("removing object type %d id %" PRIx64 " from mapping", req.type, req.id);

        if (dvz_map_get(rd->map, req.id) == NULL)
        {
            log_error("error while deleting this object, this ID doesn't exist");
            break;
        }

        // Remove the id from the mapping.
        dvz_map_remove(rd->map, req.id);

        break;

    default:
        break;
    }
}



/*************************************************************************************************/
/*  Functions                                                                                    */
/*************************************************************************************************/

DvzRenderer* dvz_renderer(DvzGpu* gpu)
{
    ASSERT(gpu != NULL);
    DvzRenderer* rd = (DvzRenderer*)calloc(1, sizeof(DvzRenderer));
    ASSERT(rd != NULL);
    rd->gpu = gpu;
    _init_renderer(rd);
    _setup_router(rd);
    return rd;
}



void dvz_renderer_request(DvzRenderer* rd, DvzRequest req)
{
    ASSERT(rd != NULL);

    DvzRouterCallback cb = rd->router->router[std::make_pair(req.action, req.type)];
    if (cb == NULL)
    {
        log_error("no router function registered for action %d and type %d", req.action, req.type);
        return;
    }
    log_trace("processing renderer request action %d and type %d", req.action, req.type);

    // Call the router callback.
    void* obj = cb(rd, req);

    // Register the pointer in the map table, associated with its id.
    _update_mapping(rd, req, obj);
}



void dvz_renderer_requests(DvzRenderer* rd, uint32_t count, DvzRequest* reqs)
{
    ASSERT(rd != NULL);
    if (count == 0)
        return;
    ASSERT(count > 0);
    ASSERT(reqs != NULL);
    for (uint32_t i = 0; i < count; i++)
    {
        dvz_renderer_request(rd, reqs[i]);
    }
}



DvzBoard* dvz_renderer_board(DvzRenderer* rd, DvzId id)
{
    ASSERT(rd != NULL);

    DvzBoard* board = (DvzBoard*)dvz_map_get(rd->map, id);
    ASSERT(board != NULL);
    return board;
}



DvzCanvas* dvz_renderer_canvas(DvzRenderer* rd, DvzId id)
{
    ASSERT(rd != NULL);

    DvzCanvas* canvas = (DvzCanvas*)dvz_map_get(rd->map, id);
    ASSERT(canvas != NULL);
    return canvas;
}



uint8_t* dvz_renderer_image(DvzRenderer* rd, DvzId board_id, DvzSize* size, uint8_t* rgb)
{
    ASSERT(rd != NULL);

    DvzBoard* board = (DvzBoard*)dvz_map_get(rd->map, board_id);
    ASSERT(board != NULL);

    // Find the pointer: either passed here, or the board-owned pointer.
    rgb = rgb != NULL ? rgb : board->rgb;
    ASSERT(rgb != NULL);

    // Download the image to the buffer.
    dvz_board_download(board, board->size, rgb);

    // Set the size.
    ASSERT(size != NULL);
    *size = board->size;

    // Return the pointer.
    return rgb;
}



void dvz_renderer_destroy(DvzRenderer* rd)
{
    ASSERT(rd != NULL);

    dvz_map_destroy(rd->map);
    dvz_workspace_destroy(rd->workspace);
    dvz_pipelib_destroy(rd->pipelib);
    dvz_context_destroy(rd->ctx);

    dvz_obj_destroyed(&rd->obj);
    FREE(rd);
}
