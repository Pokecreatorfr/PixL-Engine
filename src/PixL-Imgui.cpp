#include <modules/PixL-Imgui.hpp>

PixL_Imgui::PixL_Imgui()
{
    if ((PIXL_LOADED_MODULES & PIXL_MODULE_IMGUI) == 0)
    {
        PIXL_LOADED_MODULES |= PIXL_MODULE_IMGUI;
        std::cout << "PixL Imgui module loaded." << std::endl;
    }

    std::cout << "0" << std::endl;

    io = ImGui::GetIO();
    std::cout << "1" << std::endl;

    (void)io;
    std::cout << "2" << std::endl;

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    std::cout << "3" << std::endl;

    ImGui::StyleColorsDark();
    float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    style.ScaleAllSizes(main_scale); // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale; // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

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
}