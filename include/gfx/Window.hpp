#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <functional>
#include <string>

namespace pixl::gfx
{
    class Window
    {
    public:
        Window(const std::string &title, int width, int height, uint32_t flags);
        ~Window();

        void PollEvents(const std::function<void(const SDL_Event &)> &onEvent = {});
        bool ShouldClose() const { return shouldClose_; }

        SDL_Window *GetSDLWindow() const { return sdlWindow_; }

    private:
        SDL_Window *sdlWindow_;
        bool shouldClose_;
    };
}
