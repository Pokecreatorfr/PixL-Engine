#include <PixL_Renderer.hpp>

PixL_Renderer::PixL_Renderer()
{
    // init SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
    }
    else
    {
        // init SDL_image
        int imgFlags = IMG_INIT_PNG;
        if (!(IMG_Init(imgFlags) & imgFlags))
        {
            std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        }
    }
}

PixL_Renderer::~PixL_Renderer()
{
    // quit SDL_image
    IMG_Quit();

    // quit SDL
    SDL_Quit();
}

PixL_Renderer *PixL_Renderer::_instance = nullptr;

int PixL_Renderer_Init(uint32_t flags)
{
    // flags
    bool queue_mode = flags & PIXL_RENDERER_QUEUE_MODE;
    bool plan_mode = flags & PIXL_RENDERER_PLAN_MODE;
    bool plan_mode_auto_clear = flags & PIXL_RENDERER_PLAN_MODE_AUTO_CLEAR;
    bool plan_mode_manual_clear = flags & PIXL_RENDERER_PLAN_MODE_MANUAL_CLEAR;

    if (queue_mode && plan_mode)
    {
        return -1;
    }

    if (plan_mode_auto_clear && plan_mode_manual_clear)
    {
        return -1;
    }

    if (PixL_Renderer::_instance == nullptr)
    {
        PixL_Renderer::_instance = new PixL_Renderer();
    }
    PixL_Renderer::_instance->_flags = flags;
    return 0;
}

SDL_Window *CreateWindow(const char *title, int x, int y, int w, int h, Uint32 flags)
{
    if (PixL_Renderer::_instance == nullptr || PixL_Renderer::_instance->_window != nullptr)
    {
        std::cerr << "Renderer not initialized or window already created!" << std::endl;
        return nullptr;
    }

    PixL_Renderer::_instance->_window = SDL_CreateWindow(title, x, y, w, h, flags);
    if (PixL_Renderer::_instance->_window == nullptr)
    {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    }
    return PixL_Renderer::_instance->_window;
}

SDL_Window *GetWindow()
{
    if (PixL_Renderer::_instance == nullptr)
    {
        return nullptr;
    }

    return PixL_Renderer::_instance->_window;
}

PixL_Texture *CreateTexture(const char *path)
{
    if (PixL_Renderer::_instance == nullptr || PixL_Renderer::_instance->_renderer == nullptr)
    {
        std::cerr << "Renderer not initialized or renderer not created!" << std::endl;
        return nullptr;
    }

    SDL_Texture *texture = IMG_LoadTexture(PixL_Renderer::_instance->_renderer, path);
    if (texture == nullptr)
    {
        std::cerr << "Texture could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return nullptr;
    }

    PixL_Texture *pixl_texture = new PixL_Texture();
}

uint16_t PixL_Tilemap::_GetMapHeight()
{
    return _map_height;
}

uint16_t PixL_Tilemap::_GetMapWidth()
{
    return _map_width;
}

void PixL_Tilemap::_SetMapHeight(uint16_t height)
{
    _map_height = height;
    _map.resize(_map_width * _map_height);
    std::fill(_map.begin(), _map.end(), 0);
}

void PixL_Tilemap::_SetMapWidth(uint16_t width)
{
    _map_width = width;
    _map.resize(_map_width * _map_height);
    std::fill(_map.begin(), _map.end(), 0);
}

std::vector<uint16_t> *PixL_Tilemap::_GetTilemap()
{
    return &_map;
}
