#include <PixL_Renderer.hpp>

PixL_Renderer::PixL_Renderer()
{
    if (PixL_Renderer::_instance != nullptr)
    {
        std::cerr << "Renderer already initialized!" << std::endl;
        return;
    }

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

int PixL_Renderer::Draw()
{
    if (this->_flags & PIXL_RENDERER_QUEUE_MODE)
    {
        return this->DrawQueueMode();
    }
    else if (this->_flags & PIXL_RENDERER_PLAN_MODE)
    {
        return this->DrawPlanMode();
    }
    return -1;
}

int PixL_Renderer::Present()
{
    SDL_RenderPresent(this->_renderer);
    return 0;
}

int PixL_Renderer::DrawQueueMode()
{
    // get window size
    int w, h;
    SDL_GetWindowSize(this->_window, &w, &h);
    /*
    for (PixL_Drawable *drawable : this->_drawables)
    {
    }*/
    return 0;
}

int PixL_Renderer::DrawPlanMode()
{
    int w, h;
    SDL_GetWindowSize(this->_window, &w, &h);
    SDL_SetRenderDrawColor(this->_renderer, 0, 0, 0, 255);
    SDL_RenderClear(this->_renderer);

    // sort drawables
    std::sort(_drawables.begin(), _drawables.end(), [](PixL_Draw_Command *a, PixL_Draw_Command *b)
              { return a->property._plan < b->property._plan; });

    for (int i = 0; i < _drawables.size(); i++)
    {
        // check if drawable on screen
        if (_drawables[i]->property.x + _drawables[i]->property.w / 2 > 0 && _drawables[i]->property.x - _drawables[i]->property.w / 2 < 1 && _drawables[i]->property.y + _drawables[i]->property.h / 2 > 0 && _drawables[i]->property.y - _drawables[i]->property.h / 2 < 1)
        {
            // check drawable type
            if (_drawables[i]->drawable->getType() == PixL_DrawableType::PIXL_DRAWABLE_TEXTURE)
            {
                draw_texture((PixL_Texture *)_drawables[i]->drawable, _drawables[i]->property, w, h);
            }
            else if (_drawables[i]->drawable->getType() == PixL_DrawableType::PIXL_DRAWABLE_TILEMAP)
            {
                draw_tilemap((PixL_Tilemap *)_drawables[i]->drawable, w, h);
            }
        }
    }
    return 0;
}

int PixL_Renderer::draw_texture(PixL_Texture *texture, PixL_Draw_Property &property, int screenw, int screenh)
{
    SDL_SetTextureBlendMode(texture->_texture, SDL_BLENDMODE_BLEND);
    // set src rect
    SDL_Rect src_rect;
    src_rect.x = (property.src_x - property.src_w / 2) * texture->_width;
    src_rect.y = (property.src_y - property.src_h / 2) * texture->_height;
    src_rect.w = property.src_w * texture->_width;
    src_rect.h = property.src_h * texture->_height;

    // set dst rect
    SDL_Rect dst_rect;
    dst_rect.x = (property.x - property.w / 2) * screenw;
    dst_rect.y = (property.y - property.h / 2) * screenh;
    dst_rect.w = property.w * screenw;
    dst_rect.h = property.h * screenh;

    uint8_t flip = 0;
    if (property.flip_h)
    {
        flip |= SDL_FLIP_HORIZONTAL;
    }
    if (property.flip_v)
    {
        flip |= SDL_FLIP_VERTICAL;
    }
    if (!property.flip_h && !property.flip_v)
    {
        flip = SDL_FLIP_NONE;
    }

    // set texture color modulation
    SDL_SetTextureColorMod(texture->_texture, property.r, property.g, property.b);
    SDL_SetTextureAlphaMod(texture->_texture, property.a);

    if (property.rot != 0)
    {
        SDL_RenderCopyEx(this->_renderer, texture->_texture, &src_rect, &dst_rect, property.rot, NULL, (SDL_RendererFlip)flip);
        return 0;
    }

    // render texture
    SDL_RenderCopyEx(this->_renderer, texture->_texture, &src_rect, &dst_rect, NULL, NULL, (SDL_RendererFlip)flip);

    return 0;
}

int PixL_Renderer::draw_tilemap(PixL_Tilemap *tilemap, int screenw, int screenh)
{
    return 0;
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

int PixL_Renderer_Quit()
{
    if (PixL_Renderer::_instance == nullptr)
    {
        std::cerr << "Renderer not initialized!" << std::endl;
        return -1;
    }

    delete PixL_Renderer::_instance;
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

    PixL_Renderer::_instance->_renderer = SDL_CreateRenderer(PixL_Renderer::_instance->_window, -1, SDL_RENDERER_ACCELERATED);

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

    int w, h;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    pixl_texture->_width = w;
    pixl_texture->_height = h;
    pixl_texture->_texture = texture;

    return pixl_texture;
}

PixL_Texture *CreateTexture(std::vector<uint8_t> &data, int width, int height, PixelDataFormat format)
{
    if (PixL_Renderer::_instance == nullptr || PixL_Renderer::_instance->_renderer == nullptr)
    {
        std::cerr << "Renderer not initialized or renderer not created!" << std::endl;
        return nullptr;
    }

    int pixel_size = 0;
    if (format == PixelDataFormat::PIXL_PIXEL_FORMAT_RGBA || format == PixelDataFormat::PIXL_PIXEL_FORMAT_BGRA || format == PixelDataFormat::PIXL_PIXEL_FORMAT_ARGB || format == PixelDataFormat::PIXL_PIXEL_FORMAT_ABGR)
    {
        pixel_size = 4;
    }
    else if (format == PixelDataFormat::PIXL_PIXEL_FORMAT_RGB || format == PixelDataFormat::PIXL_PIXEL_FORMAT_BGR)
    {
        pixel_size = 3;
    }

    if (pixel_size == 0)
    {
        std::cerr << "Invalid pixel format!" << std::endl;
        return nullptr;
    }

    // check data size
    if (data.size() != width * height * pixel_size)
    {
        std::cerr << "Invalid data size!" << std::endl;
        return nullptr;
    }

    SDL_PixelFormatEnum pixel_format;
    switch (format)
    {
    case PixelDataFormat::PIXL_PIXEL_FORMAT_RGB:
        pixel_format = SDL_PIXELFORMAT_RGB24;
        break;
    case PixelDataFormat::PIXL_PIXEL_FORMAT_BGR:
        pixel_format = SDL_PIXELFORMAT_BGR24;
        break;
    case PixelDataFormat::PIXL_PIXEL_FORMAT_RGBA:
        pixel_format = SDL_PIXELFORMAT_RGBA32;
        break;
    case PixelDataFormat::PIXL_PIXEL_FORMAT_BGRA:
        pixel_format = SDL_PIXELFORMAT_BGRA32;
        break;
    case PixelDataFormat::PIXL_PIXEL_FORMAT_ARGB:
        pixel_format = SDL_PIXELFORMAT_ARGB32;
        break;
    case PixelDataFormat::PIXL_PIXEL_FORMAT_ABGR:
        pixel_format = SDL_PIXELFORMAT_ABGR32;
        break;
    }

    SDL_Texture *texture = SDL_CreateTexture(PixL_Renderer::_instance->_renderer, pixel_format, SDL_TEXTUREACCESS_STATIC, width, height);
    if (texture == nullptr)
    {
        std::cerr << "Texture could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return nullptr;
    }

    SDL_UpdateTexture(texture, NULL, data.data(), width * pixel_size);

    PixL_Texture *pixl_texture = new PixL_Texture();
    pixl_texture->_texture = texture;

    return pixl_texture;
}

uint16_t PixL_AddDrawable(PixL_Drawable *drawable, PixL_Draw_Property property)
{
    if (PixL_Renderer::_instance == nullptr)
    {
        std::cerr << "Renderer not initialized!" << std::endl;
        return -1;
    }

    uint16_t id = 0;
    for (int i = 0; i < PixL_Renderer::_instance->_drawables.size(); i++)
    {
        if (PixL_Renderer::_instance->_drawables[i]->id == id)
        {
            id++;
        }
    }

    PixL_Draw_Command *command = new PixL_Draw_Command();
    command->id = id;
    command->property = property;
    command->drawable = drawable;

    PixL_Renderer::_instance->_drawables.push_back(command);

    return id;
}

void PixL_RemoveDrawable(uint16_t id)
{
    if (PixL_Renderer::_instance == nullptr)
    {
        std::cerr << "Renderer not initialized!" << std::endl;
        return;
    }

    for (int i = 0; i < PixL_Renderer::_instance->_drawables.size(); i++)
    {
        if (PixL_Renderer::_instance->_drawables[i]->id == id)
        {
            PixL_Renderer::_instance->_drawables.erase(PixL_Renderer::_instance->_drawables.begin() + i);
            return;
        }
    }
}

PixL_Draw_Property *PixL_GetDrawableProperty(uint16_t id)
{
    if (PixL_Renderer::_instance == nullptr)
    {
        std::cerr << "Renderer not initialized!" << std::endl;
        return nullptr;
    }

    for (int i = 0; i < PixL_Renderer::_instance->_drawables.size(); i++)
    {
        if (PixL_Renderer::_instance->_drawables[i]->id == id)
        {
            return &PixL_Renderer::_instance->_drawables[i]->property;
        }
    }

    return nullptr;
}

int PixL_Draw()
{
    if (PixL_Renderer::_instance == nullptr)
    {
        std::cerr << "Renderer not initialized!" << std::endl;
        return -1;
    }

    return PixL_Renderer::_instance->Draw();
}

int PixL_Present()
{
    if (PixL_Renderer::_instance == nullptr)
    {
        std::cerr << "Renderer not initialized!" << std::endl;
        return -1;
    }

    return PixL_Renderer::_instance->Present();
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

PixL_Texture::PixL_Texture()
{
}

PixL_Texture::~PixL_Texture()
{
    SDL_DestroyTexture(_texture);
}
