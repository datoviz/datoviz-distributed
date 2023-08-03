/*************************************************************************************************/
/*  Visual                                                                                       */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include "scene/visual.h"
#include "_map.h"
#include "fileio.h"
#include "request.h"
#include "scene/baker.h"
#include "scene/dual.h"
#include "scene/graphics.h"
#include "scene/params.h"



/*************************************************************************************************/
/*  Macros                                                                                       */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Util functions                                                                               */
/*************************************************************************************************/

// static DvzSize get_attr_size(DvzFormat format)
// {
//     switch (format)
//     {
//     case DVZ_FORMAT_R32_SFLOAT:
//         return sizeof(float);

//     case DVZ_FORMAT_R32G32_SFLOAT:
//         return sizeof(vec2);

//     case DVZ_FORMAT_R32G32B32_SFLOAT:
//         return sizeof(vec3);

//     case DVZ_FORMAT_R32G32B32A32_SFLOAT:
//         return sizeof(vec4);

//     case DVZ_FORMAT_R8G8B8A8_UNORM:
//         return sizeof(cvec4);

//     case DVZ_FORMAT_R8_UNORM:
//         return sizeof(uint8_t);

//     default:
//         log_error("DvzFormat %d has not yet been implemented in get_attr_size()", format);
//         return 0;
//     }
//     // TODO: other formats
//     return 0;
// }



/*************************************************************************************************/
/*  Visual lifecycle                                                                             */
/*************************************************************************************************/

DvzVisual* dvz_visual(DvzRequester* rqr, DvzPrimitiveTopology primitive, int flags)
{
    ANN(rqr);

    DvzVisual* visual = (DvzVisual*)calloc(1, sizeof(DvzVisual));

    visual->flags = flags;
    visual->rqr = rqr;

    // No callback by default, will just use dvz_visual_instance().
    visual->callback = NULL;

    visual->baker = dvz_baker(rqr, 0);

    // Create the graphics object.
    DvzRequest req = dvz_create_graphics(rqr, DVZ_GRAPHICS_CUSTOM, 0);
    visual->graphics_id = req.id;
    visual->is_visible = true;

    // Default fixed function pipeline states:

    // Primitive topology.
    dvz_set_primitive(rqr, visual->graphics_id, primitive);

    // Polygon mode.
    dvz_set_polygon(rqr, visual->graphics_id, DVZ_POLYGON_MODE_FILL);

    dvz_obj_init(&visual->obj);
    return visual;
}



void dvz_visual_update(DvzVisual* visual)
{
    ANN(visual);
    dvz_baker_update(visual->baker);

    for (uint32_t i = 0; i < DVZ_MAX_BINDINGS; i++)
    {
        if (visual->params[i] != NULL)
            dvz_params_update(visual->params[i]);
    }
}



void dvz_visual_destroy(DvzVisual* visual)
{
    ANN(visual);
    if (visual->group_sizes != NULL)
    {
        FREE(visual->group_sizes);
    }

    // Destroy the params.
    for (uint32_t i = 0; i < DVZ_MAX_BINDINGS; i++)
    {
        if (visual->params[i] && !visual->params[i]->is_shared)
        {
            dvz_params_destroy(visual->params[i]);
        }
    }
    FREE(visual);
}



/*************************************************************************************************/
/*  Visual fixed pipeline                                                                        */
/*************************************************************************************************/

void dvz_visual_primitive(DvzVisual* visual, DvzPrimitiveTopology primitive)
{
    ANN(visual);
    DvzRequester* rqr = visual->rqr;
    ANN(rqr);

    dvz_set_primitive(rqr, visual->graphics_id, primitive);
}



void dvz_visual_blend(DvzVisual* visual, DvzBlendType blend_type)
{
    ANN(visual);
    DvzRequester* rqr = visual->rqr;
    ANN(rqr);

    dvz_set_blend(rqr, visual->graphics_id, blend_type);
}



void dvz_visual_depth(DvzVisual* visual, DvzDepthTest depth_test)
{
    ANN(visual);
    DvzRequester* rqr = visual->rqr;
    ANN(rqr);

    dvz_set_depth(rqr, visual->graphics_id, depth_test);
}



void dvz_visual_polygon(DvzVisual* visual, DvzPolygonMode polygon_mode)
{
    ANN(visual);
    DvzRequester* rqr = visual->rqr;
    ANN(rqr);

    dvz_set_polygon(rqr, visual->graphics_id, polygon_mode);
}



void dvz_visual_cull(DvzVisual* visual, DvzCullMode cull_mode)
{
    ANN(visual);
    DvzRequester* rqr = visual->rqr;
    ANN(rqr);

    dvz_set_cull(rqr, visual->graphics_id, cull_mode);
}



void dvz_visual_front(DvzVisual* visual, DvzFrontFace front_face)
{
    ANN(visual);
    DvzRequester* rqr = visual->rqr;
    ANN(rqr);

    dvz_set_front(rqr, visual->graphics_id, front_face);
}



/*************************************************************************************************/
/*  Visual declaration                                                                           */
/*************************************************************************************************/

void dvz_visual_spirv(
    DvzVisual* visual, DvzShaderType type, DvzSize size, const unsigned char* buffer)
{
    ANN(visual);
    ANN(buffer);
    ASSERT(size > 0);

    // First, create a shader object.
    DvzRequest req = dvz_create_spirv(visual->rqr, type, size, buffer);

    // Then associate it to the graphics.
    dvz_set_shader(visual->rqr, visual->graphics_id, req.id);
}



void dvz_visual_shader(DvzVisual* visual, const char* name)
{
    ANN(visual);
    ANN(name);
    ASSERT(strlen(name) < 50);

    unsigned long size = 0;
    char rname[64] = {0};

    // Vertex shader.
    snprintf(rname, 60, "%s_%s", name, "vert");
    unsigned char* buffer = dvz_resource_shader(rname, &size);
    dvz_visual_spirv(visual, DVZ_SHADER_VERTEX, size, buffer);

    memset(rname, 0, 64);

    // Fragment shader.
    snprintf(rname, 60, "%s_%s", name, "frag");
    buffer = dvz_resource_shader(rname, &size);
    dvz_visual_spirv(visual, DVZ_SHADER_FRAGMENT, size, buffer);
}



void dvz_visual_resize(DvzVisual* visual, uint32_t item_count, uint32_t vertex_count)
{
    ANN(visual);
    ASSERT(item_count > 0);

    // Mark the new item count.
    visual->item_count = item_count;
    visual->vertex_count = vertex_count;

    // Resize the baker, resize the underlying arrays, emit the dat resize commands.
    // TODO: write tests, NOT TESTED YET
    dvz_baker_resize(visual->baker, vertex_count);

    // TODO: resize the groups?
}



void dvz_visual_groups(DvzVisual* visual, uint32_t group_count, uint32_t* group_sizes)
{
    ANN(visual);
    ASSERT(group_count > 0);

    // Allocate the group sizes.
    if (visual->group_sizes == NULL)
    {
        visual->group_sizes = (uint32_t*)calloc(group_count, sizeof(uint32_t));
    }

    // Ensure the group_sizes array is large enough to hold all group sizes.
    if (group_count > visual->group_count)
    {
        REALLOC(visual->group_sizes, group_count * sizeof(uint32_t));
    }
    ASSERT(visual->group_count >= group_count);

    // Copy the group sizes.
    memcpy(visual->group_sizes, group_sizes, group_count * sizeof(uint32_t));
    visual->group_count = group_count;
}



void dvz_visual_attr(
    DvzVisual* visual, uint32_t attr_idx, DvzSize offset, DvzSize item_size, //
    DvzFormat format, int flags)
{
    ANN(visual);
    ASSERT(attr_idx < DVZ_MAX_VERTEX_ATTRS);
    ASSERT(item_size > 0);

    // NOTE: lazy spec of vertex bindings and attrs, as this will depend on all attrs.
    // Will be done at create time. Will have to do baker side and GPU request side.
    visual->attrs[attr_idx].offset = offset;
    visual->attrs[attr_idx].item_size = item_size;
    visual->attrs[attr_idx].format = format;
    visual->attrs[attr_idx].flags = flags;

    // NOTE: the last significant 4 bits of the flags encode the binding_idx. It's 0 for all flags,
    // except DVZ_ATTR_FLAGS_DYNAMIC where it is 1.
    visual->attrs[attr_idx].binding_idx = (uint32_t)(flags & 0x000F);
}



void dvz_visual_stride(DvzVisual* visual, uint32_t binding_idx, DvzSize stride)
{
    ANN(visual);
    ASSERT(binding_idx < DVZ_MAX_VERTEX_BINDINGS);
    visual->strides[binding_idx] = stride;
}



void dvz_visual_slot(DvzVisual* visual, uint32_t slot_idx, DvzSlotType type)
{
    ANN(visual)
    ASSERT(slot_idx < DVZ_MAX_BINDINGS);

    // Declare a slot.
    dvz_set_slot(
        visual->rqr, visual->graphics_id, slot_idx,
        type == DVZ_SLOT_DAT ? DVZ_DESCRIPTOR_TYPE_UNIFORM_BUFFER : DVZ_DESCRIPTOR_TYPE_SAMPLER);
}



void dvz_visual_params(DvzVisual* visual, uint32_t slot_idx, DvzParams* params)
{
    ANN(visual);
    ANN(visual->baker);
    ANN(params);

    ASSERT(visual->graphics_id != DVZ_ID_NONE);
    ASSERT(slot_idx < DVZ_MAX_BINDINGS);

    // Set a params object.
    visual->params[slot_idx] = params; // dvz_params(visual->rqr, size);

    // Call a bind_dat request for the visual graphics and the dual's dat.
    dvz_params_bind(visual->params[slot_idx], visual->graphics_id, slot_idx);
}



void dvz_visual_tex(DvzVisual* visual, uint32_t slot_idx, DvzId tex, DvzId sampler, uvec3 offset)
{
    ANN(visual);
    ANN(visual->baker);

    // Declare a slot.
    dvz_set_slot(visual->rqr, visual->graphics_id, slot_idx, DVZ_DESCRIPTOR_TYPE_SAMPLER);

    // Bind the texture to the graphics.
    dvz_bind_tex(visual->rqr, visual->graphics_id, slot_idx, tex, sampler, offset);
}



/*************************************************************************************************/
/*  Visual creation                                                                              */
/*************************************************************************************************/

void dvz_visual_alloc(
    DvzVisual* visual, uint32_t item_count, uint32_t vertex_count, uint32_t index_count)
{
    ANN(visual);
    ASSERT(vertex_count > 0);
    if (item_count == 0)
    {
        log_debug(
            "when allocating visual, item_count is 0, so using vertex_count instead (%d)",
            vertex_count);
        item_count = vertex_count;
    }
    ASSERT(item_count > 0);

    DvzBaker* baker = visual->baker;
    ANN(baker);

    DvzRequester* rqr = visual->rqr;
    ANN(rqr);

    DvzId graphics_id = visual->graphics_id;
    ASSERT(graphics_id != DVZ_ID_NONE);

    // Compute the offsets of each attribute within their vertex bindings, and the vertex bindings
    // strides.
    DvzSize attr_offsets[DVZ_MAX_VERTEX_BINDINGS] = {0};
    DvzVisualAttr* attr = NULL;
    uint32_t binding_idx = 0;
    uint32_t attr_count = 0;
    uint32_t binding_count = 0;
    for (uint32_t attr_idx = 0; attr_idx < DVZ_MAX_VERTEX_ATTRS; attr_idx++)
    {
        attr = &visual->attrs[attr_idx];
        ANN(attr);

        if (attr->format == DVZ_FORMAT_NONE)
        {
            break;
        }

        // The vertex binding of the current vertex attribute is directly encoded in the flags.
        // It was set in dvz_visual_attr();
        binding_idx = attr->binding_idx;

        // Count the number of vertex bindings.
        // NOTE: we assume that the number of vertex bindings is the maximum binding idx + 1.
        binding_count = MAX(binding_idx + 1, binding_count);

        ASSERT(binding_count <= DVZ_MAX_VERTEX_BINDINGS);

        // Compute the offset and item_size of each attribute.
        attr->offset = attr_offsets[binding_idx];

        // The attribute size is specified by the caller in dvz_visual_attr().
        ASSERT(attr->item_size > 0);

        // Keep track of the current offset within each vertex binding.
        attr_offsets[binding_idx] += attr->item_size;

        // Count the number of attributes.
        attr_count++;

        ASSERT(attr_count <= DVZ_MAX_VERTEX_ATTRS);
    }

    log_debug("found %d vertex attributes and %d vertex bindings", attr_count, binding_count);
    ASSERT(attr_count < DVZ_MAX_VERTEX_ATTRS);
    ASSERT(binding_count < DVZ_MAX_VERTEX_BINDINGS);

    // Declare the vertex bindings.
    DvzSize stride = 0;
    for (binding_idx = 0; binding_idx < binding_count; binding_idx++)
    {
        // NOTE: try fetching the user-specified binding stride, or compute it simply
        // as the last attr offset + its size.
        // THIS IS HAZARDOUS because of alignment issues.
        // This may cause the following Vulkan warning, for example:

        // Format VK_FORMAT_R32G32B32_SFLOAT has an alignment of 4 but the alignment of
        // attribAddress (57) is not aligned in pVertexAttributeDescriptions[0] (binding=0
        // location=0) where attribAddress = vertex buffer offset (0) + binding stride (57) +
        // attribute offset (0). The Vulkan spec states: For a given vertex buffer binding, any
        // attribute data fetched must be entirely contained within the corresponding vertex buffer
        // binding, as described in Vertex Input Description
        // (https://vulkan.lunarg.com/doc/view/1.3.216.0/linux/1.3-extensions/vkspec.html#VUID-vkCmdDrawIndexed-None-02721)

        stride = visual->strides[binding_idx];
        stride = stride > 0 ? stride : attr_offsets[binding_idx];
        ASSERT(stride > 0);

        // Baker-side.
        dvz_baker_vertex(baker, binding_idx, stride);

        // GPU-side.
        // TODO: input rate instance?
        dvz_set_vertex(rqr, graphics_id, binding_idx, stride, DVZ_VERTEX_INPUT_RATE_VERTEX);
    }

    // Declare the vertex attributes.
    for (uint32_t attr_idx = 0; attr_idx < attr_count; attr_idx++)
    {
        attr = &visual->attrs[attr_idx];
        ANN(attr);
        ASSERT(attr->item_size > 0);

        // Baker-side.
        dvz_baker_attr(baker, attr_idx, attr->binding_idx, attr->offset, attr->item_size);

        // GPU-side.
        dvz_set_attr(rqr, graphics_id, attr->binding_idx, attr_idx, attr->format, attr->offset);
    }

    // The baker slots are declared directly in dvz_visual_params() and dvz_visual_tex().
    // Now, we can create the baker. This will create the arrays and dats.

    visual->item_count = item_count;

    // NOTE: we declare the common bindings (mvp and viewport) as shared to prevent the baker from
    // handling them. They will be handled by the panel in scene.c.
    // TODO: use #define instead of hard-coded values here
    // dvz_baker_share_binding(baker, 0);
    // dvz_baker_share_binding(baker, 1);

    bool indexed = (visual->flags & DVZ_VISUALS_FLAGS_INDEXED) != 0;

    // NOTE: if index_count is not specified, , item_count is the number of FACES (number of
    // indices / 3).
    index_count = index_count > 0 ? index_count : (indexed ? (item_count * 3) : 0);

    dvz_baker_create(baker, index_count, vertex_count);

    // Bind the index buffer.
    if (indexed)
    {
        dvz_bind_index(rqr, graphics_id, baker->index.dat, 0);
    }

    // We now need to send the vertex/descriptor binding requests to the GPU.

    // Send the vertex binding commands.
    DvzBakerVertex* bv = NULL;
    for (binding_idx = 0; binding_idx < binding_count; binding_idx++)
    {
        bv = &baker->vertex_bindings[binding_idx];
        if (bv->shared)
        {
            log_trace(
                "skip binding of shared vertex binding #%d, it will be handled externally",
                binding_idx);
            continue;
        }
        // TODO: dat offset?
        dvz_bind_vertex(rqr, graphics_id, binding_idx, bv->dual.dat, 0);
    }

    // // Send the dat bindings commands.
    // DvzBakerDescriptor* bd = NULL;
    // for (uint32_t slot_idx = 0; slot_idx < baker->slot_count; slot_idx++)
    // {
    //     bd = &baker->descriptors[slot_idx];
    //     // NOTE: this is only for dat bindings.
    //     if (bd->type != DVZ_SLOT_DAT)
    //         continue;
    //     if (bd->shared)
    //     {
    //         log_trace(
    //             "skip binding of shared descriptor binding #%d, it will be handled externally",
    //             slot_idx);
    //         continue;
    //     }
    //     dvz_bind_dat(rqr, graphics_id, slot_idx, bd->u.dat.dual.dat, 0);
    // }

    // NOTE: when using the scene API (viewset.c), these are handled automatically.
    // But when using visuals directly, in order for dvz_visual_record() to work,
    // we need to set these as sensible defaults.
    visual->draw_first = 0;
    visual->draw_count = item_count;
    visual->first_instance = 0;
    visual->instance_count = 1;

    dvz_obj_created(&visual->obj);
}



/*************************************************************************************************/
/*  Visual common bindings                                                                       */
/*  NOTE: only for tests. Otherwise will use scene API with transforms and viewsets.             */
/*************************************************************************************************/

void dvz_visual_mvp(DvzVisual* visual, DvzMVP* mvp)
{
    // NOTE: the data is immediately copied into the visual's baker's dual

    ANN(visual);
    ANN(visual->baker);

    // if (visual->baker->slot_count == 0)
    // {
    //     log_debug("skipping visual_mvp() for visual with no common bindings");
    //     return;
    // }

    DvzParams* params = dvz_params(visual->rqr, sizeof(DvzMVP), false);
    dvz_visual_params(visual, 0, params);
    dvz_params_data(params, mvp);
}



void dvz_visual_viewport(DvzVisual* visual, DvzViewport* viewport)
{
    // NOTE: the data is immediately copied into the visual's baker's dual

    ANN(visual);
    ANN(visual->baker);

    // if (visual->baker->slot_count <= 1)
    // {
    //     log_debug("skipping visual_viewport() for visual with no common bindings");
    //     return;
    // }

    DvzParams* params = dvz_params(visual->rqr, sizeof(DvzViewport), false);
    dvz_visual_params(visual, 1, params);
    dvz_params_data(params, viewport);
}



/*************************************************************************************************/
/*  Visual data                                                                                  */
/*************************************************************************************************/

void dvz_visual_data(
    DvzVisual* visual, uint32_t attr_idx, uint32_t first, uint32_t count, void* data)
{
    ANN(visual);
    ASSERT(attr_idx < DVZ_MAX_VERTEX_ATTRS);

    DvzBaker* baker = visual->baker;
    ANN(baker);

    int flags = visual->attrs[attr_idx].flags;

    // Quad.
    if ((flags & DVZ_ATTR_FLAGS_QUAD) != 0)
    {
        log_warn("please use dvz_visual_quads() instead");
        return;
    }

    // Repeats.
    else if ((flags & DVZ_ATTR_FLAGS_REPEAT) != 0)
    {
        // Extract the N in 0xN00 part, that is the number of repeats.
        int reps = (flags & 0x0F00) >> 8;
        ASSERT(reps >= 1);

        log_debug("visual data for attr #%d (%d->%d, repeat x%d)", attr_idx, first, count, reps);
        dvz_baker_repeat(baker, attr_idx, first, count, (uint32_t)reps, data);
    }

    // Direct copy.
    else
    {
        log_debug("visual data for attr #%d (%d->%d)", attr_idx, first, count);
        dvz_baker_data(baker, attr_idx, first, count, data);
    }
}



void dvz_visual_quads(
    DvzVisual* visual, uint32_t attr_idx, uint32_t first, uint32_t count, //
    vec2 quad_size, vec2* positions)
{
    ANN(visual);
    ASSERT(attr_idx < DVZ_MAX_VERTEX_ATTRS);

    DvzBaker* baker = visual->baker;
    ANN(baker);

    int flags = visual->attrs[attr_idx].flags;
    ASSERT((flags & DVZ_ATTR_FLAGS_QUAD) != 0);

    dvz_baker_quads(baker, attr_idx, quad_size, count, positions);
}



void dvz_visual_index(DvzVisual* visual, uint32_t first, uint32_t count, DvzIndex* data)
{
    ANN(visual);

    DvzBaker* baker = visual->baker;
    ANN(baker);

    log_debug("visual data for index (%d->%d)", first, count);
    dvz_baker_index(baker, first, count, data);
}



void dvz_visual_param(DvzVisual* visual, uint32_t slot_idx, uint32_t attr_idx, void* item)
{
    ANN(visual);
    ASSERT(slot_idx < DVZ_MAX_BINDINGS);

    DvzParams* params = visual->params[slot_idx];
    ANN(params);

    dvz_params_set(params, attr_idx, item);
}



/*************************************************************************************************/
/*  Visual drawing internal functions                                                            */
/*************************************************************************************************/

void dvz_visual_instance(
    DvzVisual* visual, DvzId canvas, uint32_t first, uint32_t vertex_offset, uint32_t count,
    uint32_t first_instance, uint32_t instance_count)
{
    ANN(visual);

    bool indexed = (visual->flags & DVZ_VISUALS_FLAGS_INDEXED) != 0;
    bool indirect = (visual->flags & DVZ_VISUALS_FLAGS_INDIRECT) != 0;
    ASSERT(!indirect);

    if (indexed)
    {
        dvz_record_draw_indexed(
            visual->rqr, canvas, visual->graphics_id, first, vertex_offset, count, //
            first_instance, instance_count);
    }
    else
    {
        dvz_record_draw(
            visual->rqr, canvas, visual->graphics_id, first, count, //
            first_instance, instance_count);
    }
}



void dvz_visual_indirect(DvzVisual* visual, DvzId canvas, uint32_t draw_count)
{
    ANN(visual);
    ASSERT((visual->flags & DVZ_VISUALS_FLAGS_INDIRECT) != 0);

    DvzBaker* baker = visual->baker;
    ANN(baker);

    DvzId indirect = baker->indirect.dat;
    ASSERT(indirect != DVZ_ID_NONE);

    bool indexed = (visual->flags & DVZ_VISUALS_FLAGS_INDEXED) != 0;

    if (indexed)
    {
        dvz_record_draw_indexed_indirect(
            visual->rqr, canvas, visual->graphics_id, indirect, draw_count);
    }
    else
    {
        dvz_record_draw_indirect(visual->rqr, canvas, visual->graphics_id, indirect, draw_count);
    }
}



void dvz_visual_record(DvzVisual* visual, DvzId canvas)
{
    ANN(visual);
    ASSERT(visual->draw_count > 0);

    // Call the draw callback if there is one.
    if (visual->callback != NULL)
    {
        visual->callback(
            visual, canvas, visual->draw_first, visual->draw_count, //
            visual->first_instance, visual->instance_count);
    }

    // Otherwise call the default callback.
    else
    {
        dvz_visual_instance(
            visual, canvas, visual->draw_first, 0, visual->draw_count, //
            visual->first_instance, visual->instance_count);
    }
}



void dvz_visual_callback(DvzVisual* visual, DvzVisualCallback callback)
{
    ANN(visual);
    ANN(callback);

    visual->callback = callback;
}



/*************************************************************************************************/
/*  Visual drawing                                                                               */
/*************************************************************************************************/

void dvz_visual_visible(DvzVisual* visual, bool is_visible)
{
    ANN(visual);
    visual->is_visible = is_visible;
}
