/*************************************************************************************************/
/*  Renderer                                                                                     */
/*************************************************************************************************/

#include <map>

#include "_log.h"
#include "board.h"
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
    rd->router->router[std::make_pair(DVZ_REQUEST_ACTION_##action, DVZ_OBJECT_TYPE_##type)] =     \
        function;



/*************************************************************************************************/
/*  Typedefs                                                                                     */
/*************************************************************************************************/

typedef void* (*DvzRouterCallback)(DvzRenderer*, DvzRequest);



/*************************************************************************************************/
/*  Structs                                                                                      */
/*************************************************************************************************/

extern "C" struct DvzRouter
{
    std::map<std::pair<DvzRequestAction, DvzObjectType>, DvzRouterCallback> router;
};



/*************************************************************************************************/
/*  Board                                                                                        */
/*************************************************************************************************/

static void* _board_create(DvzRenderer* rd, DvzRequest req)
{
    ASSERT(rd != NULL);

    DvzBoard* board = dvz_workspace_board(
        rd->workspace, req.content.board.width, req.content.board.height, req.content.board.flags);
    board->rgba = dvz_board_alloc(board);
    return (void*)board;
}



static void* _board_delete(DvzRenderer* rd, DvzRequest req)
{
    ASSERT(rd != NULL);
    ASSERT(req.id != 0);

    DvzBoard* board = (DvzBoard*)dvz_map_get(rd->map, req.id);
    ASSERT(board != NULL);

    dvz_board_free(board);
    dvz_board_destroy(board);
    return NULL;
}



/*************************************************************************************************/
/*  Utils                                                                                        */
/*************************************************************************************************/

static void _setup_router(DvzRenderer* rd)
{
    ASSERT(rd != NULL);

    rd->router = new DvzRouter();
    rd->router->router = std::map<std::pair<DvzRequestAction, DvzObjectType>, DvzRouterCallback>();

    // Board.
    ROUTE(CREATE, BOARD, _board_create)
    ROUTE(DELETE, BOARD, _board_delete)
}



static DvzId _update_mapping(DvzRenderer* rd, DvzRequest req, void* obj)
{
    ASSERT(rd != NULL);

    // Handle the id-object mapping.
    DvzId id = 0;
    switch (req.action)
    {
        // Creation.
    case DVZ_REQUEST_ACTION_CREATE:
        ASSERT(obj != NULL);

        // Generate a new id.
        id = dvz_map_id(rd->map);
        log_trace("adding object type %d id %d to mapping", req.type, id);

        // Register the id with the created object
        dvz_map_add(rd->map, id, DVZ_OBJECT_TYPE_BOARD, obj);

        break;

        // Deletion.
    case DVZ_REQUEST_ACTION_DELETE:

        ASSERT(req.id != DVZ_ID_NONE);
        id = req.id;

        // Remove the id from the mapping.
        log_trace("removing object type %d id %d from mapping", req.type, req.id);
        dvz_map_remove(rd->map, req.id);

        break;

    default:
        break;
    }

    return id;
}



/*************************************************************************************************/
/*  Functions                                                                                    */
/*************************************************************************************************/

DvzRenderer* dvz_renderer_offscreen(DvzGpu* gpu)
{
    ASSERT(gpu != NULL);
    DvzRenderer* rd = (DvzRenderer*)calloc(1, sizeof(DvzRenderer));
    ASSERT(rd != NULL);
    rd->gpu = gpu;

    rd->ctx = dvz_context(gpu);
    rd->pipelib = dvz_pipelib(rd->ctx);
    rd->workspace = dvz_workspace(gpu);
    rd->map = dvz_map();

    _setup_router(rd);

    dvz_obj_init(&rd->obj);
    return rd;
}



DvzRenderer* dvz_renderer_glfw(DvzGpu* gpu)
{
    ASSERT(gpu != NULL);
    // TODO
    return NULL;
}



DvzId dvz_renderer_request(DvzRenderer* rd, DvzRequest req)
{
    ASSERT(rd != NULL);

    DvzRouterCallback cb = rd->router->router[std::make_pair(req.action, req.type)];
    if (cb == NULL)
    {
        log_error("no router function registered for action %d and type %d", req.action, req.type);
        return DVZ_ID_NONE;
    }
    log_trace("processing renderer request");

    // Call the router callback.
    void* obj = cb(rd, req);

    DvzId id = _update_mapping(rd, req, obj);

    // Return the id.
    return id;
}



void dvz_renderer_image(DvzRenderer* rd, DvzId canvas_id, DvzSize size, uint8_t* rgba)
{
    ASSERT(rd != NULL);
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