#pragma once

#include <gfx/Gpu.hpp>
#include <gfx/ImageLoader.hpp>
#include <gfx/TextureRef.hpp>
#include <memory>
#include <string>
#include <unordered_map>

namespace pixl::gfx
{
    class TextureCache
    {
    public:
        explicit TextureCache(Gpu *gpu);

        TexturePtr GetTexture(const std::string &path);
        TexturePtr GetWhiteTexture();
        void Clear();

    private:
        struct State
        {
            explicit State(Gpu *gpuPtr) : gpu(gpuPtr) {}

            Gpu *gpu = nullptr;
            std::unordered_map<std::string, std::weak_ptr<TextureRef>> cache;
            std::weak_ptr<TextureRef> white;
        };

        TextureHandle CreateWhite();
        TexturePtr MakeManagedTexture(TextureHandle handle, const std::string &path);
        TexturePtr MakeManagedWhite(TextureHandle handle);

        std::shared_ptr<State> state_;
    };
}
