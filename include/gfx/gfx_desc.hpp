#pragma once
#include <cstdint>
#include <gfx/gfx_types.hpp>
#include <limits>
#include <vector>

namespace pixl::gfx
{
    struct Buffer
    {
        uint64_t h{0};
    };
    struct Image
    {
        uint64_t h{0};
    };
    struct ImageView
    {
        uint64_t h{0};
    };
    struct Sampler
    {
        uint64_t h{0};
    };
    struct Shader
    {
        uint64_t h{0};
    };
    struct Pipeline
    {
        uint64_t h{0};
    };
    struct PipelineLayout
    {
        uint64_t h{0};
    };
    struct Shader;

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

        const Shader *shaders{nullptr};
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
        const Shader *shader{nullptr};
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

    using DescSetIndex = uint32_t;
    using BindingIndex = uint32_t;
    using ShaderStageMask = uint32_t;

    enum class DescType : uint8_t
    {
        UniformBuffer,
        StorageBuffer,
        CombinedImageSampler,
        SampledImage,
        Sampler,
        StorageImage,
        TexelBuffer,
        StorageTexelBuffer,
        AccelStructureKHR
    };

    struct BindingInfo
    {
        BindingIndex binding{0};
        DescType type{DescType::UniformBuffer};
        uint32_t count{1};
        ShaderStageMask stages{0};
        bool bindless{false};
    };

    struct SetLayoutInfo
    {
        DescSetIndex set{0};
        std::vector<BindingInfo> bindings;
    };

    struct PushConstantRangeInfo
    {
        ShaderStageMask stages{0};
        uint32_t offset{0};
        uint32_t size{0};
    };

    struct ShaderLayout
    {
        std::vector<SetLayoutInfo> sets;
        std::vector<PushConstantRangeInfo> pushConstants;
        uint64_t layoutHash{0};
        uint64_t spirvHash{0};
    };

    struct DescriptorSetLayout
    {
        uint64_t h{0};
        DescSetIndex set{0};
    };

    struct DescriptorSet
    {
        uint64_t h{0};
        DescSetIndex set{0};
        bool transient{false};
        uint64_t epoch{0};
    };

    struct WriteImg
    {
        ImageView view{};
        Sampler sampler{};
        ImageLayout layout{ImageLayout::SHADER_READ_ONLY};
    };

    struct WriteBuf
    {
        Buffer buffer{};
        uint64_t offset{0};
        uint64_t range{std::numeric_limits<uint64_t>::max()};
    };

    struct DescriptorWriteOperation
    {
        DescriptorSet set{};
        BindingIndex binding{0};
        uint32_t arrayIndex{0};
        DescType type{DescType::UniformBuffer};
        WriteImg image{};
        WriteBuf buffer{};
        bool isImage{false};
    };
}
