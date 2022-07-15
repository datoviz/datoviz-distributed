/*************************************************************************************************/
/*  Presenter                                                                                    */
/*************************************************************************************************/

#include "../include/datoviz/presenter.h"
#include "../include/datoviz/canvas.h"
#include "../include/datoviz/map.h"
#include "../include/datoviz/surface.h"
#include "../include/datoviz/vklite.h"
#include "canvas_utils.h"
#include "client_utils.h"
#include "vklite_utils.h"



/*************************************************************************************************/
/*  Constants                                                                                    */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Utils                                                                                        */
/*************************************************************************************************/

static void _process_record_requests(DvzRenderer* rd, DvzCanvas* canvas, uint32_t img_idx)
{
    ASSERT(rd != NULL);
    ASSERT(canvas != NULL);

    // Blank canvas by default.
    if (rd->req_count == 0)
    {
        log_info("default command buffer refill with blank canvas for image #%d", img_idx);
        blank_commands(canvas, &canvas->cmds, img_idx, canvas->refill_user_data);
        return;
    }

    log_info("fill command buffer from the saved record requests for image #%d", img_idx);

    // Otherwise, process all buffered commands.
    DvzRequest* rq = NULL;
    DvzPipe* pipe = NULL;
    for (uint32_t i = 0; i < rd->req_count; i++)
    {
        rq = &rd->reqs[i];
        ASSERT(rq != NULL);
        switch (rq->type)
        {

        case DVZ_REQUEST_OBJECT_BEGIN:
            dvz_cmd_reset(&canvas->cmds, img_idx);
            dvz_canvas_begin(canvas, &canvas->cmds, img_idx);
            break;

        case DVZ_REQUEST_OBJECT_VIEWPORT:
            dvz_canvas_viewport(
                canvas, &canvas->cmds, img_idx, //
                rq->content.record_viewport.offset, rq->content.record_viewport.shape);
            break;

        case DVZ_REQUEST_OBJECT_DRAW:

            pipe = (DvzPipe*)dvz_map_get(rd->map, rq->content.record_draw.graphics);
            ASSERT(pipe != NULL);

            dvz_pipe_draw(
                pipe, &canvas->cmds, img_idx, //
                rq->content.record_draw.first_vertex, rq->content.record_draw.vertex_count);
            break;

        case DVZ_REQUEST_OBJECT_END:
            dvz_canvas_end(canvas, &canvas->cmds, img_idx);
            break;

        default:
            log_error("unknown record request #%d with type %d", i, rq->type);
            break;
        }
    }
}



static void _fill_canvas(DvzCanvas* canvas, DvzCommands* cmds, uint32_t idx, void* user_data)
{
    ASSERT(canvas != NULL);
    ASSERT(cmds != NULL);

    DvzPresenter* prt = (DvzPresenter*)user_data;
    ASSERT(prt != NULL);

    DvzRenderer* rnd = prt->rnd;
    ASSERT(rnd != NULL);

    // Process the buffered record requests.
    uint32_t img_idx = canvas->render.swapchain.img_idx;
    _process_record_requests(rnd, canvas, img_idx);
}



// This function is called when a CANVAS creation request is received. The renderer independently
// receives the request and creates the object, but the presenter needs to tell the client to
// create an associated window with a surface.
// NOTE: this function must be called AFTER the request has been processed by the renderer.
static void _canvas_request(DvzPresenter* prt, DvzRequest rq)
{
    ASSERT(prt != NULL);

    DvzClient* client = prt->client;
    ASSERT(client != NULL);

    DvzRenderer* rnd = prt->rnd;
    ASSERT(rnd != NULL);

    DvzGpu* gpu = rnd->gpu;
    ASSERT(gpu != NULL);

    DvzHost* host = gpu->host;
    ASSERT(host != NULL);

    switch (rq.action)
    {
    case DVZ_REQUEST_ACTION_CREATE:;

        // When the client receives a REQUEST event with a canvas creation command, it will *also*
        // create a window in the client with the same id and size. The canvas and window will be
        // linked together via a surface.

        // Retrieve the canvas that was just created by the renderer in _requester_callback().
        DvzCanvas* canvas = dvz_renderer_canvas(rnd, rq.id);

        // TODO: canvas.screen_width/height because this is the window size, not the framebuffer
        // size
        uint32_t width = rq.content.canvas.width;
        uint32_t height = rq.content.canvas.height;

        // Create a client window.
        // NOTE: the window's id in the Client matches the canvas's id in the Renderer.
        DvzWindow* window = create_client_window(client, rq.id, width, height, 0);

        // Create a surface (requires the renderer's GPU).
        VkSurfaceKHR surface = dvz_window_surface(host, window);

        // Finally, associate the canvas with the created window surface.
        dvz_canvas_create(canvas, surface);

        // Refill function for the canvas.
        dvz_canvas_refill(canvas, _fill_canvas, (void*)prt);

        break;
    default:
        break;
    }
}



/*************************************************************************************************/
/*  Callbacks                                                                                    */
/*************************************************************************************************/

// This function is called when the Client receives a REQUESTS event. It will route the requests to
// the underlying renderer, and also create associated Client objects such as windows associated to
// canvases.
static void _requester_callback(DvzClient* client, DvzClientEvent ev, void* user_data)
{
    ASSERT(client != NULL);
    ASSERT(user_data != NULL);
    DvzPresenter* prt = (DvzPresenter*)user_data;

    ASSERT(ev.type == DVZ_CLIENT_EVENT_REQUESTS);

    DvzRequester* rqr = (DvzRequester*)ev.content.r.requests;
    ASSERT(rqr != NULL);

    DvzRenderer* rnd = prt->rnd;
    ASSERT(rnd != NULL);

    // Submit the pending requests to the renderer.
    log_debug("renderer processes %d requests", rqr->count);

    // Go through all pending requests.
    for (uint32_t i = 0; i < rqr->count; i++)
    {
        log_trace("renderer processes request #%d", i);
        // Process each request immediately in the renderer.
        dvz_renderer_request(rnd, rqr->requests[i]);

        // CANVAS requests need special care, as the client may need to manage corresponding
        // windows.
        if (rqr->requests[i].type == DVZ_REQUEST_OBJECT_CANVAS)
        {
            _canvas_request(prt, rqr->requests[i]);
        }
        // Here, new canvases have been properly created with an underlying window and surface.
    }
}



static void _frame_callback(DvzClient* client, DvzClientEvent ev, void* user_data)
{
    ASSERT(client != NULL);
    ASSERT(user_data != NULL);
    DvzPresenter* prt = (DvzPresenter*)user_data;

    dvz_presenter_frame(prt, ev.window_id);
}



/*************************************************************************************************/
/*  Presenter                                                                                    */
/*************************************************************************************************/

DvzPresenter* dvz_presenter(DvzRenderer* rnd)
{
    ASSERT(rnd != NULL);
    DvzPresenter* prt = calloc(1, sizeof(DvzPresenter));
    prt->rnd = rnd;
    return prt;
}



void dvz_presenter_frame(DvzPresenter* prt, DvzId window_id)
{
    ASSERT(prt != NULL);

    DvzClient* client = prt->client;
    ASSERT(client != NULL);

    DvzRenderer* rnd = prt->rnd;
    ASSERT(rnd != NULL);

    DvzGpu* gpu = rnd->gpu;
    ASSERT(gpu != NULL);

    DvzHost* host = gpu->host;
    ASSERT(host != NULL);

    DvzContext* ctx = rnd->ctx;
    ASSERT(ctx != NULL);

    // Retrieve the window from its id.
    DvzWindow* window = id2window(client, window_id);
    ASSERT(window != NULL);

    // Retrieve the canvas from its id.
    DvzCanvas* canvas = dvz_renderer_canvas(rnd, window_id);
    ASSERT(canvas != NULL);

    uint64_t frame_idx = client->frame_idx;
    log_trace("frame %d, window #%x", frame_idx, window_id);

    // Swapchain logic.

    DvzSwapchain* swapchain = &canvas->render.swapchain;
    DvzFramebuffers* framebuffers = &canvas->render.framebuffers;
    DvzRenderpass* renderpass = &canvas->render.renderpass;
    DvzFences* fences = &canvas->sync.fences_render_finished;
    DvzFences* fences_bak = &canvas->sync.fences_flight;
    DvzSemaphores* sem_img_available = &canvas->sync.sem_img_available;
    DvzSemaphores* sem_render_finished = &canvas->sync.sem_render_finished;
    DvzCommands* cmds = &canvas->cmds;
    DvzSubmit* submit = &canvas->render.submit;

    // Wait for fence.
    dvz_fences_wait(fences, canvas->cur_frame);

    // We acquire the next swapchain image.
    dvz_swapchain_acquire(swapchain, sem_img_available, canvas->cur_frame, NULL, 0);
    if (swapchain->obj.status == DVZ_OBJECT_STATUS_INVALID)
    {
        dvz_gpu_wait(gpu);
        return;
    }
    // Handle resizing.
    else if (swapchain->obj.status == DVZ_OBJECT_STATUS_NEED_RECREATE)
    {
        log_trace("recreating the swapchain");

        // Wait until the device is ready and the window fully resized.
        // Framebuffer new size.
        dvz_gpu_wait(gpu);
        dvz_window_poll_size(window);

        // Destroy swapchain resources.
        dvz_framebuffers_destroy(framebuffers);
        dvz_images_destroy(&canvas->render.depth);
        dvz_images_destroy(canvas->render.swapchain.images);

        // Recreate the swapchain. This will automatically set the swapchain->images new
        // size.
        dvz_swapchain_recreate(swapchain);
        // Find the new framebuffer size as determined by the swapchain recreation.
        uint32_t width = swapchain->images->shape[0];
        uint32_t height = swapchain->images->shape[1];

        // Need to recreate the depth image with the new size.
        dvz_images_size(&canvas->render.depth, (uvec3){width, height, 1});
        dvz_images_create(&canvas->render.depth);

        // Recreate the framebuffers with the new size.
        ASSERT(framebuffers->attachments[0]->shape[0] == width);
        ASSERT(framebuffers->attachments[0]->shape[1] == height);
        dvz_framebuffers_create(framebuffers, renderpass);

        // Emit a client Resize event.
        dvz_client_event(
            client, (DvzClientEvent){
                        .type = DVZ_CLIENT_EVENT_WINDOW_RESIZE,
                        .window_id = window_id,
                        .content.w.width = width,
                        .content.w.height = height});

        // Need to refill the command buffers.
        for (uint32_t i = 0; i < cmds->count; i++)
        {
            dvz_cmd_reset(cmds, i);
            canvas->refill(canvas, cmds, i, canvas->refill_user_data);
        }
    }
    else
    {
        dvz_fences_copy(fences, canvas->cur_frame, fences_bak, swapchain->img_idx);

        // Reset the Submit instance before adding the command buffers.
        dvz_submit_reset(submit);

        // Then, we submit the cmds on that image
        dvz_submit_commands(submit, cmds);
        dvz_submit_wait_semaphores(
            submit, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, sem_img_available,
            canvas->cur_frame);
        // Once the render is finished, we signal another semaphore.
        dvz_submit_signal_semaphores(submit, sem_render_finished, canvas->cur_frame);
        dvz_submit_send(submit, swapchain->img_idx, fences, canvas->cur_frame);

        // Once the image is rendered, we present the swapchain image.
        dvz_swapchain_present(swapchain, 1, sem_render_finished, canvas->cur_frame);

        canvas->cur_frame = (canvas->cur_frame + 1) % DVZ_MAX_FRAMES_IN_FLIGHT;
    }

    // IMPORTANT: we need to wait for the present queue to be idle, otherwise the GPU hangs
    // when waiting for fences (not sure why). The problem only arises when using different
    // queues for command buffer submission and swapchain present.
    dvz_queue_wait(gpu, DVZ_DEFAULT_QUEUE_PRESENT);

    // HACK: improve this: img_idx depends on the canvas, but this function does not...
    // DUP transfers must be refactored.
    dvz_transfers_frame(&ctx->transfers, 0);

    // TODO:
    // need to go through the pending requests again in the requester (eg those raise in the RESIZE
    // callbacks)?

    // UPFILL: when there is a command refill + data uploads in the same batch, register
    // the cmd buf at the moment when the GPU-blocking upload really occurs
}



void dvz_presenter_client(DvzPresenter* prt, DvzClient* client)
{
    ASSERT(prt != NULL);
    ASSERT(client != NULL);

    prt->client = client;

    // Register a REQUESTS callback which submits pending requests to the renderer.
    dvz_client_callback(
        client, DVZ_CLIENT_EVENT_REQUESTS, DVZ_CLIENT_CALLBACK_SYNC, _requester_callback, prt);

    // Register a FRAME callback which calls dvz_presenter_frame().
    dvz_client_callback(
        client, DVZ_CLIENT_EVENT_FRAME, DVZ_CLIENT_CALLBACK_SYNC, _frame_callback, prt);
}



void dvz_presenter_destroy(DvzPresenter* prt)
{
    ASSERT(prt != NULL);
    FREE(prt);
}
