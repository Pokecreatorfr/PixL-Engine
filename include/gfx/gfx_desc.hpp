#pragma once
#include <cstdint>
#include <gfx/gfx_types.hpp>

namespace pixl::gfx
{

    struct BufferDesc
    {
        uint64_t size{0};
        BufferUsage usage{BufferUsage::NONE};
        MemoryUsage memory{MemoryUsage::GPU_ONLY};
    };

    struct ImageDesc
    {
        uint32_t width{1}, height{1}, depth{1};
        uint32_t mipLevels{1}, arrayLayers{1};
        TextureFormat format{TextureFormat::R8G8B8A8_UNORM};
        TextureUsage usage{TextureUsage::SAMPLED};
        SampleCount samples{SampleCount::_1};
    };

    struct ImageViewDesc
    {
        ImageViewType type{ImageViewType::TYPE_2D};
        TextureFormat format{TextureFormat::UNKNOWN};
        ImageSubresourceRange range{};
    };

    struct VertexAttribute
    {
        uint32_t location{0};
        uint32_t binding{0};
        TextureFormat format{TextureFormat::R32G32B32_FLOAT};
        uint32_t offset{0};
    };
    struct VertexBinding
    {
        uint32_t binding{0};
        uint32_t stride{0};
        bool perInstance{false};
    };

    enum class BlendFactor : uint8_t
    {
        ZERO,
        ONE,
        SRC_COLOR,
        ONE_MINUS_SRC_COLOR,
        DST_COLOR,
        ONE_MINUS_DST_COLOR,
        SRC_ALPHA,
        ONE_MINUS_SRC_ALPHA,
        DST_ALPHA,
        ONE_MINUS_DST_ALPHA
    };
    enum class BlendOp : uint8_t
    {
        ADD,
        SUBTRACT,
        REVERSE_SUBTRACT,
        MIN,
        MAX
    };

    struct ColorBlendAttachment
    {
        bool enable{false};
        BlendFactor srcColor{BlendFactor::ONE}, dstColor{BlendFactor::ZERO};
        BlendOp colorOp{BlendOp::ADD};
        BlendFactor srcAlpha{BlendFactor::ONE}, dstAlpha{BlendFactor::ZERO};
        BlendOp alphaOp{BlendOp::ADD};
        uint8_t writeMask{0xF}; // RGBA bits
    };

    struct DepthStencilState
    {
        bool depthTest{true};
        bool depthWrite{true};
        CompareOp depthCompare{CompareOp::LESS_EQUAL};
        bool stencilTest{false};
    };

    struct RasterState
    {
        PolygonMode polygon{PolygonMode::FILL};
        CullMode cull{CullMode::BACK};
        FrontFace front{FrontFace::COUNTER_CLOCKWISE};
        bool depthClamp{false};
        bool conservative{false};
        float depthBiasConstant{0.f};
        float depthBiasSlope{0.f};
    };

    struct MultisampleState
    {
        SampleCount samples{SampleCount::_1};
        bool sampleShading{false};
        float minSampleShading{1.0f};
    };

    struct ShaderDesc
    {
        ShaderStage stage{ShaderStage::NONE};
        const void *code{nullptr};
        uint32_t codeSize{0};
        const char *entryPoint{"main"};
    };

    struct GraphicsPipelineDesc
    {
        PrimitiveTopology topology{PrimitiveTopology::TRIANGLE_LIST};
        VertexBinding *bindings{nullptr};
        uint32_t bindingCount{0};
        VertexAttribute *attributes{nullptr};
        uint32_t attributeCount{0};

        const ShaderDesc *shaders{nullptr};
        uint32_t shaderCount{0};

        RasterState raster{};
        DepthStencilState depth{};
        MultisampleState msaa{};
        ColorBlendAttachment *blendAttachments{nullptr};
        uint32_t blendAttachmentCount{0};

        const TextureFormat *colorFormats{nullptr};
        uint32_t colorFormatCount{0};
        TextureFormat depthFormat{TextureFormat::UNKNOWN};
    };

    struct ComputePipelineDesc
    {
        ShaderDesc shader{};
    };

    struct RenderingAttachment
    {
        // color ou depth-stencil
        void *imageView{nullptr};
        ImageLayout layout{ImageLayout::COLOR_ATTACHMENT};
        bool load{true};
        bool store{true};
        ClearColor clearColor{};
        ClearDepthStencil clearDS{};
    };

    struct RenderingDesc
    {
        RenderingAttachment *color{nullptr};
        uint32_t colorCount{0};
        RenderingAttachment *depth{nullptr};
        uint32_t width{0}, height{0};
    };
}
