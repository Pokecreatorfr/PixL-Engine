#pragma once
#include <cstdint>

namespace pixl::gfx
{
    // Basic GPU enums kept renderer-agnostic so we can move to another backend later.
    enum class PixelFormat : uint8_t
    {
        Unknown = 0,
        RGBA8,
        BGRA8,
        RGBA16F,
        R8,
        R16F,
        R32F
    };

    enum class BufferUsage : uint8_t
    {
        Vertex = 0,
        Index,
        Uniform,
        Storage
    };

    enum class ShaderStage : uint8_t
    {
        Vertex = 0,
        Fragment,
        Compute
    };

    enum class ShaderLanguage : uint8_t
    {
        SPV = 0,
        HLSL,
        MSL
    };

    enum class TextureFilter : uint8_t
    {
        Nearest = 0,
        Linear
    };

    enum class TextureWrap : uint8_t
    {
        ClampToEdge = 0,
        Repeat
    };

    using TextureHandle = uint32_t;
    using BufferHandle = uint32_t;
    using ShaderHandle = uint32_t;
    using PipelineHandle = uint32_t;
    using SamplerHandle = uint32_t;
    using MaterialHandle = uint32_t;

    constexpr uint32_t InvalidHandle = 0u;
}
