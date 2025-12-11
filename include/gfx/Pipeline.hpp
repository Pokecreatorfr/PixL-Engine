#pragma once
#include <gfx/GpuTypes.hpp>
#include <vector>

namespace pixl::gfx
{
    enum class VertexFormat : uint8_t
    {
        Float = 0,
        Float2,
        Float3,
        Float4,
        UInt,
        UInt2,
        UInt3,
        UInt4
    };

    enum class PrimitiveType : uint8_t
    {
        TriangleList = 0,
        TriangleStrip,
        LineList,
        LineStrip,
        PointList
    };

    enum class VertexInputRate : uint8_t
    {
        PerVertex = 0,
        PerInstance
    };

    struct VertexBufferLayout
    {
        uint32_t stride = 0;
        VertexInputRate rate = VertexInputRate::PerVertex;
    };

    struct VertexAttribute
    {
        uint32_t location = 0;
        uint32_t bufferSlot = 0;
        VertexFormat format = VertexFormat::Float3;
        uint32_t offset = 0;
    };

    struct VertexLayout
    {
        std::vector<VertexBufferLayout> buffers;
        std::vector<VertexAttribute> attributes;
    };

    struct GraphicsPipelineDesc
    {
        ShaderHandle vertexShader = InvalidHandle;
        ShaderHandle fragmentShader = InvalidHandle;
        VertexLayout vertexLayout{};
        PrimitiveType primitive = PrimitiveType::TriangleList;
        bool depthTest = false;
        bool depthWrite = false;
    };
}
