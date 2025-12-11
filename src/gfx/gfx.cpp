#include <core/Errors.hpp>
#include <gfx/PipelineCache.hpp>
#include <gfx/TextureCache.hpp>
#include <gfx/gfx.hpp>
#include <SDL3/SDL.h>

using namespace pixl::gfx;

bool Gfx::IsInitialized_ = false;
std::unique_ptr<Window> Gfx::window_;
std::unique_ptr<Gpu> Gfx::gpu_;
std::unique_ptr<TextureCache> Gfx::textureCache_;
std::unique_ptr<PipelineCache> Gfx::pipelineCache_;

int Gfx::Init(const GfxInitData &initData)
{
    if (IsInitialized_)
    {
        return 0;
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        PIXL_LOG_AND_RETURN_ERROR(-1, pixl::core::Errc::UnknownError, "gfx", "SDL video init failed: {}", SDL_GetError());
    }

    IsInitialized_ = true;

    window_ = std::make_unique<Window>(initData.windowTitle, initData.windowWidth, initData.windowHeight, initData.windowFlags);
    if (!window_ || !window_->GetSDLWindow())
    {
        Quit();
        PIXL_LOG_AND_RETURN_ERROR(-1, pixl::core::Errc::UnknownError, "gfx", "Window creation failed: {}", SDL_GetError());
    }

    gpu_ = std::make_unique<Gpu>();
    if (gpu_->Init(*window_, initData.enableVsync) != 0)
    {
        Quit();
        PIXL_LOG_AND_RETURN_ERROR(-1, pixl::core::Errc::UnknownError, "gfx", "Failed to initialize GPU backend");
    }

    pipelineCache_ = std::make_unique<PipelineCache>(gpu_.get());
    textureCache_ = std::make_unique<TextureCache>(gpu_.get());

    return 0;
}

int Gfx::Quit()
{
    if (!IsInitialized_)
    {
        // Video subsystem might have been partially initialized even on failure paths.
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        return 0;
    }

    if (pipelineCache_)
    {
        pipelineCache_->Clear();
        pipelineCache_.reset();
    }

    if (textureCache_)
    {
        textureCache_->Clear();
        textureCache_.reset();
    }

    if (gpu_)
    {
        gpu_->Shutdown();
        gpu_.reset();
    }

    window_.reset();

    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    IsInitialized_ = false;
    return 0;
}
