#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

int main()
{
    SDL_SetLogPriorities(SDL_LOG_PRIORITY_VERBOSE);

    SDL_Log("Available video drivers:");
    for (int i = 0; i < SDL_GetNumVideoDrivers(); ++i)
        SDL_Log("  - %s", SDL_GetVideoDriver(i));
    SDL_Log("Env: XDG_SESSION_TYPE=%s DISPLAY=%s WAYLAND_DISPLAY=%s SDL_VIDEODRIVER=%s",
            SDL_getenv("XDG_SESSION_TYPE"),
            SDL_getenv("DISPLAY"),
            SDL_getenv("WAYLAND_DISPLAY"),
            SDL_getenv("SDL_VIDEODRIVER"));

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "SDL_Init failed: %s", SDL_GetError());
        return -1;
    }

    SDL_Log("Current video driver after SDL_Init: %s", SDL_GetCurrentVideoDriver());

    SDL_Window *win = SDL_CreateWindow("PixL", 1600, 900, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
    if (!win)
    {
        SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "SDL_CreateWindow failed: %s", SDL_GetError());
        return -1;
    }

    uint32_t extCount = 0;
    if (!SDL_Vulkan_GetInstanceExtensions(&extCount) || extCount == 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "SDL_Vulkan_GetInstanceExtensions failed: %s", SDL_GetError());
        return -1;
    }

    SDL_Log("SDL init OK, window OK, Vulkan extensions count = %u", extCount);
    return 0;
}
