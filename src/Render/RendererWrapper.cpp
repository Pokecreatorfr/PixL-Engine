#include <Render/RendererWrapper.hpp>

RendererWrapper::RendererWrapper(RendererType type)
{
    this->renderer_type = type;
    switch (type)
    {
    case RendererType::SDL2:
        this->renderer = new SDL2Renderer();
        break;
    case RendererType::Vulkan:
        break;
    case RendererType::OpenGL:
        break;
    case RendererType::DirectX:
        break;
    }
}

RendererWrapper::~RendererWrapper()
{
}

void RendererWrapper::init()
{
    this->renderer->init(this->window);
}

SDL_Window *RendererWrapper::create_window(std::string title, int width, int height)
{
    SDL_Init(SDL_INIT_VIDEO);

    this->window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);

    return this->window;
}

bool RendererWrapper::destroy_window()
{
    SDL_DestroyWindow(this->window);
    return true;
}
