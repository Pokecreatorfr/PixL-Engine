#pragma once
#include <gfx/GpuTypes.hpp>

namespace pixl::gfx
{
    struct TextureDesc
    {
        int width = 1;
        int height = 1;
        PixelFormat format = PixelFormat::RGBA8;
        bool renderTarget = false;
        TextureFilter minFilter = TextureFilter::Linear;
        TextureFilter magFilter = TextureFilter::Linear;
        TextureWrap wrapU = TextureWrap::ClampToEdge;
        TextureWrap wrapV = TextureWrap::ClampToEdge;
    };
}
