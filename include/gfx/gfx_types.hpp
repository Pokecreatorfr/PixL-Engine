#pragma once
#include <cfloat>
#include <cstdint>
#include <type_traits>

namespace pixl::gfx
{

    template <class E>
    constexpr auto to_u(E e) { return static_cast<std::underlying_type_t<E>>(e); }

    template <class E>
    constexpr E operator|(E a, E b) { return static_cast<E>(to_u(a) | to_u(b)); }

    template <class E>
    constexpr E &operator|=(E &a, E b)
    {
        a = a | b;
        return a;
    }

    template <class E>
    constexpr E operator&(E a, E b) { return static_cast<E>(to_u(a) & to_u(b)); }

    template <class E>
    constexpr bool any(E a) { return to_u(a) != 0; }

    enum class TextureFormat : uint16_t
    {
        // 8-bit
        R8_UNORM,
        R8_SNORM,
        R8_UINT,
        R8_SINT,
        R8G8_UNORM,
        R8G8_SNORM,
        R8G8_UINT,
        R8G8_SINT,
        R8G8B8_UNORM,
        B8G8R8_UNORM,
        R8G8B8A8_UNORM,
        R8G8B8A8_SNORM,
        R8G8B8A8_UINT,
        R8G8B8A8_SINT,
        R8G8B8A8_UNORM_SRGB,
        B8G8R8A8_UNORM,
        B8G8R8A8_UNORM_SRGB,

        // HDR 10/11
        R10G10B10A2_UNORM,
        R11G11B10_FLOAT,

        // 16-bit
        R16_UNORM,
        R16_SNORM,
        R16_UINT,
        R16_SINT,
        R16_FLOAT,
        R16G16_UNORM,
        R16G16_SNORM,
        R16G16_UINT,
        R16G16_SINT,
        R16G16_FLOAT,
        R16G16B16A16_UNORM,
        R16G16B16A16_SNORM,
        R16G16B16A16_UINT,
        R16G16B16A16_SINT,
        R16G16B16A16_FLOAT,

        // 32-bit
        R32_UINT,
        R32_SINT,
        R32_FLOAT,
        R32G32_UINT,
        R32G32_SINT,
        R32G32_FLOAT,
        R32G32B32_UINT,
        R32G32B32_SINT,
        R32G32B32_FLOAT,
        R32G32B32A32_UINT,
        R32G32B32A32_SINT,
        R32G32B32A32_FLOAT,

        // Depth/Stencil
        D16_UNORM,
        D24_UNORM,
        S8_UINT,
        D24_UNORM_S8_UINT,
        D32_FLOAT,
        D32_FLOAT_S8X24_UINT,

        // Compressed
        BC1_UNORM,
        BC1_UNORM_SRGB,
        BC3_UNORM,
        BC3_UNORM_SRGB,
        BC4_UNORM,
        BC5_UNORM,
        BC6H_UFLOAT,
        BC6H_SFLOAT,
        BC7_UNORM,
        BC7_UNORM_SRGB,

        ETC2_R8G8B8_UNORM,
        ETC2_R8G8B8A1_UNORM,
        ETC2_R8G8B8A8_UNORM,
        ASTC_4x4_UNORM,
        ASTC_4x4_UNORM_SRGB,

        UNKNOWN
    };

    enum class SampleCount : uint8_t
    {
        _1 = 1,
        _2 = 2,
        _4 = 4,
        _8 = 8,
        _16 = 16
    };

    enum class AddressMode : uint8_t
    {
        REPEAT,
        MIRRORED_REPEAT,
        CLAMP_TO_EDGE,
        CLAMP_TO_BORDER
    };

    enum class CompareOp : uint8_t
    {
        NEVER,
        LESS,
        EQUAL,
        LESS_EQUAL,
        GREATER,
        NOT_EQUAL,
        GREATER_EQUAL,
        ALWAYS
    };

    enum class Filter : uint8_t
    {
        NEAREST,
        LINEAR
    };
    enum class MipmapMode : uint8_t
    {
        NEAREST,
        LINEAR
    };
    enum class BorderColor : uint8_t
    {
        TRANSPARENT_BLACK,
        OPAQUE_BLACK,
        OPAQUE_WHITE
    };

    struct SamplerDesc
    {
        Filter minFilter{Filter::LINEAR};
        Filter magFilter{Filter::LINEAR};
        MipmapMode mipmapMode{MipmapMode::LINEAR};
        AddressMode addressU{AddressMode::REPEAT};
        AddressMode addressV{AddressMode::REPEAT};
        AddressMode addressW{AddressMode::REPEAT};
        bool anisotropyEnable{false};
        float maxAnisotropy{1.0f};
        bool compareEnable{false};
        CompareOp compareOp{CompareOp::LESS_EQUAL};
        float minLod{0.f}, maxLod{FLT_MAX}, lodBias{0.f};
        BorderColor borderColor{BorderColor::OPAQUE_BLACK};
    };

    enum class QueueType : uint8_t
    {
        GRAPHICS,
        COMPUTE,
        TRANSFER
    };

    enum class PrimitiveTopology : uint8_t
    {
        POINT_LIST,
        LINE_LIST,
        LINE_STRIP,
        TRIANGLE_LIST,
        TRIANGLE_STRIP,
        PATCH_LIST
    };
    enum class PolygonMode : uint8_t
    {
        FILL,
        LINE,
        POINT
    };
    enum class CullMode : uint8_t
    {
        NONE,
        FRONT,
        BACK,
        FRONT_AND_BACK
    };
    enum class FrontFace : uint8_t
    {
        CLOCKWISE,
        COUNTER_CLOCKWISE
    };

    enum class IndexType : uint8_t
    {
        UINT16,
        UINT32
    };

    enum class BufferUsage : uint32_t
    {
        NONE = 0,
        VERTEX = 1 << 0,
        INDEX = 1 << 1,
        UNIFORM = 1 << 2,
        STORAGE = 1 << 3,
        INDIRECT = 1 << 4,
        TRANSFER_SRC = 1 << 5,
        TRANSFER_DST = 1 << 6,
        SHADER_DEVICE_ADDRESS = 1 << 7,
        ACCEL_STRUCT_BUILD_INPUT = 1 << 8
    };
    enum class TextureUsage : uint32_t
    {
        NONE = 0,
        SAMPLED = 1 << 0,
        STORAGE = 1 << 1,
        COLOR_ATTACHMENT = 1 << 2,
        DEPTH_STENCIL_ATTACHMENT = 1 << 3,
        TRANSFER_SRC = 1 << 4,
        TRANSFER_DST = 1 << 5,
        RESOLVE_SRC = 1 << 6,
        RESOLVE_DST = 1 << 7,
        INPUT_ATTACHMENT = 1 << 8
    };
    enum class MemoryUsage : uint8_t
    {
        GPU_ONLY,
        CPU_ONLY,
        CPU_TO_GPU,
        GPU_TO_CPU
    };

    enum class ShaderStage : uint32_t
    {
        NONE = 0,
        VERTEX = 1 << 0,
        FRAGMENT = 1 << 1,
        COMPUTE = 1 << 2,
        GEOMETRY = 1 << 3,
        TESS_CONTROL = 1 << 4,
        TESS_EVAL = 1 << 5,
        TASK = 1 << 6,
        MESH = 1 << 7,
        // ray tracing
        RAYGEN = 1 << 8,
        MISS = 1 << 9,
        CLOSEST_HIT = 1 << 10,
        ANY_HIT = 1 << 11,
        INTERSECTION = 1 << 12,
        CALLABLE = 1 << 13,
        ALL_GRAPHICS = VERTEX | FRAGMENT | GEOMETRY | TESS_CONTROL | TESS_EVAL
    };

    enum class PipelineStage : uint32_t
    {
        NONE = 0,
        TOP = 1 << 0,
        DRAW = 1 << 1,
        COMPUTE = 1 << 2,
        COPY = 1 << 3,
        BOTTOM = 1 << 4
    };
    enum class Access : uint32_t
    {
        NONE = 0,
        INDIRECT_READ = 1 << 0,
        UNIFORM_READ = 1 << 1,
        SHADER_SAMPLED_READ = 1 << 2,
        SHADER_STORAGE_READ = 1 << 3,
        SHADER_STORAGE_WRITE = 1 << 4,
        COLOR_ATTACHMENT_READ = 1 << 5,
        COLOR_ATTACHMENT_WRITE = 1 << 6,
        DEPTH_STENCIL_READ = 1 << 7,
        DEPTH_STENCIL_WRITE = 1 << 8,
        TRANSFER_READ = 1 << 9,
        TRANSFER_WRITE = 1 << 10
    };

    enum class ImageViewType : uint8_t
    {
        TYPE_2D,
        TYPE_2D_ARRAY,
        TYPE_3D,
        TYPE_CUBE,
        TYPE_CUBE_ARRAY
    };
    enum class ImageLayout : uint8_t
    {
        UNDEFINED,
        GENERAL,
        SHADER_READ_ONLY,
        COLOR_ATTACHMENT,
        DEPTH_STENCIL_ATTACHMENT,
        TRANSFER_SRC,
        TRANSFER_DST,
        PRESENT
    };
    enum class AspectMask : uint8_t
    {
        NONE = 0,
        COLOR = 1 << 0,
        DEPTH = 1 << 1,
        STENCIL = 1 << 2
    };
    struct ImageSubresourceRange
    {
        AspectMask aspect{AspectMask::COLOR};
        uint32_t baseMip{0}, levelCount{1};
        uint32_t baseArrayLayer{0}, layerCount{1};
    };

    struct ClearColor
    {
        float r{0}, g{0}, b{0}, a{1};
    };
    struct ClearDepthStencil
    {
        float depth{1.0f};
        uint32_t stencil{0};
    };

}
