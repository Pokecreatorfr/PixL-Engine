#pragma once
#include <cstdint>
#include <gfx/gfx_desc.hpp>
#include <gfx/gfx_device.hpp>
#include <gfx/gfx_types.hpp>
#include <memory>
#include <span>
#include <utility>
#include <vector>

namespace pixl::gfx
{
    struct BufferBarrier
    {
        Buffer buffer{};
        PipelineStage srcStage{PipelineStage::TOP};
        Access srcAccess{Access::NONE};
        PipelineStage dstStage{PipelineStage::DRAW};
        Access dstAccess{Access::NONE};
        uint64_t offset{0};
        uint64_t size{~0ull};
    };

    struct ImageBarrier
    {
        Image image{};
        PipelineStage srcStage{PipelineStage::TOP};
        Access srcAccess{Access::NONE};
        ImageLayout oldLayout{ImageLayout::UNDEFINED};
        PipelineStage dstStage{PipelineStage::DRAW};
        Access dstAccess{Access::NONE};
        ImageLayout newLayout{ImageLayout::SHADER_READ_ONLY};
        ImageSubresourceRange range{};
    };

    class CommandList
    {
    public:
        virtual ~CommandList() = default;

        virtual void begin() = 0;
        virtual void end() = 0;

        virtual void barrier(
            uint32_t bufferCount, const BufferBarrier *buffers,
            uint32_t imageCount, const ImageBarrier *images) = 0;

        virtual void beginRendering(const RenderingDesc &) = 0;
        virtual void endRendering() = 0;

        // Bindings
        virtual void bindPipeline(Pipeline) = 0;
        virtual void bindPipelineLayout(PipelineLayout) = 0;
        virtual void bindDescriptorSet(const PipelineLayout &, DescSetIndex, const DescriptorSet &,
                                       std::span<const uint32_t> dynamicOffsets = {}) = 0;
        virtual void bindVertexBuffers(uint32_t firstBinding, uint32_t count,
                                       const Buffer *, const uint64_t *offsets) = 0;
        virtual void bindIndexBuffer(Buffer, uint64_t offset, IndexType) = 0;
        virtual void pushConstants(PipelineLayout, ShaderStage stages,
                                   uint32_t offset, uint32_t size, const void *data) = 0;

        // Draws
        virtual void draw(uint32_t vtxCount, uint32_t instCount,
                          uint32_t firstVtx, uint32_t firstInst) = 0;
        virtual void drawIndexed(uint32_t idxCount, uint32_t instCount,
                                 uint32_t firstIdx, int32_t vtxOffset, uint32_t firstInst) = 0;

        // Indirect draws
        virtual void drawIndirect(Buffer cmdBuf, uint64_t cmdOffset,
                                  uint32_t drawCount, uint32_t stride) = 0;
        virtual void drawIndexedIndirect(Buffer cmdBuf, uint64_t cmdOffset,
                                         uint32_t drawCount, uint32_t stride) = 0;
        virtual void drawIndexedIndirectCount(Buffer cmdBuf, uint64_t cmdOffset,
                                              Buffer countBuf, uint64_t countOffset,
                                              uint32_t maxDraws, uint32_t stride) = 0;

        // Compute
        virtual void dispatch(uint32_t gx, uint32_t gy, uint32_t gz) = 0;

        // Copies
        virtual void copyBuffer(Buffer src, uint64_t srcOff,
                                Buffer dst, uint64_t dstOff, uint64_t size) = 0;
        virtual void copyBufferToImage(Buffer src, uint64_t srcOff,
                                       Image dst, ImageLayout layout,
                                       uint32_t width, uint32_t height) = 0;

        // Debug
        virtual void beginLabel(const char *name,
                                float r = 0.2f, float g = 0.6f, float b = 0.9f, float a = 1.f) = 0;
        virtual void endLabel() = 0;
    };

    class Queue
    {
    public:
        virtual ~Queue() = default;
        virtual void submit(CommandList **lists, uint32_t count) = 0;
        virtual void present() = 0;
    };

    // Device

    struct DeviceCreateInfo
    {
        void *windowHandle{nullptr};
        device::API api{device::API::VULKAN};
        bool enableValidation{true};
        bool headless{false};
        device::Features requestedFeatures{};
        device::RequestQueues queues{};
        TextureFormat preferredColorFormat{TextureFormat::B8G8R8A8_UNORM_SRGB};
        TextureFormat preferredDepthFormat{TextureFormat::D32_FLOAT};
    };

    struct FrameEpoch
    {
        uint64_t index{0};
    };

    class Device
    {
    public:
        static std::unique_ptr<Device> Create(const DeviceCreateInfo &info);

        virtual ~Device() = default;

        // Informations générales
        virtual const device::DeviceInfo &getDeviceInfo() const = 0;
        virtual const device::Features &getEnabledFeatures() const = 0;
        virtual const device::Limits &getLimits() const = 0;
        virtual device::FormatCaps queryFormatCaps(TextureFormat fmt) const = 0;

        // Ressources
        virtual Buffer createBuffer(const BufferDesc &) = 0;
        virtual void destroy(Buffer) = 0;

        virtual Image createImage(const ImageDesc &) = 0;
        virtual void destroy(Image) = 0;

        virtual ImageView createImageView(Image, const ImageViewDesc &) = 0;
        virtual void destroy(ImageView) = 0;

        virtual Sampler createSampler(const SamplerDesc &) = 0;
        virtual void destroy(Sampler) = 0;

        virtual Shader createShader(const ShaderDesc &) = 0;
        virtual void destroy(Shader) = 0;
        virtual const ShaderLayout &getShaderLayout(Shader) const = 0;
        virtual const ShaderLayout &getPipelineLayoutInfo(PipelineLayout) const = 0;

        virtual DescriptorSetLayout createDescriptorSetLayout(const SetLayoutInfo &) = 0;
        virtual void destroy(DescriptorSetLayout) = 0;

        virtual PipelineLayout createPipelineLayout(const ShaderLayout &) = 0;
        virtual DescriptorSetLayout getDescriptorSetLayout(PipelineLayout, DescSetIndex set) const = 0;
        virtual void destroy(PipelineLayout) = 0;

        virtual Pipeline createGraphicsPipeline(const GraphicsPipelineDesc &, PipelineLayout) = 0;
        virtual Pipeline createComputePipeline(const ComputePipelineDesc &, PipelineLayout) = 0;
        virtual void destroy(Pipeline) = 0;

        virtual DescriptorSet allocateDescriptorSet(DescriptorSetLayout layout, bool transient = false) = 0;
        virtual void free(DescriptorSet set) = 0;
        virtual void updateDescriptors(std::span<const DescriptorWriteOperation> writes) = 0;

        // Commandes
        virtual CommandList *createCommandList(QueueType) = 0;
        virtual void destroy(CommandList *) = 0;

        virtual Queue *getQueue(QueueType) = 0;

        virtual bool acquireNextImage(uint32_t &outImageIndex) = 0;
        virtual void present(uint32_t imageIndex) = 0;

        virtual void waitIdle() = 0;

        virtual FrameEpoch beginFrame() = 0;
        virtual void endFrame() = 0;

        virtual uint32_t getSwapchainImageCount() const = 0;
        virtual Image getSwapchainImage(uint32_t index) const = 0;
        virtual ImageView getSwapchainImageView(uint32_t index) const = 0;
        virtual std::pair<uint32_t, uint32_t> getSwapchainExtent() const = 0;
        virtual TextureFormat getSwapchainFormat() const = 0;
    };

    class DescriptorWriter
    {
    public:
        explicit DescriptorWriter(Device &device)
            : m_device(device)
        {
        }

        DescriptorWriter &image(const DescriptorSet &set, BindingIndex binding, uint32_t arrayIndex, DescType type, const WriteImg &img)
        {
            DescriptorWriteOperation op{};
            op.set = set;
            op.binding = binding;
            op.arrayIndex = arrayIndex;
            op.type = type;
            op.image = img;
            op.isImage = true;
            m_ops.push_back(op);
            return *this;
        }

        DescriptorWriter &buffer(const DescriptorSet &set, BindingIndex binding, uint32_t arrayIndex, DescType type, const WriteBuf &buf)
        {
            DescriptorWriteOperation op{};
            op.set = set;
            op.binding = binding;
            op.arrayIndex = arrayIndex;
            op.type = type;
            op.buffer = buf;
            op.isImage = false;
            m_ops.push_back(op);
            return *this;
        }

        void submit()
        {
            if (!m_ops.empty())
            {
                m_device.updateDescriptors(m_ops);
                m_ops.clear();
            }
        }

        void clear() { m_ops.clear(); }

    private:
        Device &m_device;
        std::vector<DescriptorWriteOperation> m_ops;
    };

}
