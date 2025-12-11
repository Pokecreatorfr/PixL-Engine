#pragma once
#include <array>
#include <gfx/GpuTypes.hpp>
#include <gfx/Texture.hpp>
#include <gfx/TextureRef.hpp>

namespace pixl::gfx
{

    struct MaterialDesc
    {
        PipelineHandle pipeline = InvalidHandle;
        std::array<TexturePtr, 4> textures{};
        std::array<SamplerHandle, 4> samplers{InvalidHandle, InvalidHandle, InvalidHandle, InvalidHandle};
        float color[4]{1.0f, 1.0f, 1.0f, 1.0f};
        bool transparent = false;
    };
}
