#include <PixL-Ressource.hpp>

PixL_Ressource::PixL_Ressource()
{
}

PixL_Ressource::~PixL_Ressource()
{
    for (const auto &pakContext : pakContexts)
    {
        delete pakContext.second;
    }
    pakContexts.clear();
}

PixL_Ressource *PixL_Ressource::_instance = nullptr;

int8_t PixL_Ressource::loadPak(std::string pakFileName)
{
    PixL_Pak::Context *context = PixL_Pak::Open(pakFileName);

    if (!context)
    {
        std::cerr << "Failed to open pak file: " << pakFileName << std::endl;
        return -1;
    }

    bool found = false;
    int8_t id = 0;
    while (!found)
    {
        found = true;
        for (const auto &pakContext : pakContexts)
        {
            if (pakContext.first == id)
            {
                found = false;
                id++;
                break;
            }
        }
        if (id > 127)
        {
            std::cerr << "Too many pak files loaded. Maximum is 128." << std::endl;
            delete context;
            return -1;
        }
    }

    pakContexts.emplace_back(id, context);

    return id;
}

bool PixL_Ressource::unloadPak(int8_t pakID)
{
    for (auto it = pakContexts.begin(); it != pakContexts.end(); ++it)
    {
        if (it->first == pakID)
        {
            delete it->second;
            pakContexts.erase(it);
            return true;
        }
    }
    std::cerr << "Pak ID " << static_cast<int>(pakID) << " not found." << std::endl;
    return false;
}

bool PixL_Ressource::CreatePipeline(std::string name, Shader_Struct *vertexShader, Shader_Struct *fragmentShader, bool depthTest, SDL_GPUCompareOp compareOp, bool enable_depth_test, bool enable_depth_write)
{
    return true;
}

bool PixL_Ressource::CreateTexture(std::string name, std::string imagePath)
{
    if (!PixL_Renderer::_instance)
    {
        std::cerr << "PixL_Renderer not initialized. Call PixL_Renderer_Init() first." << std::endl;
        return false;
    }

    std::vector<char> buffer;
    SDL_Surface *surface = nullptr;

    try
    {
        for (int i = pakContexts.size() - 1; i >= 0 || surface == nullptr; --i)
        {
            PixL_Pak::Context *context = pakContexts[i].second;
            buffer = context->readFile(imagePath);
            if (!buffer.empty())
            {
                SDL_IOStream *rw = SDL_IOFromConstMem(buffer.data(),
                                                      static_cast<int>(buffer.size()));

                if (!rw)
                {
                    std::cerr << "Failed to create SDL_IOStream from memory: " << SDL_GetError() << std::endl;
                    break;
                }

                surface = SDL_LoadBMP_IO(rw, true);
                if (!surface)
                {
                    std::cerr << "Failed to load image from pak: " << imagePath << " - " << SDL_GetError() << std::endl;
                    SDL_DestroySurface(surface);
                    surface = nullptr;
                    throw std::runtime_error("Failed to load image from pak: " + imagePath);
                }
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error loading image: " << imagePath << std::endl;
        SDL_IOStream *rw = SDL_IOFromConstMem(missing_texture, missing_texture_len);
        if (!rw)
        {
            std::cerr << "Failed to create SDL_IOStream from memory: " << SDL_GetError() << std::endl;
        }
        surface = SDL_LoadBMP_IO(rw, true);
        if (!surface)
        {
            std::cerr << "Failed to load missing texture: " << SDL_GetError() << std::endl;
            return false;
        }
    }

    if (PixL_CreateTexture(name, surface))
    {
        SDL_DestroySurface(surface);
        return true;
    }
    SDL_DestroySurface(surface);
    return false;
}
