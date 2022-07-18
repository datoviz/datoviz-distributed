/*************************************************************************************************/
/*  Dear ImGUI wrapper                                                                           */
/*************************************************************************************************/

#include "gui.h"
#include "canvas.h"
#include "host.h"
#include "vklite.h"
#include "window.h"

// ImGUI includes
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include "imgui.h"



/*************************************************************************************************/
/*  Structs                                                                                      */
/*************************************************************************************************/



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

    ASSERT(gpu != NULL);

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

    // TODO
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
    ASSERT(ImGui::GetCurrentContext() != NULL);

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
        // ASSERT(buffer != NULL);
        // ImFontAtlas_AddFontDefault(io.Fonts, NULL);
        // font = ImFontAtlas_AddFontFromMemoryTTF(
        //     io.Fonts, buffer, file_size, font_size, &config, NULL);
        // ASSERT(font != NULL);
        // ASSERT(ImFont_IsLoaded(font));
    }


    DvzCommands cmd = dvz_commands(gpu, 0, 1);
    dvz_cmd_begin(&cmd, 0);
    ImGui_ImplVulkan_CreateFontsTexture(cmd.cmds[0]);
    dvz_cmd_end(&cmd, 0);
    dvz_cmd_submit_sync(&cmd, 0);

    ImGui_ImplVulkan_DestroyFontUploadObjects();
    dvz_commands_destroy(&cmd);
}



static void _imgui_set_window(DvzWindow* window)
{
    ASSERT(window != NULL);

    DvzBackend backend = window->backend;
    ASSERT(backend != DVZ_BACKEND_NONE);
    switch (backend)
    {
    case DVZ_BACKEND_GLFW:
        if (window->backend_window)
            ImGui_ImplGlfw_InitForVulkan((GLFWwindow*)window->backend_window, true);
        break;
    default:
        break;
    }

    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize.x = window->width;
    io.DisplaySize.y = window->height;
}



static DvzRenderpass _imgui_renderpass(DvzGpu* gpu)
{
    ASSERT(gpu != NULL);
    INIT(DvzRenderpass, renderpass)

    // Color attachment.
    dvz_renderpass_attachment(
        &renderpass, 0, //
        DVZ_RENDERPASS_ATTACHMENT_COLOR, (VkFormat)DVZ_DEFAULT_FORMAT,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    dvz_renderpass_attachment_layout(
        &renderpass, 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    dvz_renderpass_attachment_ops(
        &renderpass, 0, VK_ATTACHMENT_LOAD_OP_LOAD, VK_ATTACHMENT_STORE_OP_STORE);

    // Subpass.
    dvz_renderpass_subpass_attachment(&renderpass, 0, 0);

    return renderpass;
}



static DvzFramebuffers
_imgui_framebuffers(DvzGpu* gpu, DvzRenderpass* renderpass, DvzImages* images)
{
    ASSERT(gpu != NULL);
    INIT(DvzFramebuffers, framebuffers)

    framebuffers = dvz_framebuffers(gpu);
    dvz_framebuffers_attachment(&framebuffers, 0, images);
    dvz_framebuffers_create(&framebuffers, renderpass);

    return framebuffers;
}



static void _imgui_destroy()
{
    ImGui_ImplVulkan_Shutdown();
    if (_imgui_has_glfw())
        ImGui_ImplGlfw_Shutdown();
    ASSERT(ImGui::GetCurrentContext() != NULL);
    ImGui::DestroyContext(ImGui::GetCurrentContext());
    ASSERT(ImGui::GetCurrentContext() == NULL);
}



/*************************************************************************************************/
/*  Functions                                                                                    */
/*************************************************************************************************/

DvzGui* dvz_gui(DvzGpu* gpu, uint32_t queue_idx)
{
    ASSERT(gpu != NULL);

    if (_imgui_has_context())
    {
        log_warn("GUI context already created, skipping");
        return NULL;
    }
    log_debug("initialize the Dear ImGui context");

    DvzGui* gui = (DvzGui*)calloc(1, sizeof(DvzGui));
    gui->gpu = gpu;
    gui->renderpass = _imgui_renderpass(gpu);

    _imgui_init(gpu, queue_idx, &gui->renderpass);
    _imgui_setup();
    _imgui_fonts_upload(gpu);
    return gui;
}



void dvz_gui_frame_offscreen(uint32_t width, uint32_t height)
{
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize.x = width;
    io.DisplaySize.y = height;

    ImGui_ImplVulkan_NewFrame();
    ImGui::NewFrame();
}



DvzGuiWindow* dvz_gui_window(DvzGui* gui, DvzWindow* window, DvzImages* images)
{
    // NOTE: window is optional (offscreen tests)
    // NOTE: glfw is the only supported backend for now

    ASSERT(gui != NULL);
    ASSERT(window != NULL);
    ASSERT(!window || window->gui_window == NULL); // Only set it once.

    DvzGpu* gpu = gui->gpu;
    ASSERT(gpu != NULL);

    DvzGuiWindow* gui_window = (DvzGuiWindow*)calloc(1, sizeof(DvzGuiWindow));
    gui_window->gui = gui;
    gui_window->window = window;

    // Initialize the list of callbacks.
    gui_window->callbacks = dvz_list();

    // Create the command buffers.
    gui_window->cmds = dvz_commands(gpu, DVZ_DEFAULT_QUEUE_RENDER, images->count);

    // Create the framebuffers.
    gui_window->framebuffers = _imgui_framebuffers(gpu, &gui->renderpass, images);

    // window->gui_window = gui_window;
    if (window != NULL)
        _imgui_set_window(window);
    return gui_window;
}



void dvz_gui_frame_begin(DvzGui* gui, DvzWindow* window)
{
    ASSERT(gui != NULL);
    ASSERT(!window || window->gui_window != NULL);

    ImGui_ImplVulkan_NewFrame();
    if (window)
        ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}



void dvz_gui_dialog_begin(vec2 pos, vec2 size)
{
    // const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(pos[0], pos[1]), ImGuiCond_FirstUseEver, ImVec2(0, 0));
    // (ImVec2){main_viewport->WorkPos.x, main_viewport->WorkPos.y},
    ImGui::SetNextWindowSize(ImVec2(size[0], size[1]), ImGuiCond_FirstUseEver);

    bool open = true;
    // ImGui::PushFont(font);
    ImGui::Begin("Dialog", &open, ImGuiWindowFlags_NoSavedSettings);
}



void dvz_gui_text(const char* str) { ImGui::Text(str); }



void dvz_gui_dialog_end() { ImGui::End(); }



void dvz_gui_demo()
{
    bool open = true;
    ImGui::ShowDemoWindow(&open);
}



void dvz_gui_frame_end(DvzCommands* cmds, uint32_t idx)
{
    ASSERT(cmds != NULL);
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmds->cmds[idx], VK_NULL_HANDLE);
}



void dvz_gui_window_destroy(DvzGuiWindow* gui_window)
{
    ASSERT(gui_window != NULL);
    dvz_list_destroy(gui_window->callbacks);
    dvz_framebuffers_destroy(&gui_window->framebuffers);
    FREE(gui_window);
}



void dvz_gui_destroy(DvzGui* gui)
{
    ASSERT(gui != NULL);
    _imgui_destroy();
    FREE(gui);
}
