#include <core/Errors.hpp>
#include <gfx/Window.hpp>

using namespace pixl::gfx;

Window::Window(const std::string &title, int width, int height, uint32_t flags)
    : sdlWindow_(nullptr), shouldClose_(false)
{
    sdlWindow_ = SDL_CreateWindow(title.c_str(),
                                  width,
                                  height,
                                  flags);

    if (!sdlWindow_)
    {
        PIXL_LOG_ERROR(pixl::core::Errc::UnknownError, "gfx::Window", "Failed to create SDL window: {}", SDL_GetError());
        shouldClose_ = true;
    }
}

pixl::gfx::Window::~Window()
{
    if (sdlWindow_)
    {
        SDL_DestroyWindow(sdlWindow_);
        sdlWindow_ = nullptr;
    }
}

void pixl::gfx::Window::PollEvents(const std::function<void(const SDL_Event &)> &onEvent)
{
    if (!sdlWindow_)
    {
        shouldClose_ = true;
        return;
    }

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (onEvent)
        {
            onEvent(event);
        }

        if (event.type == SDL_EVENT_QUIT)
        {
            shouldClose_ = true;
        }
        else if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
        {
            if (event.window.windowID == SDL_GetWindowID(sdlWindow_))
            {
                shouldClose_ = true;
            }
        }
    }
}
