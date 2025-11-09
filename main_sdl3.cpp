#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vector>

int main()
{
    SDL_SetHint("SDL_VIDEO_WAYLAND_PREFER_LIBDECOR", "1");

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init failed: %s", SDL_GetError());
        return 1;
    }
    SDL_Log("Video driver: %s", SDL_GetCurrentVideoDriver());

    Uint32 flags = SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE;
    SDL_Window *win = SDL_CreateWindow("PixL", 1600, 900, flags);
    if (!win)
    {
        SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "SDL_CreateWindow failed: %s", SDL_GetError());
        return 2;
    }

    SDL_Renderer *r = SDL_CreateRenderer(win, NULL);
    if (r)
    {
        SDL_SetRenderDrawColor(r, 10, 10, 20, 255);
        SDL_RenderClear(r);
        SDL_RenderPresent(r);
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_RENDER, "No SDL_Renderer (ok si tu fais Vulkan ensuite): %s", SDL_GetError());
    }

    bool running = true;
    Uint64 t0 = SDL_GetTicks();
    while (running)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_EVENT_QUIT)
                running = false;
        }
        if (SDL_GetTicks() - t0 > 5000)
            running = false;
        SDL_Delay(16);
    }

    if (r)
        SDL_DestroyRenderer(r);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
