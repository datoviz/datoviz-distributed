/*************************************************************************************************/
/*  Dear ImGUI wrapper                                                                           */
/*************************************************************************************************/

#include <stdarg.h>

#include "canvas.h"
#include "gui.h"
#include "host.h"
#include "resources.h"
#include "vklite.h"
#include "window.h"

// ImGUI includes
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include "imgui.h"



/*************************************************************************************************/
/*  Utils functions                                                                              */
/*************************************************************************************************/

static inline void _imgui_check_vk_result(VkResult err)
{
    if (err == 0)
        return;
    log_error("VkResult %d\n", err);
    if (err < 0)
        abort();
}



static inline bool _imgui_has_context() { return ImGui::GetCurrentContext() != NULL; }



static inline bool _imgui_has_glfw()
{
    return ImGui::GetCurrentContext() && ImGui::GetIO().BackendPlatformUserData != NULL;
}



static void _imgui_init(DvzGpu* gpu, uint32_t queue_idx, DvzRenderpass* renderpass)
{
    ASSERT(!_imgui_has_context());
    log_debug("initialize the Dear ImGui context");

    ANN(gpu);

    ImGui::DebugCheckVersionAndDataLayout(
        IMGUI_VERSION, sizeof(ImGuiIO), sizeof(ImGuiStyle), sizeof(ImVec2), sizeof(ImVec4),
        sizeof(ImDrawVert), sizeof(ImDrawIdx));
    ImGui::CreateContext(NULL);
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = NULL;

    INIT(ImGui_ImplVulkan_InitInfo, init_info)
    init_info.Instance = gpu->host->instance;
    init_info.PhysicalDevice = gpu->physical_device;
    init_info.Device = gpu->device;
    // should be the render queue idx
    init_info.QueueFamily = gpu->queues.queue_families[queue_idx];
    init_info.Queue = gpu->queues.queues[queue_idx];
    init_info.DescriptorPool = gpu->dset_pool;
    // init_info.PipelineCache = gpu->pipeline_cache;
    // init_info.Allocator = gpu->allocator;

    // TODO: better selection of image count (from Vulkan instead of hard-coded)
    init_info.MinImageCount = 2;
    init_info.ImageCount = 2;

    init_info.CheckVkResultFn = _imgui_check_vk_result;

    ASSERT(renderpass->renderpass != VK_NULL_HANDLE);
    ImGui_ImplVulkan_Init(&init_info, renderpass->renderpass);
}



static void _imgui_setup()
{
    // ImGuiIO* io = ImGui::GetIO();
    // int flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar |
    //             ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav
    //             | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNavInputs |
    //             ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
    //             ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing;
    // ImGui::SetNextWindowBgAlpha(0.5f);

    // float distance = 0;
    // int corner = 0;
    // ASSERT(corner >= 0);
    // ImVec2 window_pos = (ImVec2){
    //     (corner & 1) ? io.DisplaySize.x - distance : distance,
    //     (corner & 2) ? io.DisplaySize.y - distance : distance};
    // ImVec2 window_pos_pivot = (ImVec2){(corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f};
    // ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
}



static void _imgui_fonts_upload(DvzGpu* gpu)
{
    ANN(ImGui::GetCurrentContext());

    log_trace("uploading Dear ImGui fonts");

    // Load Fonts.
    // Load first font.
    {
        // float font_size = 14.0f;
        // ASSERT(font_size > 0);
        // ImFontConfig config = {0};
        // config.FontDataOwnedByAtlas = false; // Important!
        // ImGuiIO* io = ImGui::GetIO();
        // unsigned long file_size = 0;
        // unsigned char* buffer = dvz_resource_font("Roboto_Medium", &file_size);
        // ASSERT(file_size > 0);
        // ANN(buffer);
        // ImFontAtlas_AddFontDefault(io.Fonts, NULL);
        // font = ImFontAtlas_AddFontFromMemoryTTF(
        //     io.Fonts, buffer, file_size, font_size, &config, NULL);
        // ANN(font);
        // ASSERT(ImFont_IsLoaded(font));
    }

    // NOTE: not the TRANSFER queue, otherwise the following warning occurs:
    // dstStageMask flag VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT is not compatible with the queue
    // family properties (VK_QUEUE_TRANSFER_BIT|VK_QUEUE_SPARSE_BINDING_BIT) of this command
    // buffer.
    // HACK: we may use this code when the DVZ_DEFAULT_QUEUE_RENDER is not defined, ie when there
    // is a single queue (offscreen tests, for example).
    DvzCommands cmd =
        dvz_commands(gpu, MIN(((int)DVZ_DEFAULT_QUEUE_RENDER), (gpu->queues.queue_count - 1)), 1);
    dvz_cmd_begin(&cmd, 0);
    ImGui_ImplVulkan_CreateFontsTexture(cmd.cmds[0]);
    dvz_cmd_end(&cmd, 0);
    dvz_cmd_submit_sync(&cmd, 0);

    ImGui_ImplVulkan_DestroyFontUploadObjects();
    dvz_commands_destroy(&cmd);
}



static void _imgui_set_window(DvzWindow* window)
{
    ANN(window);

    DvzBackend backend = window->backend;
    ASSERT(backend != DVZ_BACKEND_NONE);
    switch (backend)
    {
    case DVZ_BACKEND_GLFW:
        if (window->backend_window != NULL)
        {
            ImGui_ImplGlfw_InitForVulkan((GLFWwindow*)window->backend_window, true);
        }
        break;
    default:
        break;
    }
}



static DvzRenderpass _imgui_renderpass(DvzGpu* gpu, bool offscreen)
{
    ANN(gpu);

    log_trace("create Dear ImGui renderpass");

    DvzRenderpass renderpass = dvz_renderpass(gpu);

    // Color attachment.
    dvz_renderpass_attachment(
        &renderpass, 0, //
        DVZ_RENDERPASS_ATTACHMENT_COLOR, (VkFormat)DVZ_DEFAULT_FORMAT,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    dvz_renderpass_attachment_layout(
        &renderpass, 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        offscreen ? VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    dvz_renderpass_attachment_ops(
        &renderpass, 0, VK_ATTACHMENT_LOAD_OP_LOAD, VK_ATTACHMENT_STORE_OP_STORE);

    // Subpass.
    dvz_renderpass_subpass_attachment(&renderpass, 0, 0);

    dvz_renderpass_create(&renderpass);

    return renderpass;
}



static void _imgui_framebuffers(
    DvzGpu* gpu, DvzRenderpass* renderpass, DvzImages* images, DvzFramebuffers* framebuffers)
{
    ANN(gpu);
    ANN(renderpass);
    ANN(images);

    log_trace("creating Dear ImGui framebuffers");

    *framebuffers = dvz_framebuffers(gpu);
    dvz_framebuffers_attachment(framebuffers, 0, images);
    dvz_framebuffers_create(framebuffers, renderpass);
}



static int _imgui_styling(int flags)
{
    const ImGuiIO& io = ImGui::GetIO();
    int imgui_flags = ImGuiWindowFlags_NoSavedSettings;

    if ((flags & DVZ_DIALOG_FLAGS_FPS) != 0)
    {
        imgui_flags |= ImGuiWindowFlags_NoTitleBar |        //
                       ImGuiWindowFlags_NoScrollbar |       //
                       ImGuiWindowFlags_NoResize |          //
                       ImGuiWindowFlags_NoCollapse |        //
                       ImGuiWindowFlags_NoNav |             //
                       ImGuiWindowFlags_NoNavInputs |       //
                       ImGuiWindowFlags_NoDecoration |      //
                       ImGuiWindowFlags_NoMove |            //
                       ImGuiWindowFlags_NoSavedSettings |   //
                       ImGuiWindowFlags_NoFocusOnAppearing; //
        ImGui::SetNextWindowBgAlpha(0.5f);

        // 0 = TL, 1 = TR, 2 = LL, 3 = LR
        // NOTE: by default, always top right
        int corner = 1;
        float distance = 0;
        ImVec2 window_pos = ImVec2(
            (corner & 1) ? io.DisplaySize.x - distance : distance,
            (corner & 2) ? io.DisplaySize.y - distance : distance);
        ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
    }

    return imgui_flags;
}



static void _imgui_destroy()
{
    ImGui_ImplVulkan_Shutdown();
    if (_imgui_has_glfw())
        ImGui_ImplGlfw_Shutdown();
    ANN(ImGui::GetCurrentContext());
    ImGui::DestroyContext(ImGui::GetCurrentContext());
    ASSERT(ImGui::GetCurrentContext() == NULL);
}



/*************************************************************************************************/
/*  GUI functions                                                                                */
/*************************************************************************************************/

DvzGui* dvz_gui(DvzGpu* gpu, uint32_t queue_idx, int flags)
{
    ANN(gpu);

    if (_imgui_has_context())
    {
        log_warn("GUI context already created, skipping");
        return NULL;
    }
    log_debug("initialize the Dear ImGui context");

    DvzGui* gui = (DvzGui*)calloc(1, sizeof(DvzGui));
    gui->gpu = gpu;

    gui->gui_windows = dvz_container(
        DVZ_CONTAINER_DEFAULT_COUNT, sizeof(DvzGuiWindow), DVZ_OBJECT_TYPE_GUI_WINDOW);

    gui->renderpass = _imgui_renderpass(gpu, flags == DVZ_GUI_FLAGS_OFFSCREEN);
    ASSERT(dvz_obj_is_created(&gui->renderpass.obj));

    _imgui_init(gpu, queue_idx, &gui->renderpass);
    _imgui_setup();
    _imgui_fonts_upload(gpu);
    return gui;
}



void dvz_gui_destroy(DvzGui* gui)
{
    ANN(gui);

    // Destroy the GUI windows.
    CONTAINER_DESTROY_ITEMS(DvzGuiWindow, gui->gui_windows, dvz_gui_window_destroy)
    dvz_container_destroy(&gui->gui_windows);

    dvz_renderpass_destroy(&gui->renderpass);

    // NOTE: this must occur BEFORE backend_destroy(), as imgui will unregister the callbacks using
    // glfw functions.
    _imgui_destroy();

    FREE(gui);
}



/*************************************************************************************************/
/*  GUI window                                                                                   */
/*************************************************************************************************/

DvzGuiWindow* dvz_gui_window(DvzGui* gui, DvzWindow* window, DvzImages* images, uint32_t queue_idx)
{
    // NOTE: window is optional (offscreen tests)
    // NOTE: glfw is the only supported backend for now

    ANN(gui);
    ANN(window);
    ANN(images);

    ASSERT(!window || window->gui_window == NULL); // Only set it once.
    ASSERT(images->count > 0);

    DvzGpu* gpu = gui->gpu;
    ANN(gpu);

    DvzGuiWindow* gui_window = (DvzGuiWindow*)dvz_container_alloc(&gui->gui_windows);
    gui_window->gui = gui;
    gui_window->window = window;

    // GUI window width and height relate to the framebuffer, not the window size.
    gui_window->width = images->shape[0];
    gui_window->height = images->shape[1];

    gui_window->is_offscreen = false;

    // Create the command buffers.
    gui_window->cmds = dvz_commands(gpu, queue_idx, images->count);

    // Create the framebuffers.
    _imgui_framebuffers(gpu, &gui->renderpass, images, &gui_window->framebuffers);

    if (window->gui_window == NULL)
        _imgui_set_window(window);
    window->gui_window = gui_window;

    dvz_obj_created(&gui_window->obj);
    return gui_window;
}



DvzGuiWindow* dvz_gui_offscreen(DvzGui* gui, DvzImages* images, uint32_t queue_idx)
{
    ANN(gui);
    ANN(images);

    DvzGpu* gpu = gui->gpu;
    ANN(gpu);

    DvzGuiWindow* gui_window = (DvzGuiWindow*)dvz_container_alloc(&gui->gui_windows);
    gui_window->gui = gui;

    // GUI window width and height relate to the framebuffer, not the window size.
    gui_window->width = images->shape[0];
    gui_window->height = images->shape[1];

    gui_window->is_offscreen = true;

    // Create the command buffers.
    gui_window->cmds = dvz_commands(gpu, queue_idx, 1);

    // Create the framebuffers.
    _imgui_framebuffers(gpu, &gui->renderpass, images, &gui_window->framebuffers);

    dvz_obj_created(&gui_window->obj);
    return gui_window;
}



void dvz_gui_window_begin(DvzGuiWindow* gui_window, uint32_t idx)
{
    ANN(gui_window);

    DvzCommands* cmds = &gui_window->cmds;
    ANN(cmds);

    DvzGui* gui = gui_window->gui;
    ANN(gui);

    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize.x = gui_window->width;
    io.DisplaySize.y = gui_window->height;

    ImGui_ImplVulkan_NewFrame();

    if (!gui_window->is_offscreen)
        ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();

    dvz_cmd_begin(cmds, idx);
    dvz_cmd_begin_renderpass(cmds, idx, &gui->renderpass, &gui_window->framebuffers);
}



void dvz_gui_window_end(DvzGuiWindow* gui_window, uint32_t idx)
{
    ANN(gui_window);

    DvzCommands* cmds = &gui_window->cmds;
    ANN(cmds);

    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmds->cmds[idx], VK_NULL_HANDLE);

    dvz_cmd_end_renderpass(cmds, idx);
    dvz_cmd_end(cmds, idx);
}



void dvz_gui_window_resize(DvzGuiWindow* gui_window, uint32_t width, uint32_t height)
{
    ANN(gui_window);
    gui_window->width = width;
    gui_window->height = height;

    DvzGui* gui = gui_window->gui;
    ANN(gui);

    // Recreate the framebuffers.
    dvz_framebuffers_destroy(&gui_window->framebuffers);
    dvz_framebuffers_create(&gui_window->framebuffers, &gui->renderpass);
}



void dvz_gui_window_destroy(DvzGuiWindow* gui_window)
{
    ANN(gui_window);
    dvz_framebuffers_destroy(&gui_window->framebuffers);
    dvz_obj_destroyed(&gui_window->obj);
}



/*************************************************************************************************/
/*  DearImGui Wrappers                                                                           */
/*************************************************************************************************/

void dvz_gui_dialog_begin(const char* title, vec2 pos, vec2 size, int flags)
{
    ANN(title);

    // WARNING: the title should be unique for each different dialog!
    ImGui::SetNextWindowPos(ImVec2(pos[0], pos[1]), ImGuiCond_FirstUseEver, ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(size[0], size[1]), ImGuiCond_FirstUseEver);

    int imgui_flags = _imgui_styling(flags);

    bool open = true;
    ImGui::Begin(title, &open, imgui_flags);
}



void dvz_gui_text(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    ImGui::TextV(fmt, args);
    va_end(args);
}



void dvz_gui_image(DvzTex* tex, float width, float height)
{
    ANN(tex);

    ASSERT(tex->dims == DVZ_TEX_2D);

    // HACK: create a Vulkan descriptor set for ImGui.
    if (tex->_imgui_texid == VK_NULL_HANDLE)
    {
        DvzSampler* sampler = dvz_resources_sampler(
            tex->res, DVZ_FILTER_NEAREST, DVZ_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);

        tex->_imgui_texid = ImGui_ImplVulkan_AddTexture(
            sampler->sampler, tex->img->image_views[0], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
    ASSERT(tex->_imgui_texid != VK_NULL_HANDLE);

    ImVec2 uv_min = ImVec2(0.0f, 0.0f);               // Top-left
    ImVec2 uv_max = ImVec2(1.0f, 1.0f);               // Lower-right
    ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // No tint
    // ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white

    ImGui::Image((ImTextureID)tex->_imgui_texid, ImVec2(width, height), uv_min, uv_max, tint_col);
}



void dvz_gui_dialog_end() { ImGui::End(); }



void dvz_gui_demo()
{
    bool open = true;
    ImGui::ShowDemoWindow(&open);
}
