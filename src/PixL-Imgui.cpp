#include <modules/PixL-Imgui.hpp>

PixL_Imgui::PixL_Imgui()
{
    if ((PIXL_LOADED_MODULES & PIXL_MODULE_IMGUI) == 0)
    {
        PIXL_LOADED_MODULES |= PIXL_MODULE_IMGUI;
        std::cout << "PixL Imgui module loaded." << std::endl;
    }

    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts; // Enable DPI scaling for fonts

    ImGui::StyleColorsDark();
    float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    ImGuiStyle &style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale); // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    window = GetWindow();
    gpu_device = PixL_GetDevice();

    ImGui_ImplSDL3_InitForSDLGPU(window);
    ImGui_ImplSDLGPU3_InitInfo init_info = {};
    init_info.Device = gpu_device;
    init_info.ColorTargetFormat = SDL_GetGPUSwapchainTextureFormat(gpu_device, window);
    init_info.MSAASamples = SDL_GPU_SAMPLECOUNT_1;
    ImGui_ImplSDLGPU3_Init(&init_info);
}

PixL_Imgui::~PixL_Imgui()
{
    ImGui_ImplSDL3_Shutdown();
    ImGui_ImplSDLGPU3_Shutdown();
    ImGui::DestroyContext();
}

PixL_Imgui *PixL_Imgui::_instance = nullptr;

void PixL_Imgui_Update_Callbacks()
{
    ImGui_ImplSDLGPU3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    for (const auto &callback : PixL_Imgui::getInstance().callbacks)
    {
        const Imgui_Callback &cb = callback.second;
        if (cb.func)
        {
            cb.func(cb.user_data);
        }
    }

    ImGui::Render();
    ImDrawData *draw_data = ImGui::GetDrawData();
    const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);

    SDL_Window *window = GetWindow();

    ImGui_ImplSDLGPU3_PrepareDrawData(draw_data, PixL_GetCommandBuffer());

    PixL_StartRenderPass("", "", false);
    ImGui_ImplSDLGPU3_RenderDrawData(draw_data, PixL_GetCommandBuffer(), PixL_GetRenderPass());
    PixL_EndRenderPass();
}