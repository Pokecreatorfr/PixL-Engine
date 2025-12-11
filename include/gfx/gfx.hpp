#pragma once
#include <SDL3/SDL.h>
#include <gfx/Buffer.hpp>
#include <gfx/Gpu.hpp>
#include <gfx/GpuTypes.hpp>
#include <gfx/Pipeline.hpp>
#include <gfx/Pass.hpp>
#include <gfx/Shader.hpp>
#include <gfx/Texture.hpp>
#include <gfx/Window.hpp>
#include <memory>
#include <string>

namespace pixl::gfx
{
    class TextureCache;
    class PipelineCache;

    struct GfxInitData
    {
        std::string windowTitle = "PixL Engine";
        int windowWidth = 1280;
        int windowHeight = 720;
        uint32_t windowFlags = 0;
        bool enableVsync = true;
    };

    class Gfx
    {
    public:
        static int Init(const GfxInitData &initData);
        static int Quit();

        static Window *GetWindow() { return window_.get(); }
        static Gpu *GetGpu() { return gpu_.get(); }
        static TextureCache *GetTextureCache() { return textureCache_.get(); }
        static PipelineCache *GetPipelineCache() { return pipelineCache_.get(); }

    private:
        static bool IsInitialized_;
        static std::unique_ptr<Window> window_;
        static std::unique_ptr<Gpu> gpu_;
        static std::unique_ptr<TextureCache> textureCache_;
        static std::unique_ptr<PipelineCache> pipelineCache_;
    };
}
