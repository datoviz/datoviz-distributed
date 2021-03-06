/*************************************************************************************************/
/*  Request                                                                                      */
/*************************************************************************************************/

#include "request.h"



/*************************************************************************************************/
/*  Macros                                                                                       */
/*************************************************************************************************/

#define CREATE_REQUEST(_action, _type)                                                            \
    ASSERT(rqr != NULL);                                                                          \
    DvzRequest req = _request();                                                                  \
    req.action = DVZ_REQUEST_ACTION_##_action;                                                    \
    req.type = DVZ_REQUEST_OBJECT_##_type;



/*************************************************************************************************/
/*  Functions                                                                                    */
/*************************************************************************************************/

static DvzRequest _request(void)
{
    DvzRequest req = {0};
    req.version = DVZ_REQUEST_VERSION;
    return req;
}



DvzRequester dvz_requester(void)
{
    DvzRequester rqr = {0};
    rqr.prng = dvz_prng();

    // Initialize the list of requests for batchs.
    rqr.capacity = DVZ_CONTAINER_DEFAULT_COUNT;
    rqr.requests = (DvzRequest*)calloc(rqr.capacity, sizeof(DvzRequest));

    dvz_obj_init(&rqr.obj);
    return rqr;
}



void dvz_requester_destroy(DvzRequester* rqr)
{
    ASSERT(rqr != NULL);
    FREE(rqr->requests);
    dvz_prng_destroy(rqr->prng);
    dvz_obj_destroyed(&rqr->obj);
}



/*************************************************************************************************/
/*  Request batch                                                                                */
/*************************************************************************************************/

void dvz_requester_begin(DvzRequester* rqr)
{
    ASSERT(rqr != NULL);
    rqr->count = 0;
}



void dvz_requester_add(DvzRequester* rqr, DvzRequest req)
{
    ASSERT(rqr != NULL);
    // Resize the array if needed.
    if (rqr->count == rqr->capacity)
    {
        rqr->capacity *= 2;
        REALLOC(rqr->requests, rqr->capacity * sizeof(DvzRequest));
    }
    ASSERT(rqr->count < rqr->capacity);

    // Append the request.
    rqr->requests[rqr->count++] = req;
}



DvzRequest* dvz_requester_end(DvzRequester* rqr, uint32_t* count)
{
    ASSERT(rqr != NULL);
    if (count != NULL)
        *count = rqr->count;
    return rqr->requests;
}



DvzRequest* dvz_requester_flush(DvzRequester* rqr, uint32_t* count)
{
    ASSERT(rqr != NULL);
    ASSERT(count != NULL);
    uint32_t n = rqr->count;

    // Modify the count pointer to the number of returned requests.
    *count = n;

    // Make a copy of the pending requests.
    DvzRequest* requests = calloc(n, sizeof(DvzRequest));
    memcpy(requests, rqr->requests, n * sizeof(DvzRequest));

    // Flush the requests.
    rqr->count = 0;

    return requests;
}



void dvz_request_print(DvzRequest* req)
{
    ASSERT(req != NULL);
    log_info("Request action %d <type %d> <id %" PRIx64 ">", req->action, req->type, req->id);
}



/*************************************************************************************************/
/*  Board                                                                                        */
/*************************************************************************************************/

DvzRequest
dvz_create_board(DvzRequester* rqr, uint32_t width, uint32_t height, cvec4 background, int flags)
{
    CREATE_REQUEST(CREATE, BOARD);
    req.id = dvz_prng_uuid(rqr->prng);
    req.flags = flags;
    req.content.board.width = width;
    req.content.board.height = height;
    memcpy(req.content.board.background, background, sizeof(cvec4));
    return req;
}



DvzRequest dvz_update_board(DvzRequester* rqr, DvzId id)
{
    CREATE_REQUEST(UPDATE, BOARD);
    req.id = id;
    return req;
}



DvzRequest dvz_resize_board(DvzRequester* rqr, DvzId board, uint32_t width, uint32_t height)
{
    CREATE_REQUEST(RESIZE, BOARD);
    req.id = board;
    req.content.board.width = width;
    req.content.board.height = height;
    return req;
}



DvzRequest dvz_set_background(DvzRequester* rqr, DvzId id, cvec4 color)
{
    CREATE_REQUEST(SET, BACKGROUND);
    req.id = id;
    memcpy(req.content.board.background, color, sizeof(cvec4));
    return req;
}



DvzRequest dvz_delete_board(DvzRequester* rqr, DvzId id)
{
    CREATE_REQUEST(DELETE, BOARD);
    req.id = id;
    return req;
}



/*************************************************************************************************/
/*  Canvas                                                                                       */
/*************************************************************************************************/

DvzRequest
dvz_create_canvas(DvzRequester* rqr, uint32_t width, uint32_t height, cvec4 background, int flags)
{
    CREATE_REQUEST(CREATE, CANVAS);
    req.id = dvz_prng_uuid(rqr->prng);
    req.flags = flags;
    req.content.canvas.width = width;
    req.content.canvas.height = height;
    memcpy(req.content.canvas.background, background, sizeof(cvec4));
    return req;
}



DvzRequest dvz_delete_canvas(DvzRequester* rqr, DvzId id)
{
    CREATE_REQUEST(DELETE, CANVAS);
    req.id = id;
    return req;
}



/*************************************************************************************************/
/*  Dat                                                                                          */
/*************************************************************************************************/

DvzRequest dvz_create_dat(DvzRequester* rqr, DvzBufferType type, DvzSize size, int flags)
{
    CREATE_REQUEST(CREATE, DAT);
    req.id = dvz_prng_uuid(rqr->prng);
    req.flags = flags;
    req.content.dat.type = type;
    req.content.dat.size = size;
    return req;
}



DvzRequest dvz_resize_dat(DvzRequester* rqr, DvzId dat, DvzSize size)
{
    CREATE_REQUEST(RESIZE, DAT);
    req.id = dat;
    req.content.dat.size = size;
    return req;
}



DvzRequest dvz_upload_dat(DvzRequester* rqr, DvzId dat, DvzSize offset, DvzSize size, void* data)
{
    CREATE_REQUEST(UPLOAD, DAT);
    req.id = dat;
    req.content.dat_upload.offset = offset;
    req.content.dat_upload.size = size;
    req.content.dat_upload.data = data;
    return req;
}



/*************************************************************************************************/
/*  Tex                                                                                          */
/*************************************************************************************************/

DvzRequest
dvz_create_tex(DvzRequester* rqr, DvzTexDims dims, DvzFormat format, uvec3 shape, int flags)
{
    CREATE_REQUEST(CREATE, TEX);
    req.id = dvz_prng_uuid(rqr->prng);
    req.flags = flags;
    req.content.tex.dims = dims;
    memcpy(req.content.tex.shape, shape, sizeof(uvec3));
    req.content.tex.format = format;
    return req;
}



DvzRequest dvz_resize_tex(DvzRequester* rqr, DvzId tex, uvec3 shape)
{
    CREATE_REQUEST(RESIZE, TEX);
    req.id = tex;
    memcpy(req.content.tex.shape, shape, sizeof(uvec3));
    return req;
}



DvzRequest
dvz_upload_tex(DvzRequester* rqr, DvzId tex, uvec3 offset, uvec3 shape, DvzSize size, void* data)
{
    CREATE_REQUEST(UPLOAD, TEX);
    req.id = tex;

    memcpy(req.content.tex_upload.offset, offset, sizeof(uvec3));
    memcpy(req.content.tex_upload.shape, shape, sizeof(uvec3));
    req.content.tex_upload.size = size;
    req.content.tex_upload.data = data;
    return req;
}



/*************************************************************************************************/
/*  Sampler                                                                                      */
/*************************************************************************************************/

DvzRequest dvz_create_sampler(DvzRequester* rqr, DvzFilter filter, DvzSamplerAddressMode mode)
{
    CREATE_REQUEST(CREATE, SAMPLER);
    req.id = dvz_prng_uuid(rqr->prng);
    req.content.sampler.filter = filter;
    req.content.sampler.mode = mode;
    return req;
}



/*************************************************************************************************/
/*  Graphics                                                                                     */
/*************************************************************************************************/

DvzRequest dvz_create_graphics(DvzRequester* rqr, DvzId parent, DvzGraphicsType type, int flags)
{
    CREATE_REQUEST(CREATE, GRAPHICS);
    req.id = dvz_prng_uuid(rqr->prng);
    req.flags = flags;
    req.content.graphics.parent = parent;
    req.content.graphics.type = type;
    return req;
}



DvzRequest dvz_set_vertex(DvzRequester* rqr, DvzId graphics, DvzId dat)
{
    CREATE_REQUEST(SET, VERTEX);
    req.id = graphics;
    req.content.set_vertex.dat = dat;
    return req;
}



/*************************************************************************************************/
/*  Bindings                                                                                     */
/*************************************************************************************************/

DvzRequest dvz_bind_dat(DvzRequester* rqr, DvzId pipe, uint32_t slot_idx, DvzId dat)
{
    CREATE_REQUEST(BIND, DAT);
    req.id = pipe;
    req.content.set_dat.slot_idx = slot_idx;
    req.content.set_dat.dat = dat;
    return req;
}



DvzRequest dvz_bind_tex(DvzRequester* rqr, DvzId pipe, uint32_t slot_idx, DvzId tex, DvzId sampler)
{
    CREATE_REQUEST(BIND, TEX);
    req.id = pipe;
    req.content.set_tex.slot_idx = slot_idx;
    req.content.set_tex.tex = tex;
    req.content.set_tex.sampler = sampler;
    return req;
}



/*************************************************************************************************/
/*  Command buffer                                                                               */
/*************************************************************************************************/

DvzRequest dvz_record_begin(DvzRequester* rqr, DvzId board)
{
    CREATE_REQUEST(RECORD, RECORD);
    req.id = board;
    req.content.record.command.type = DVZ_RECORDER_BEGIN;
    return req;
}



DvzRequest dvz_record_viewport(DvzRequester* rqr, DvzId board, vec2 offset, vec2 shape)
{
    CREATE_REQUEST(RECORD, RECORD);
    req.id = board;
    req.content.record.command.type = DVZ_RECORDER_VIEWPORT;
    memcpy(req.content.record.command.contents.v.offset, offset, sizeof(vec2));
    memcpy(req.content.record.command.contents.v.shape, shape, sizeof(vec2));
    return req;
}



DvzRequest dvz_record_draw(
    DvzRequester* rqr, DvzId board, DvzId graphics, uint32_t first_vertex, uint32_t vertex_count)
{
    CREATE_REQUEST(RECORD, RECORD);
    req.id = board;
    req.content.record.command.type = DVZ_RECORDER_DRAW_DIRECT;
    req.content.record.command.contents.draw_direct.pipe_id = graphics;
    req.content.record.command.contents.draw_direct.first_vertex = first_vertex;
    req.content.record.command.contents.draw_direct.vertex_count = vertex_count;
    return req;
}



DvzRequest dvz_record_end(DvzRequester* rqr, DvzId board)
{
    CREATE_REQUEST(RECORD, RECORD);
    req.id = board;
    req.content.record.command.type = DVZ_RECORDER_END;
    return req;
}
