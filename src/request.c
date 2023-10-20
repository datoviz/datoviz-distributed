/*************************************************************************************************/
/*  Request                                                                                      */
/*************************************************************************************************/

#include "request.h"
#include "_debug.h"
#include "_list.h"
#include "_pointer.h"
#include "fifo.h"
#include "fileio.h"



/*************************************************************************************************/
/*  Macros                                                                                       */
/*************************************************************************************************/

#define CREATE_REQUEST(_action, _type)                                                            \
    ANN(batch);                                                                                   \
    DvzRequest req = _request();                                                                  \
    req.action = DVZ_REQUEST_ACTION_##_action;                                                    \
    req.type = DVZ_REQUEST_OBJECT_##_type;

#define RETURN_REQUEST                                                                            \
    dvz_batch_add(batch, req);                                                                    \
    return req;

#define IF_REQ(_action, _type)                                                                    \
    if ((req->action == DVZ_REQUEST_ACTION_##_action) && (req->type == DVZ_REQUEST_OBJECT_##_type))

#define STR_ACTION(r)                                                                             \
    case DVZ_REQUEST_ACTION_##r:                                                                  \
        str = #r;                                                                                 \
        break

#define STR_OBJECT(r)                                                                             \
    case DVZ_REQUEST_OBJECT_##r:                                                                  \
        str = #r;                                                                                 \
        break

// Maximum size of buffers encoded in base64 when printing the commands
#define VERBOSE_MAX_BASE64 1048576

#define IF_VERBOSE_DATA                                                                           \
    if (getenv("DVZ_VERBOSE") && (strncmp(getenv("DVZ_VERBOSE"), "req", 3) == 0) &&               \
        (size < VERBOSE_MAX_BASE64))



/*************************************************************************************************/
/*  Util functions                                                                               */
/*************************************************************************************************/

// Global PRNG for all requests.
static DvzPrng* PRNG;



static DvzRequest _request(void)
{
    DvzRequest req = {0};
    req.version = DVZ_REQUEST_VERSION;
    return req;
}



// NOTE: the returned pointer will have to be freed.
static uint32_t* _cpy_uint32(DvzSize size, const void* data)
{
    ASSERT(size % 4 == 0);
    uint32_t* data_cpy = (uint32_t*)calloc(size / 4, sizeof(uint32_t));
    memcpy(data_cpy, data, size);
    return data_cpy;
}



static int write_file(const char* filename, DvzSize block_size, uint32_t block_count, void* data)
{
    ANN(filename);
    ASSERT(block_size > 0);
    ASSERT(block_count > 0);
    ANN(data);

    log_trace("saving binary `%s`", filename);
    FILE* fp = fopen(filename, "wb");
    if (fp == NULL)
    {
        log_error("error writing `%s`", filename);
        return 1;
    }
    fwrite(data, block_size, block_count, fp);
    fclose(fp);
    return 0;
}


static char* show_data(const unsigned char* src, size_t len)
{
    if (len > 1024)
    {
        return b64_encode(src, len);
    }
    else
    {
        char* buf = (char*)calloc(3 * len + 1, sizeof(char));
        int index = 0;
        for (uint32_t i = 0; i < len; i++)
        {
            // printf("%hhu ", src[i]);
            index += sprintf(&buf[index], "%02X ", src[i]);
        }
        return buf;
    }
}



/*************************************************************************************************/
/*  Print functions                                                                              */
/*************************************************************************************************/

static void _print_start(void)
{
    log_trace("print_start");
    printf("---\n"
           "version: '1.0'\n"
           "requests:\n");
}



static void _print_create_board(DvzRequest* req)
{
    log_trace("print_create_board");
    ANN(req);
    printf(
        "- action: create\n"
        "  type: board\n"
        "  id: 0x%" PRIx64 "\n"
        "  flags: %d\n"
        "  content:\n"
        "    width: %d\n"
        "    height: %d\n",
        req->id, req->flags, req->content.board.width, req->content.board.height);
}

static void _print_update_board(DvzRequest* req)
{
    log_trace("print_update_board");
    ANN(req);
    printf(
        "- action: update\n"
        "  type: board\n"
        "  id: 0x%" PRIx64 "\n",
        req->id);
}

static void _print_resize_board(DvzRequest* req)
{
    log_trace("print_resize_board");
    ANN(req);
    printf(
        "- action: resize\n"
        "  type: board\n"
        "  id: 0x%" PRIx64 "\n"
        "  content:\n"
        "    width: %d\n"
        "    height: %d\n",
        req->id, req->content.board.width, req->content.board.height);
}

static void _print_set_background(DvzRequest* req)
{
    log_trace("print_set_background");
    ANN(req);
    printf(
        "- action: set\n"
        "  type: background\n"
        "  id: 0x%" PRIx64 "\n"
        "  content:\n"
        "    background: [%d, %d, %d, %d]\n",
        req->id,                          //
        req->content.board.background[0], //
        req->content.board.background[1], //
        req->content.board.background[2], //
        req->content.board.background[3]);
}

static void _print_delete_board(DvzRequest* req)
{
    log_trace("print_delete_board");
    ANN(req);
    printf(
        "- action: delete\n"
        "  type: board\n"
        "  id: 0x%" PRIx64 "\n",
        req->id);
}



static void _print_create_canvas(DvzRequest* req)
{
    log_trace("print_create_canvas");
    ANN(req);
    printf(
        "- action: create\n"
        "  type: canvas\n"
        "  id: 0x%" PRIx64 "\n"
        "  flags: %d\n"
        "  content:\n"
        "    framebuffer_width: %d\n"
        "    framebuffer_height: %d\n"
        "    screen_width: %d\n"
        "    screen_height: %d\n",
        req->id, req->flags, //
        req->content.canvas.framebuffer_width, req->content.canvas.framebuffer_height,
        req->content.canvas.screen_width, req->content.canvas.screen_height);
}

static void _print_delete_canvas(DvzRequest* req)
{
    log_trace("print_delete_canvas");
    ANN(req);
    printf(
        "- action: delete\n"
        "  type: canvas\n"
        "  id: 0x%" PRIx64 "\n",
        req->id);
}



static void _print_create_dat(DvzRequest* req)
{
    log_trace("print_create_dat");
    ANN(req);
    printf(
        "- action: create\n"
        "  type: dat\n"
        "  id: 0x%" PRIx64 "\n"
        "  flags: %d\n"
        "  content:\n"
        "    type: %d\n"
        "    size: %" PRId64 "\n",
        req->id, req->flags, req->content.dat.type, req->content.dat.size);
}

static void _print_resize_dat(DvzRequest* req)
{
    log_trace("print_resize_dat");
    ANN(req);
    printf(
        "- action: resize\n"
        "  type: dat\n"
        "  id: 0x%" PRIx64 "\n"
        "  content:\n"
        "    size: %" PRId64 "\n",
        req->id, req->content.dat.size);
}

static void _print_upload_dat(DvzRequest* req)
{
    log_trace("print_upload_dat");
    ANN(req);

    DvzId dat = req->id;
    DvzSize size = req->content.dat_upload.size;
    DvzSize offset = req->content.dat_upload.offset;
    void* data = req->content.dat_upload.data;

    char* encoded = NULL;
    // NOTE: avoid computing the base64 of large arrays.

    IF_VERBOSE_DATA
    encoded = show_data((const unsigned char*)data, size);

    else encoded = "<snip>";
    printf(
        "- action: upload\n"
        "  type: dat\n"
        "  id: 0x%" PRIx64 "\n"
        "  content:\n"
        "    offset: %" PRId64 "\n"
        "    size: %" PRId64 "\n"
        "    data:\n"
        "      mode: base64\n"
        "      buffer: %s\n",
        dat, offset, size, encoded);

    IF_VERBOSE_DATA
    free(encoded);
}



static void _print_create_tex(DvzRequest* req)
{
    log_trace("print_create_tex");
    ANN(req);
    printf(
        "- action: create\n"
        "  type: tex\n"
        "  id: 0x%" PRIx64 "\n"
        "  flags: %d\n"
        "  content:\n"
        "    format: %d\n"
        "    dims: %d\n"
        "    size: [%d, %d, %d]\n",
        req->id, req->flags, req->content.tex.format, req->content.tex.dims,
        req->content.tex.shape[0], req->content.tex.shape[1], req->content.tex.shape[2]);
}

static void _print_resize_tex(DvzRequest* req)
{
    log_trace("print_resize_tex");
    ANN(req);
    printf(
        "- action: resize\n"
        "  type: tex\n"
        "  id: 0x%" PRIx64 "\n"
        "  content:\n"
        "    size: [%d, %d, %d]\n",
        req->id, req->content.tex.shape[0], req->content.tex.shape[1], req->content.tex.shape[2]);
}

static void _print_upload_tex(DvzRequest* req)
{
    log_trace("print_upload_tex");
    ANN(req);

    DvzId tex = req->id;
    DvzSize size = req->content.tex_upload.size;

    uint32_t* offset = req->content.tex_upload.offset;
    uint32_t* shape = req->content.tex_upload.shape;

    void* data = req->content.tex_upload.data;

    char* encoded = NULL;
    // NOTE: avoid computing the base64 of large arrays.
    IF_VERBOSE_DATA
    encoded = show_data((const unsigned char*)data, size);

    else encoded = "<snip>";
    printf(
        "- action: upload\n"
        "  type: tex\n"
        "  id: 0x%" PRIx64 "\n"
        "  content:\n"
        "    size: %" PRId64 "\n"
        "    offset: [%d, %d, %d]\n"
        "    shape: [%d, %d, %d]\n"
        "    data:\n"
        "      mode: base64\n"
        "      buffer: %s\n",
        tex, size, offset[0], offset[1], offset[2], shape[0], shape[1], shape[2], encoded);

    IF_VERBOSE_DATA
    free(encoded);
}



static void _print_create_sampler(DvzRequest* req)
{
    log_trace("print_create_sampler");
    ANN(req);
    printf(
        "- action: create\n"
        "  type: sampler\n"
        "  id: 0x%" PRIx64 "\n"
        "  flags: %d\n"
        "  content:\n"
        "    filter: %d\n"
        "    mode: %d\n",
        req->id, req->flags, //
        req->content.sampler.filter, req->content.sampler.mode);
}



static void _print_create_shader(DvzRequest* req)
{
    log_trace("print_create_shader");
    ANN(req);

    DvzShaderFormat format = req->content.shader.format;
    DvzShaderType shader_type = req->content.shader.type;
    DvzSize size = req->content.shader.size;

    void* code_buffer =
        (format == DVZ_SHADER_SPIRV ? (void*)req->content.shader.buffer
                                    : (void*)req->content.shader.code);
    ANN(code_buffer);

    char* encoded = NULL;
    // NOTE: avoid computing the base64 of large arrays.

    IF_VERBOSE_DATA
    encoded = show_data((const unsigned char*)code_buffer, size);
    else encoded = "<snip>";

    printf(
        "- action: create\n"
        "  type: shader\n"
        "  id: 0x%" PRIx64 "\n"
        "  content:\n"
        "    type: %d\n"
        "    format: %s\n"
        "    size: %" PRId64 "\n"
        "    %s:\n"
        "      mode: base64\n"
        "      buffer: %s\n",
        req->id, shader_type, format == DVZ_SHADER_SPIRV ? "spirv" : "glsl", //
        size, format == DVZ_SHADER_SPIRV ? "buffer" : "code", encoded);

    IF_VERBOSE_DATA
    free(encoded);
}



static void _print_create_graphics(DvzRequest* req)
{
    log_trace("print_create_graphics");
    ANN(req);
    printf(
        "- action: create\n"
        "  type: graphics\n"
        "  id: 0x%" PRIx64 "\n"
        "  flags: %d\n"
        "  content:\n"
        "    type: %d\n",
        req->id, req->flags, req->content.graphics.type);
}



static void _print_bind_vertex(DvzRequest* req)
{
    log_trace("print_bind_vertex");
    ANN(req);
    printf(
        "- action: bind\n"
        "  type: vertex\n"
        "  id: 0x%" PRIx64 "\n"
        "  content:\n"
        "    binding_idx: %d\n"
        "    dat: 0x%" PRIx64 "\n"
        "    offset: %" PRId64 "\n",
        req->id,                              //
        req->content.bind_vertex.binding_idx, //
        req->content.bind_vertex.dat,         //
        req->content.bind_vertex.offset);
}

static void _print_bind_index(DvzRequest* req)
{
    log_trace("print_bind_index");
    ANN(req);

    printf(
        "- action: bind\n"
        "  type: index\n"
        "  id: 0x%" PRIx64 "\n"
        "  content:\n"
        "    dat: 0x%" PRIx64 "\n"
        "    offset: %" PRId64 "\n",
        req->id,                     //
        req->content.bind_index.dat, //
        req->content.bind_index.offset);
}



static void _print_set_primitive(DvzRequest* req)
{
    log_trace("print_set_primitive");
    ANN(req);

    printf(
        "- action: set\n"
        "  type: primitive\n"
        "  id: 0x%" PRIx64 "\n"
        "  content:\n"
        "    primitive: %d\n",
        req->id, req->content.set_primitive.primitive);
}

static void _print_set_blend(DvzRequest* req)
{
    log_trace("print_set_blend");
    ANN(req);

    printf(
        "- action: set\n"
        "  type: blend\n"
        "  id: 0x%" PRIx64 "\n"
        "  content:\n"
        "    blend: %d\n",
        req->id, req->content.set_blend.blend);
}

static void _print_set_depth(DvzRequest* req)
{
    log_trace("print_set_depth");
    ANN(req);

    printf(
        "- action: set\n"
        "  type: depth\n"
        "  id: 0x%" PRIx64 "\n"
        "  content:\n"
        "    depth: %d\n",
        req->id, req->content.set_depth.depth);
}

static void _print_set_polygon(DvzRequest* req)
{
    log_trace("print_set_polygon");
    ANN(req);

    printf(
        "- action: set\n"
        "  type: polygon\n"
        "  id: 0x%" PRIx64 "\n"
        "  content:\n"
        "    polygon: %d\n",
        req->id, req->content.set_polygon.polygon);
}

static void _print_set_cull(DvzRequest* req)
{
    log_trace("print_set_cull");
    ANN(req);

    printf(
        "- action: set\n"
        "  type: cull\n"
        "  id: 0x%" PRIx64 "\n"
        "  content:\n"
        "    cull: %d\n",
        req->id, req->content.set_cull.cull);
}

static void _print_set_front(DvzRequest* req)
{
    log_trace("print_set_front");
    ANN(req);

    printf(
        "- action: set\n"
        "  type: front\n"
        "  id: 0x%" PRIx64 "\n"
        "  content:\n"
        "    front: %d\n",
        req->id, req->content.set_front.front);
}



static void _print_set_shader(DvzRequest* req)
{
    log_trace("print_set_shader");
    ANN(req);

    DvzId shader = req->content.set_shader.shader;

    printf(
        "- action: set\n"
        "  type: shader\n"
        "  id: 0x%" PRIx64 "\n"
        "  content:\n"
        "    shader:0x%" PRIx64 "\n",
        req->id, shader);
}



static void _print_set_vertex(DvzRequest* req)
{
    log_trace("print_set_vertex");
    ANN(req);

    printf(
        "- action: set\n"
        "  type: vertex\n"
        "  id: 0x%" PRIx64 "\n"
        "  content:\n"
        "    binding_idx: %d\n"
        "    stride: %" PRId64 "\n"
        "    input_rate: %d\n",
        req->id,                             //
        req->content.set_vertex.binding_idx, //
        req->content.set_vertex.stride,      //
        req->content.set_vertex.input_rate);
}

static void _print_set_attr(DvzRequest* req)
{
    log_trace("print_set_attr");
    ANN(req);

    printf(
        "- action: set\n"
        "  type: attr\n"
        "  id: 0x%" PRIx64 "\n"
        "  content:\n"
        "    binding_idx: %d\n"
        "    location: %d\n"
        "    format: %d\n"
        "    offset: %" PRId64 "\n",
        req->id,                           //
        req->content.set_attr.binding_idx, //
        req->content.set_attr.location,    //
        req->content.set_attr.format,      //
        req->content.set_attr.offset);
}

static void _print_set_slot(DvzRequest* req)
{
    log_trace("print_set_slot");
    ANN(req);

    printf(
        "- action: set\n"
        "  type: slot\n"
        "  id: 0x%" PRIx64 "\n"
        "  content:\n"
        "    slot_idx: %d\n"
        "    type: %d\n",
        req->id, req->content.set_slot.slot_idx, req->content.set_slot.type);
}

static void _print_set_specialization(DvzRequest* req)
{
    log_trace("print_create_specialization");
    ANN(req);

    uint32_t idx = req->content.set_specialization.idx;
    DvzShaderType shader = req->content.set_specialization.shader;
    DvzSize size = req->content.set_specialization.size;
    void* value = req->content.set_specialization.value;

    ASSERT(size > 0);
    ANN(value);

    char* encoded = NULL;
    // NOTE: avoid computing the base64 of large arrays.

    IF_VERBOSE_DATA
    encoded = show_data((const unsigned char*)value, size);
    else encoded = "<snip>";

    printf(
        "- action: set\n"
        "  type: specialization\n"
        "  id: 0x%" PRIx64 "\n"
        "  content:\n"
        "    idx: %d\n"
        "    shader: %d\n"
        "    size: %" PRId64 "\n"
        "    value:\n"
        "      mode: base64\n"
        "      buffer: %s\n",
        req->id, idx, shader, size, encoded);

    IF_VERBOSE_DATA
    free(encoded);
}



static void _print_bind_dat(DvzRequest* req)
{
    log_trace("print_bind_dat");
    ANN(req);
    printf(
        "- action: bind\n"
        "  type: dat\n"
        "  id: 0x%" PRIx64 "\n"
        "  content:\n"
        "    slot_idx: %d\n"
        "    dat: 0x%" PRIx64 "\n",
        req->id, req->content.bind_dat.slot_idx, req->content.bind_dat.dat);
}

static void _print_bind_tex(DvzRequest* req)
{
    log_trace("print_bind_tex");
    ANN(req);
    printf(
        "- action: bind\n"
        "  type: tex\n"
        "  id: 0x%" PRIx64 "\n"
        "  content:\n"
        "    slot_idx: %d\n"
        "    tex: 0x%" PRIx64 "\n"
        "    sampler: 0x%" PRIx64 "\n",
        req->id,                        //
        req->content.bind_tex.slot_idx, //
        req->content.bind_tex.tex,      //
        req->content.bind_tex.sampler);
}



static void _print_record_begin(DvzRequest* req)
{
    log_trace("print_record_begin");
    ANN(req);
    printf(
        "- action: record\n"
        "  type: begin\n"
        "  id: 0x%" PRIx64 "\n",
        req->id);
}

static void _print_record_viewport(DvzRequest* req)
{
    log_trace("print_record_viewport");
    ANN(req);
    printf(
        "- action: record\n"
        "  type: viewport\n"
        "  id: 0x%" PRIx64 "\n"
        "  content:\n"
        "    offset: [%.3f, %.3f]\n"
        "    shape: [%.3f, %.3f]\n",
        req->id, //
        req->content.record.command.contents.v.offset[0],
        req->content.record.command.contents.v.offset[1],
        req->content.record.command.contents.v.shape[0],
        req->content.record.command.contents.v.shape[1]);
}

static void _print_record_draw(DvzRequest* req)
{
    log_trace("print_record_draw");
    ANN(req);

    printf(
        "- action: record\n"
        "  type: draw\n"
        "  id: 0x%" PRIx64 "\n"
        "  content:\n"
        "    graphics: 0x%" PRIx64 "\n"
        "    first_vertex: %u\n"
        "    vertex_count: %u\n"
        "    first_instance: %u\n"
        "    instance_count: %u\n",
        req->id, req->content.record.command.contents.draw.pipe_id,
        req->content.record.command.contents.draw.first_vertex,
        req->content.record.command.contents.draw.vertex_count,
        req->content.record.command.contents.draw.first_instance,
        req->content.record.command.contents.draw.instance_count);
}

static void _print_record_draw_indexed(DvzRequest* req)
{
    log_trace("print_record_draw_indexed");
    ANN(req);

    printf(
        "- action: record\n"
        "  type: draw_indexed\n"
        "  id: 0x%" PRIx64 "\n"
        "  content:\n"
        "    graphics: 0x%" PRIx64 "\n"
        "    first_index: %u\n"
        "    vertex_offset: %u\n"
        "    index_count: %u\n"
        "    first_instance: %u\n"
        "    instance_count: %u\n",
        req->id, //
        req->content.record.command.contents.draw_indexed.pipe_id,
        req->content.record.command.contents.draw_indexed.first_index,
        req->content.record.command.contents.draw_indexed.vertex_offset,
        req->content.record.command.contents.draw_indexed.index_count,
        req->content.record.command.contents.draw_indexed.first_instance,
        req->content.record.command.contents.draw_indexed.instance_count);
}

static void _print_record_draw_indirect(DvzRequest* req)
{
    log_trace("print_record_draw_indirect");
    ANN(req);

    printf(
        "- action: record\n"
        "  type: draw_indirect\n"
        "  id: 0x%" PRIx64 "\n"
        "  content:\n"
        "    graphics: 0x%" PRIx64 "\n"
        "    indirect: 0x%" PRIx64 "\n"
        "    draw_count: %u\n",
        req->id, //
        req->content.record.command.contents.draw_indirect.pipe_id,
        req->content.record.command.contents.draw_indirect.dat_indirect_id,
        req->content.record.command.contents.draw_indirect.draw_count);
}

static void _print_record_draw_indexed_indirect(DvzRequest* req)
{
    log_trace("print_record_draw_indexed_indirect");
    ANN(req);

    printf(
        "- action: record\n"
        "  type: draw_indirect\n"
        "  id: 0x%" PRIx64 "\n"
        "  content:\n"
        "    graphics: 0x%" PRIx64 "\n"
        "    indirect: 0x%" PRIx64 "\n"
        "    draw_count: %u\n",
        req->id, //
        req->content.record.command.contents.draw_indirect.pipe_id,
        req->content.record.command.contents.draw_indirect.dat_indirect_id,
        req->content.record.command.contents.draw_indirect.draw_count);
}



static void _print_record_end(DvzRequest* req)
{
    log_trace("print_record_end");
    ANN(req);
    printf(
        "- action: record\n"
        "  type: end\n"
        "  id: 0x%" PRIx64 "\n",
        req->id);
}



void dvz_request_print(DvzRequest* req)
{
    ANN(req);

    IF_REQ(CREATE, BOARD) _print_create_board(req);
    IF_REQ(UPDATE, BOARD) _print_update_board(req);
    IF_REQ(RESIZE, BOARD) _print_resize_board(req);
    IF_REQ(SET, BACKGROUND) _print_set_background(req);
    IF_REQ(DELETE, BOARD) _print_delete_board(req);

    IF_REQ(CREATE, CANVAS) _print_create_canvas(req);
    IF_REQ(DELETE, CANVAS) _print_delete_canvas(req);

    IF_REQ(CREATE, DAT) _print_create_dat(req);
    IF_REQ(RESIZE, DAT) _print_resize_dat(req);
    IF_REQ(UPLOAD, DAT) _print_upload_dat(req);

    IF_REQ(CREATE, TEX) _print_create_tex(req);
    IF_REQ(RESIZE, TEX) _print_resize_tex(req);
    IF_REQ(UPLOAD, TEX) _print_upload_tex(req);

    IF_REQ(CREATE, SAMPLER) _print_create_sampler(req);
    IF_REQ(CREATE, SHADER) _print_create_shader(req);

    IF_REQ(CREATE, GRAPHICS) _print_create_graphics(req);


    IF_REQ(BIND, VERTEX) _print_bind_vertex(req);
    IF_REQ(BIND, INDEX) _print_bind_index(req);
    IF_REQ(SET, PRIMITIVE) _print_set_primitive(req);
    IF_REQ(SET, BLEND) _print_set_blend(req);
    IF_REQ(SET, DEPTH) _print_set_depth(req);
    IF_REQ(SET, POLYGON) _print_set_polygon(req);
    IF_REQ(SET, CULL) _print_set_cull(req);
    IF_REQ(SET, FRONT) _print_set_front(req);
    IF_REQ(SET, SHADER) _print_set_shader(req);
    IF_REQ(SET, VERTEX) _print_set_vertex(req);
    IF_REQ(SET, VERTEX_ATTR) _print_set_attr(req);
    IF_REQ(SET, SLOT) _print_set_slot(req);
    IF_REQ(SET, SPECIALIZATION) _print_set_specialization(req);

    IF_REQ(BIND, DAT) _print_bind_dat(req);
    IF_REQ(BIND, TEX) _print_bind_tex(req);

    IF_REQ(RECORD, RECORD)
    {
        if (req->content.record.command.type == DVZ_RECORDER_BEGIN)
            _print_record_begin(req);
        if (req->content.record.command.type == DVZ_RECORDER_VIEWPORT)
            _print_record_viewport(req);
        if (req->content.record.command.type == DVZ_RECORDER_DRAW)
            _print_record_draw(req);
        if (req->content.record.command.type == DVZ_RECORDER_DRAW_INDEXED)
            _print_record_draw_indexed(req);
        if (req->content.record.command.type == DVZ_RECORDER_DRAW_INDIRECT)
            _print_record_draw_indirect(req);
        if (req->content.record.command.type == DVZ_RECORDER_DRAW_INDEXED_INDIRECT)
            _print_record_draw_indexed_indirect(req);
        if (req->content.record.command.type == DVZ_RECORDER_END)
            _print_record_end(req);
    }
}



/*************************************************************************************************/
/*  Requester                                                                                    */
/*************************************************************************************************/

DvzRequester* dvz_requester(void)
{
    log_trace("create requester");

    // Initialize the global PRNG.
    if (!PRNG)
        PRNG = dvz_prng();

    DvzRequester* rqr = (DvzRequester*)calloc(1, sizeof(DvzRequester));

    // Initialize the FIFO queue of requests.
    rqr->fifo = dvz_fifo(DVZ_MAX_FIFO_CAPACITY);

    IF_VERBOSE
    _print_start();

    return rqr;
}



void dvz_requester_destroy(DvzRequester* rqr)
{
    log_trace("destroy requester");
    ANN(rqr);

    dvz_fifo_destroy(rqr->fifo);
    FREE(rqr);

    // Destroy the global PRNG.
    dvz_prng_destroy(PRNG);

    log_trace("requester destroyed");
}



/*************************************************************************************************/
/*  Request batch                                                                                */
/*************************************************************************************************/

DvzBatch* dvz_batch(void)
{
    // Initialize the global PRNG.
    if (!PRNG)
        PRNG = dvz_prng();

    DvzBatch* batch = (DvzBatch*)calloc(1, sizeof(DvzBatch));
    batch->capacity = DVZ_BATCH_DEFAULT_CAPACITY;
    batch->requests = (DvzRequest*)calloc(DVZ_BATCH_DEFAULT_CAPACITY, sizeof(DvzRequest));
    batch->count = 0;

    batch->pointers_to_free = dvz_list();

    return batch;
}



void dvz_batch_clear(DvzBatch* batch)
{
    ANN(batch);

    if (batch->pointers_to_free != NULL)
    {
        // NOTE: free all pointers created when loading requests dumps.
        uint32_t n = dvz_list_count(batch->pointers_to_free);
        void* pointer = NULL;
        for (uint32_t i = 0; i < n; i++)
        {
            pointer = dvz_list_get(batch->pointers_to_free, i).p;
            FREE(pointer);
        }

        dvz_list_clear(batch->pointers_to_free);
    }

    batch->count = 0;
}



void dvz_batch_add(DvzBatch* batch, DvzRequest req)
{
    ANN(batch);
    ASSERT(batch->count <= batch->capacity);

    // Resize the array if needed.
    if (batch->count == batch->capacity)
    {
        batch->capacity *= 2;
        REALLOC(batch->requests, batch->capacity * sizeof(DvzRequest));
    }
    ASSERT(batch->count < batch->capacity);

    // Append the request.
    batch->requests[batch->count++] = req;
}



// NOTE: the caller must NOT free the result
DvzRequest* dvz_batch_requests(DvzBatch* batch)
{
    ANN(batch);
    // ANN(count);
    // // Batch size.
    // uint32_t n = batch->count;
    // if (n == 0)
    // {
    //     return NULL;
    // }
    // ASSERT(n > 0);

    // // Modify the count pointer to the number of returned requests.
    // *count = n;

    // // Make a copy of the pending requests.
    // DvzRequest* requests = (DvzRequest*)calloc(n, sizeof(DvzRequest));
    // memcpy(requests, batch->requests, n * sizeof(DvzRequest));
    return batch->requests;
}



uint32_t dvz_batch_size(DvzBatch* batch)
{
    ANN(batch);
    return batch->count;
}



void dvz_batch_print(DvzBatch* batch)
{
    ANN(batch);

    _print_start();

    uint32_t count = batch->count;
    for (uint32_t i = 0; i < count; i++)
    {
        log_trace("print request %d/%d", i + 1, count);
        dvz_request_print(&batch->requests[i]);
    }
}



int dvz_batch_dump(DvzBatch* batch, const char* filename)
{
    ANN(batch);
    ANN(filename);

    int res = 0;
    uint32_t count = batch->count;
    if (count == 0)
    {
        log_error("empty requester, aborting requester dump");
        return 1;
    }

    log_trace("start serializing %d requests", count);

    // Dump the DvzRequest structures.
    log_trace("saving main dump file `%s`", filename);
    res = write_file(filename, sizeof(DvzRequest), count, batch->requests);
    if (res != 0)
        return res;

    // Write additional files for uploaded data.
    DvzRequest* req = NULL;
    DvzRequestContent* c = NULL;
    char filename_bin[32] = {0};
    uint32_t k = 1;
    for (uint32_t i = 0; i < batch->count; i++)
    {
        req = &batch->requests[i];
        c = &req->content;
        ANN(req);

        if (req->action == DVZ_REQUEST_ACTION_UPLOAD)
        {
            // Increment the filename.
            snprintf(filename_bin, 30, "%s.%03d", filename, k++);
            log_trace("saving secondary dump file `%s`", filename_bin);

            ANN(c);
            if (req->type == DVZ_REQUEST_OBJECT_DAT)
            {
                if (write_file(filename_bin, c->dat_upload.size, 1, c->dat_upload.data) != 0)
                    return 1;
            }
            else if (req->type == DVZ_REQUEST_OBJECT_TEX)
            {
                if (write_file(filename_bin, c->tex_upload.size, 1, c->tex_upload.data) != 0)
                    return 1;
            }
        }
    }

    return 0;
}



void dvz_batch_load(DvzBatch* batch, const char* filename)
{
    ANN(batch);
    ANN(filename);

    ANN(batch->requests);

    // int res = 0;
    log_trace("start deserializing requests from file `%s`", filename);

    // Dump the DvzRequest structures.
    log_trace("load main dump file `%s`", filename);

    DvzSize size = 0;
    DvzRequest* requests = (DvzRequest*)dvz_read_file(filename, &size);
    if (requests == NULL)
    {
        log_error("unable to read `%s`", filename);
        return;
    }
    ASSERT(size > 0);

    // Number of requests.
    uint32_t count = size / sizeof(DvzRequest);

    // Write additional files for uploaded data.
    DvzRequest* req = NULL;
    DvzRequestContent* c = NULL;
    char filename_bin[32] = {0};
    uint32_t k = 1;

    for (uint32_t i = 0; i < count; i++)
    {
        req = &requests[i];
        c = &req->content;
        ANN(req);

        if (req->action == DVZ_REQUEST_ACTION_UPLOAD)
        {
            // Increment the filename.
            snprintf(filename_bin, 30, "%s.%03d", filename, k++);
            log_trace("saving secondary dump file `%s`", filename_bin);

            ANN(c);
            if (req->type == DVZ_REQUEST_OBJECT_DAT)
            {
                c->dat_upload.data = (void*)dvz_read_file(filename_bin, &c->dat_upload.size);
                dvz_list_append(batch->pointers_to_free, (DvzListItem){.p = c->dat_upload.data});
            }
            else if (req->type == DVZ_REQUEST_OBJECT_TEX)
            {
                c->tex_upload.data = (void*)dvz_read_file(filename_bin, &c->tex_upload.size);
                dvz_list_append(batch->pointers_to_free, (DvzListItem){.p = c->tex_upload.data});
            }
        }

        dvz_batch_add(batch, *req);
    }
}



DvzBatch* dvz_batch_copy(DvzBatch* batch)
{
    ANN(batch);
    DvzBatch* cpy = (DvzBatch*)_cpy(sizeof(DvzBatch), batch);
    cpy->pointers_to_free = NULL;
    cpy->requests = (DvzRequest*)_cpy(batch->capacity * sizeof(DvzRequest), batch->requests);
    return cpy;
}



void dvz_batch_destroy(DvzBatch* batch)
{
    ANN(batch);

    dvz_batch_clear(batch);

    if (batch->pointers_to_free != NULL)
    {
        dvz_list_destroy(batch->pointers_to_free);
        batch->pointers_to_free = NULL;
    }

    FREE(batch->requests);
    FREE(batch);
}



/*************************************************************************************************/
/*  Requester functions                                                                          */
/*************************************************************************************************/

// Enqueue a copy of the batch to the requester's FIFO queue.
void dvz_requester_commit(DvzRequester* rqr, DvzBatch* batch)
{
    ANN(rqr);
    ANN(batch);

    DvzBatch* batch_cpy = (DvzBatch*)_cpy(sizeof(DvzBatch), batch);
    dvz_fifo_enqueue(rqr->fifo, batch_cpy);
}



// Return a copy of all batches in the FIFO queue, and clear the FIFO queue.
// NOTE: the caller MUST free the output.
DvzBatch* dvz_requester_flush(DvzRequester* rqr, uint32_t* count)
{
    ANN(rqr);
    ANN(count);

    int size = dvz_fifo_size(rqr->fifo);
    ASSERT(size >= 0);
    ASSERT(size < (int)UINT16_MAX);
    *count = (uint32_t)size;

    DvzBatch* batches = (DvzBatch*)calloc(*count, sizeof(DvzBatch));
    for (uint32_t i = 0; i < *count; i++)
    {
        memcpy(&batches[i], dvz_fifo_dequeue(rqr->fifo, false), sizeof(DvzBatch));
    }
    return batches;
}



/*************************************************************************************************/
/*  Board                                                                                        */
/*************************************************************************************************/

DvzRequest
dvz_create_board(DvzBatch* batch, uint32_t width, uint32_t height, cvec4 background, int flags)
{
    CREATE_REQUEST(CREATE, BOARD);
    req.id = dvz_prng_uuid(PRNG);
    req.flags = flags;
    req.content.board.width = width;
    req.content.board.height = height;
    memcpy(req.content.board.background, background, sizeof(cvec4));

    IF_VERBOSE
    _print_create_board(&req);

    RETURN_REQUEST
}



DvzRequest dvz_update_board(DvzBatch* batch, DvzId id)
{
    CREATE_REQUEST(UPDATE, BOARD);
    req.id = id;

    IF_VERBOSE
    _print_update_board(&req);

    RETURN_REQUEST
}



DvzRequest dvz_resize_board(DvzBatch* batch, DvzId board, uint32_t width, uint32_t height)
{
    CREATE_REQUEST(RESIZE, BOARD);
    req.id = board;
    req.content.board.width = width;
    req.content.board.height = height;

    IF_VERBOSE
    _print_resize_board(&req);

    RETURN_REQUEST
}



DvzRequest dvz_set_background(DvzBatch* batch, DvzId id, cvec4 background)
{
    CREATE_REQUEST(SET, BACKGROUND);
    req.id = id;
    memcpy(req.content.board.background, background, sizeof(cvec4));

    IF_VERBOSE
    _print_set_background(&req);

    RETURN_REQUEST
}



DvzRequest dvz_delete_board(DvzBatch* batch, DvzId id)
{
    CREATE_REQUEST(DELETE, BOARD);
    req.id = id;

    IF_VERBOSE
    _print_delete_board(&req);

    RETURN_REQUEST
}



/*************************************************************************************************/
/*  Canvas                                                                                       */
/*************************************************************************************************/

DvzRequest
dvz_create_canvas(DvzBatch* batch, uint32_t width, uint32_t height, cvec4 background, int flags)
{
    CREATE_REQUEST(CREATE, CANVAS);
    req.id = dvz_prng_uuid(PRNG);
    req.flags = flags;
    req.content.canvas.screen_width = width;
    req.content.canvas.screen_height = height;

    // NOTE: the framebuffer size will have to be determined once the window has been created.
    // As a fallback, the window size will be taken as equal to the screen size.
    memcpy(req.content.canvas.background, background, sizeof(cvec4));

    IF_VERBOSE
    _print_create_canvas(&req);

    RETURN_REQUEST
}



DvzRequest dvz_delete_canvas(DvzBatch* batch, DvzId id)
{
    CREATE_REQUEST(DELETE, CANVAS);
    req.id = id;

    IF_VERBOSE
    _print_delete_canvas(&req);

    RETURN_REQUEST
}



/*************************************************************************************************/
/*  Dat                                                                                          */
/*************************************************************************************************/

DvzRequest dvz_create_dat(DvzBatch* batch, DvzBufferType type, DvzSize size, int flags)
{
    CREATE_REQUEST(CREATE, DAT);
    req.id = dvz_prng_uuid(PRNG);
    req.flags = flags;
    req.content.dat.type = type;
    req.content.dat.size = size;

    IF_VERBOSE
    _print_create_dat(&req);

    RETURN_REQUEST
}



DvzRequest dvz_resize_dat(DvzBatch* batch, DvzId dat, DvzSize size)
{
    ASSERT(size > 0);

    CREATE_REQUEST(RESIZE, DAT);
    req.id = dat;
    req.content.dat.size = size;

    IF_VERBOSE
    _print_resize_dat(&req);

    RETURN_REQUEST
}



DvzRequest dvz_upload_dat(DvzBatch* batch, DvzId dat, DvzSize offset, DvzSize size, void* data)
{
    ASSERT(size > 0);
    ANN(data);

    CREATE_REQUEST(UPLOAD, DAT);
    req.id = dat;
    req.content.dat_upload.offset = offset;
    req.content.dat_upload.size = size;

    // NOTE: we make a copy of the data to ensure it lives until the renderer has done processing
    // it.
    req.content.dat_upload.data = _cpy(size, data);

    IF_VERBOSE
    _print_upload_dat(&req);

    RETURN_REQUEST
}



/*************************************************************************************************/
/*  Tex                                                                                          */
/*************************************************************************************************/

DvzRequest
dvz_create_tex(DvzBatch* batch, DvzTexDims dims, DvzFormat format, uvec3 shape, int flags)
{
    CREATE_REQUEST(CREATE, TEX);
    req.id = dvz_prng_uuid(PRNG);
    req.flags = flags;
    req.content.tex.dims = dims;
    memcpy(req.content.tex.shape, shape, sizeof(uvec3));
    req.content.tex.format = format;

    IF_VERBOSE
    _print_create_tex(&req);

    RETURN_REQUEST
}



DvzRequest dvz_resize_tex(DvzBatch* batch, DvzId tex, uvec3 shape)
{
    CREATE_REQUEST(RESIZE, TEX);
    req.id = tex;
    memcpy(req.content.tex.shape, shape, sizeof(uvec3));

    IF_VERBOSE
    _print_resize_tex(&req);

    RETURN_REQUEST
}



DvzRequest
dvz_upload_tex(DvzBatch* batch, DvzId tex, uvec3 offset, uvec3 shape, DvzSize size, void* data)
{
    CREATE_REQUEST(UPLOAD, TEX);
    req.id = tex;

    memcpy(req.content.tex_upload.offset, offset, sizeof(uvec3));
    memcpy(req.content.tex_upload.shape, shape, sizeof(uvec3));
    req.content.tex_upload.size = size;

    // NOTE: we make a copy of the data to ensure it lives until the renderer has done processing
    // it.
    req.content.tex_upload.data = _cpy(size, data);

    IF_VERBOSE
    _print_upload_tex(&req);

    RETURN_REQUEST
}



/*************************************************************************************************/
/*  Sampler                                                                                      */
/*************************************************************************************************/

DvzRequest dvz_create_sampler(DvzBatch* batch, DvzFilter filter, DvzSamplerAddressMode mode)
{
    CREATE_REQUEST(CREATE, SAMPLER);
    req.id = dvz_prng_uuid(PRNG);
    req.content.sampler.filter = filter;
    req.content.sampler.mode = mode;

    IF_VERBOSE
    _print_create_sampler(&req);

    RETURN_REQUEST
}



/*************************************************************************************************/
/*  Shaders                                                                                      */
/*************************************************************************************************/

DvzRequest
dvz_create_glsl(DvzBatch* batch, DvzShaderType shader_type, DvzSize size, const char* code)
{
    ANN(code);
    ASSERT(size > 0);

    CREATE_REQUEST(CREATE, SHADER);
    req.id = dvz_prng_uuid(PRNG);
    req.content.shader.type = shader_type;
    req.content.shader.size = size;
    req.content.shader.code = _cpy(size, code); // NOTE: the renderer will need to free it

    IF_VERBOSE _print_create_shader(&req);

    RETURN_REQUEST
}



DvzRequest dvz_create_spirv(
    DvzBatch* batch, DvzShaderType shader_type, DvzSize size, const unsigned char* buffer)
{
    ANN(buffer);
    ASSERT(size > 0);

    CREATE_REQUEST(CREATE, SHADER);
    req.id = dvz_prng_uuid(PRNG);
    req.content.shader.type = shader_type;
    req.content.shader.size = size;
    req.content.shader.buffer = _cpy(size, buffer); // NOTE: the renderer will need to free it

    IF_VERBOSE _print_create_shader(&req);

    RETURN_REQUEST
}



/*************************************************************************************************/
/*  Graphics                                                                                     */
/*************************************************************************************************/

DvzRequest dvz_create_graphics(DvzBatch* batch, DvzGraphicsType type, int flags)
{
    CREATE_REQUEST(CREATE, GRAPHICS);
    req.id = dvz_prng_uuid(PRNG);
    req.flags = flags;
    req.content.graphics.type = type;

    IF_VERBOSE
    _print_create_graphics(&req);

    RETURN_REQUEST
}



DvzRequest dvz_set_primitive(DvzBatch* batch, DvzId graphics, DvzPrimitiveTopology primitive)
{
    CREATE_REQUEST(SET, PRIMITIVE);
    req.id = graphics;
    req.content.set_primitive.primitive = primitive;

    IF_VERBOSE
    _print_set_primitive(&req);

    RETURN_REQUEST
}



DvzRequest dvz_set_blend(DvzBatch* batch, DvzId graphics, DvzBlendType blend_type)
{
    CREATE_REQUEST(SET, BLEND);
    req.id = graphics;
    req.content.set_blend.blend = blend_type;

    IF_VERBOSE
    _print_set_blend(&req);

    RETURN_REQUEST
}



DvzRequest dvz_set_depth(DvzBatch* batch, DvzId graphics, DvzDepthTest depth_test)
{
    CREATE_REQUEST(SET, DEPTH);
    req.id = graphics;
    req.content.set_depth.depth = depth_test;

    IF_VERBOSE
    _print_set_depth(&req);

    RETURN_REQUEST
}



DvzRequest dvz_set_polygon(DvzBatch* batch, DvzId graphics, DvzPolygonMode polygon_mode)
{
    CREATE_REQUEST(SET, POLYGON);
    req.id = graphics;
    req.content.set_polygon.polygon = polygon_mode;

    IF_VERBOSE
    _print_set_polygon(&req);

    RETURN_REQUEST
}



DvzRequest dvz_set_cull(DvzBatch* batch, DvzId graphics, DvzCullMode cull_mode)
{
    CREATE_REQUEST(SET, CULL);
    req.id = graphics;
    req.content.set_cull.cull = cull_mode;

    IF_VERBOSE
    _print_set_cull(&req);

    RETURN_REQUEST
}



DvzRequest dvz_set_front(DvzBatch* batch, DvzId graphics, DvzFrontFace front_face)
{
    CREATE_REQUEST(SET, FRONT);
    req.id = graphics;
    req.content.set_front.front = front_face;

    IF_VERBOSE
    _print_set_front(&req);

    RETURN_REQUEST
}



DvzRequest dvz_set_shader(DvzBatch* batch, DvzId graphics, DvzId shader)
{
    CREATE_REQUEST(SET, SHADER);
    req.id = graphics;
    req.content.set_shader.shader = shader;

    IF_VERBOSE
    _print_set_shader(&req);

    RETURN_REQUEST
}



DvzRequest dvz_set_vertex(
    DvzBatch* batch, DvzId graphics, uint32_t binding_idx, DvzSize stride,
    DvzVertexInputRate input_rate)
{
    CREATE_REQUEST(SET, VERTEX);
    req.id = graphics;
    req.content.set_vertex.binding_idx = binding_idx;
    req.content.set_vertex.stride = stride;
    req.content.set_vertex.binding_idx = input_rate;

    IF_VERBOSE
    _print_set_vertex(&req);

    RETURN_REQUEST
}



DvzRequest dvz_set_attr(
    DvzBatch* batch, DvzId graphics, uint32_t binding_idx, uint32_t location, DvzFormat format,
    DvzSize offset)
{
    CREATE_REQUEST(SET, VERTEX_ATTR);
    req.id = graphics;
    req.content.set_attr.binding_idx = binding_idx;
    req.content.set_attr.location = location;
    req.content.set_attr.format = format;
    req.content.set_attr.offset = offset;

    IF_VERBOSE
    _print_set_attr(&req);

    RETURN_REQUEST
}



DvzRequest dvz_set_slot(DvzBatch* batch, DvzId graphics, uint32_t slot_idx, DvzDescriptorType type)
{
    CREATE_REQUEST(SET, SLOT);
    req.id = graphics;
    req.content.set_slot.slot_idx = slot_idx;
    req.content.set_slot.type = type;

    IF_VERBOSE
    _print_set_slot(&req);

    RETURN_REQUEST
}



DvzRequest dvz_set_specialization(
    DvzBatch* batch, DvzId graphics, DvzShaderType shader, uint32_t idx, DvzSize size, void* value)
{
    ASSERT(size > 0);
    ANN(value);

    CREATE_REQUEST(SET, SPECIALIZATION);
    req.id = graphics;
    req.content.set_specialization.shader = shader;
    req.content.set_specialization.idx = idx;
    req.content.set_specialization.size = size;
    req.content.set_specialization.value =
        _cpy(size, value); // NOTE: the renderer will have to free it.

    IF_VERBOSE
    _print_set_specialization(&req);

    RETURN_REQUEST
}



/*************************************************************************************************/
/*  Bindings                                                                                     */
/*************************************************************************************************/

DvzRequest
dvz_bind_vertex(DvzBatch* batch, DvzId graphics, uint32_t binding_idx, DvzId dat, DvzSize offset)
{
    CREATE_REQUEST(BIND, VERTEX);
    req.id = graphics;
    req.content.bind_vertex.binding_idx = binding_idx;
    req.content.bind_vertex.dat = dat;
    req.content.bind_vertex.offset = offset;

    IF_VERBOSE
    _print_bind_vertex(&req);

    RETURN_REQUEST
}



DvzRequest dvz_bind_index(DvzBatch* batch, DvzId graphics, DvzId dat, DvzSize offset)
{
    CREATE_REQUEST(BIND, INDEX);
    req.id = graphics;
    req.content.bind_index.dat = dat;
    req.content.bind_index.offset = offset;

    IF_VERBOSE
    _print_bind_index(&req);

    RETURN_REQUEST
}



DvzRequest dvz_bind_dat(DvzBatch* batch, DvzId pipe, uint32_t slot_idx, DvzId dat, DvzSize offset)
{
    CREATE_REQUEST(BIND, DAT);
    req.id = pipe;
    req.content.bind_dat.slot_idx = slot_idx;
    req.content.bind_dat.dat = dat;
    req.content.bind_dat.offset = offset;

    IF_VERBOSE
    _print_bind_dat(&req);

    RETURN_REQUEST
}



DvzRequest dvz_bind_tex(
    DvzBatch* batch, DvzId pipe, uint32_t slot_idx, DvzId tex, DvzId sampler, uvec3 offset)
{
    CREATE_REQUEST(BIND, TEX);
    req.id = pipe;
    req.content.bind_tex.slot_idx = slot_idx;
    req.content.bind_tex.tex = tex;
    req.content.bind_tex.sampler = sampler;
    memcpy(&req.content.bind_tex.offset, offset, sizeof(uvec3));

    IF_VERBOSE
    _print_bind_tex(&req);

    RETURN_REQUEST
}



/*************************************************************************************************/
/*  Command buffer                                                                               */
/*************************************************************************************************/

DvzRequest dvz_record_begin(DvzBatch* batch, DvzId canvas_or_board_id)
{
    CREATE_REQUEST(RECORD, RECORD);
    req.id = canvas_or_board_id;
    req.content.record.command.type = DVZ_RECORDER_BEGIN;

    IF_VERBOSE
    _print_record_begin(&req);

    RETURN_REQUEST
}



DvzRequest dvz_record_viewport(DvzBatch* batch, DvzId canvas_or_board_id, vec2 offset, vec2 shape)
{
    CREATE_REQUEST(RECORD, RECORD);
    req.id = canvas_or_board_id;
    req.content.record.command.type = DVZ_RECORDER_VIEWPORT;
    glm_vec2_copy(offset, req.content.record.command.contents.v.offset);
    glm_vec2_copy(shape, req.content.record.command.contents.v.shape);

    IF_VERBOSE
    _print_record_viewport(&req);

    RETURN_REQUEST
}



DvzRequest dvz_record_draw(
    DvzBatch* batch, DvzId canvas_or_board_id, DvzId graphics, //
    uint32_t first_vertex, uint32_t vertex_count,              //
    uint32_t first_instance, uint32_t instance_count)
{
    CREATE_REQUEST(RECORD, RECORD);
    req.id = canvas_or_board_id;
    req.content.record.command.type = DVZ_RECORDER_DRAW;
    req.content.record.command.contents.draw.pipe_id = graphics;
    req.content.record.command.contents.draw.first_vertex = first_vertex;
    req.content.record.command.contents.draw.vertex_count = vertex_count;
    req.content.record.command.contents.draw.first_instance = first_instance;
    req.content.record.command.contents.draw.instance_count = instance_count;

    IF_VERBOSE
    _print_record_draw(&req);

    RETURN_REQUEST
}



DvzRequest dvz_record_draw_indexed(
    DvzBatch* batch, DvzId canvas_or_board_id, DvzId graphics,          //
    uint32_t first_index, uint32_t vertex_offset, uint32_t index_count, //
    uint32_t first_instance, uint32_t instance_count)
{
    CREATE_REQUEST(RECORD, RECORD);
    req.id = canvas_or_board_id;
    req.content.record.command.type = DVZ_RECORDER_DRAW_INDEXED;
    req.content.record.command.contents.draw_indexed.pipe_id = graphics;
    req.content.record.command.contents.draw_indexed.first_index = first_index;
    req.content.record.command.contents.draw_indexed.vertex_offset = vertex_offset;
    req.content.record.command.contents.draw_indexed.index_count = index_count;
    req.content.record.command.contents.draw_indexed.first_instance = first_instance;
    req.content.record.command.contents.draw_indexed.instance_count = instance_count;

    IF_VERBOSE
    _print_record_draw_indexed(&req);

    RETURN_REQUEST
}



DvzRequest dvz_record_draw_indirect(
    DvzBatch* batch, DvzId canvas_or_board_id, DvzId graphics, DvzId indirect, uint32_t draw_count)
{
    CREATE_REQUEST(RECORD, RECORD);
    req.id = canvas_or_board_id;
    req.content.record.command.type = DVZ_RECORDER_DRAW_INDIRECT;
    req.content.record.command.contents.draw_indirect.pipe_id = graphics;
    req.content.record.command.contents.draw_indirect.dat_indirect_id = indirect;
    req.content.record.command.contents.draw_indirect.draw_count = draw_count;

    IF_VERBOSE
    _print_record_draw_indirect(&req);

    RETURN_REQUEST
}



DvzRequest dvz_record_draw_indexed_indirect(
    DvzBatch* batch, DvzId canvas_or_board_id, DvzId graphics, DvzId indirect, uint32_t draw_count)
{
    CREATE_REQUEST(RECORD, RECORD);
    req.id = canvas_or_board_id;
    req.content.record.command.type = DVZ_RECORDER_DRAW_INDEXED_INDIRECT;
    req.content.record.command.contents.draw_indirect.pipe_id = graphics;
    req.content.record.command.contents.draw_indirect.dat_indirect_id = indirect;
    req.content.record.command.contents.draw_indirect.draw_count = draw_count;

    IF_VERBOSE
    _print_record_draw_indexed_indirect(&req);

    RETURN_REQUEST
}



DvzRequest dvz_record_end(DvzBatch* batch, DvzId canvas_or_board_id)
{
    CREATE_REQUEST(RECORD, RECORD);
    req.id = canvas_or_board_id;
    req.content.record.command.type = DVZ_RECORDER_END;

    IF_VERBOSE
    _print_record_end(&req);

    RETURN_REQUEST
}
