#include "Renderer.hpp"

Renderer::Renderer()
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);

    // load glad
    if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
    {
        std::cout << "Failed to load glad\n";
        return;
    }
}

Renderer* Renderer::instance_ = nullptr;

Renderer* Renderer::GetInstance()
{
    if (instance_ == nullptr)
    {
        instance_ = new Renderer();
    }
    return instance_;
}
