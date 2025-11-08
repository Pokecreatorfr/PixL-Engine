#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <algorithm>
#include <array>
#include <atomic>
#include <cstdio>
#include <cstring>
#include <deque>
#include <functional>
#include <gfx/gfx_api.hpp>
#include <iostream>
#include <limits>
#include <mutex>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#ifndef VULKAN_HPP_DISPATCH_LOADER_DYNAMIC
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#endif
#ifndef VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_NO_EXCEPTIONS 1
#endif
#include <vk_mem_alloc.hpp>
#include <vulkan/vulkan.hpp>

namespace pixl::gfx
{
    class VulkanCommandList;
    class VulkanQueue;
    class VulkanDevice final : public Device
    {
    public:
        VulkanDevice(const DeviceCreateInfo &info);
        ~VulkanDevice() override;

        const device::DeviceInfo &getDeviceInfo() const override { return m_info; }
        const device::Features &getEnabledFeatures() const override { return m_info.supportedFeatures; }
        const device::Limits &getLimits() const override { return m_info.limits; }
        device::FormatCaps queryFormatCaps(TextureFormat fmt) const override;

        Buffer createBuffer(const BufferDesc &desc) override;
        void destroy(Buffer buffer) override;

        Image createImage(const ImageDesc &desc) override;
        void destroy(Image image) override;

        ImageView createImageView(Image image, const ImageViewDesc &desc) override;
        void destroy(ImageView view) override;

        Sampler createSampler(const SamplerDesc &desc) override;
        void destroy(Sampler sampler) override;

        Shader createShader(const ShaderDesc &desc) override;
        void destroy(Shader shader) override;
        const ShaderLayout &getShaderLayout(Shader shader) const override;
        const ShaderLayout &getPipelineLayoutInfo(PipelineLayout layout) const override;

        DescriptorSetLayout createDescriptorSetLayout(const SetLayoutInfo &) override;
        void destroy(DescriptorSetLayout layout) override;
        PipelineLayout createPipelineLayout(const ShaderLayout &) override;
        DescriptorSetLayout getDescriptorSetLayout(PipelineLayout layout, DescSetIndex set) const override;
        void destroy(PipelineLayout layout) override;

        Pipeline createGraphicsPipeline(const GraphicsPipelineDesc &desc, PipelineLayout layout) override;
        Pipeline createComputePipeline(const ComputePipelineDesc &desc, PipelineLayout layout) override;
        void destroy(Pipeline pipeline) override;

        DescriptorSet allocateDescriptorSet(DescriptorSetLayout layout, bool transient = false) override;
        void free(DescriptorSet set) override;
        void updateDescriptors(std::span<const DescriptorWriteOperation> writes) override;

        CommandList *createCommandList(QueueType type) override;
        void destroy(CommandList *list) override;

        Queue *getQueue(QueueType type) override;

        bool acquireNextImage(uint32_t &outImageIndex) override;
        void present(uint32_t imageIndex) override;

        void waitIdle() override;
        FrameEpoch beginFrame() override;
        void endFrame() override;

        uint32_t getSwapchainImageCount() const override;
        Image getSwapchainImage(uint32_t index) const override;
        ImageView getSwapchainImageView(uint32_t index) const override;
        std::pair<uint32_t, uint32_t> getSwapchainExtent() const override;
        TextureFormat getSwapchainFormat() const override;

    private:
        struct BufferResource
        {
            BufferDesc desc{};
            vk::Buffer buffer{};
            vma::Allocation allocation{};
        };

        struct ImageResource
        {
            ImageDesc desc{};
            vk::Image image{};
            vma::Allocation allocation{};
            bool swapchain{false};
        };

        struct ImageViewResource
        {
            ImageViewDesc desc{};
            Image image{};
            vk::UniqueImageView view{};
            bool swapchain{false};
        };

        struct SamplerResource
        {
            SamplerDesc desc{};
            vk::UniqueSampler sampler{};
        };

        struct ShaderResource
        {
            ShaderStage stage{ShaderStage::NONE};
            std::string entryPoint{"main"};
            vk::UniqueShaderModule module{};
            ShaderLayout layout{};
        };

        struct PipelineLayoutResource
        {
            vk::UniquePipelineLayout layout{};
            ShaderLayout layoutInfo{};
            std::vector<DescriptorSetLayout> setLayouts;
        };

        struct PipelineResource
        {
            vk::UniquePipeline pipeline{};
            PipelineLayout layout{};
            vk::PipelineBindPoint bindPoint{vk::PipelineBindPoint::eGraphics};
        };

        struct DescriptorSetLayoutResource
        {
            SetLayoutInfo info{};
            vk::UniqueDescriptorSetLayout layout{};
            bool updateAfterBind{false};
            std::vector<uint32_t> descriptorCounts;
        };

        struct DescriptorSetResource
        {
            DescriptorSetLayout layout{};
            vk::DescriptorSet set{};
            vk::DescriptorPool pool{};
            bool transient{false};
            bool pendingRelease{false};
            uint64_t epoch{0};
        };

        struct DescriptorPoolResource
        {
            vk::UniqueDescriptorPool pool{};
            uint32_t maxSets{0};
            uint32_t allocated{0};
            bool transient{false};
            bool updateAfterBind{false};
            uint64_t epoch{0};
        };

        struct FrameResetRequest
        {
            uint64_t epoch{0};
            uint64_t timelineValue{0};
        };

        device::DeviceInfo m_info{};
        vk::PhysicalDevice m_physicalDevice{};
        vk::UniqueDevice m_device{};
        vk::PhysicalDeviceMemoryProperties m_memoryProperties{};
        uint32_t m_graphicsQueueFamily{0};
        SDL_Window *m_window{nullptr};
        bool m_headless{false};
        vk::UniqueSurfaceKHR m_surface{};
        vk::UniqueSwapchainKHR m_swapchain{};
        vk::Format m_swapchainFormat{vk::Format::eB8G8R8A8Srgb};
        vk::Extent2D m_swapchainExtent{};
        std::vector<vk::Image> m_swapchainImages;
        std::vector<Image> m_swapchainImageHandles;
        std::vector<ImageView> m_swapchainImageViewHandles;
        std::vector<ImageLayout> m_swapchainImageLayouts;
        vk::Queue m_graphicsQueue{};
        std::unique_ptr<VulkanQueue> m_graphicsQueueWrapper{};
        vk::UniqueSemaphore m_imageAvailableSemaphore{};
        vk::UniqueSemaphore m_renderFinishedSemaphore{};
        vk::UniqueSemaphore m_graphicsTimelineSemaphore{};
        uint64_t m_graphicsTimelineValue{0};
        bool m_hasSwapchainImage{false};
        uint32_t m_currentSwapchainImage{0};
        bool m_acquireWaitPending{false};
        TextureFormat m_preferredColorFormat{TextureFormat::B8G8R8A8_UNORM_SRGB};
        vma::Allocator m_allocator{};
        vma::VulkanFunctions m_vmaFunctions{};
        mutable std::mutex m_resourceMutex{};
        std::unordered_map<uint64_t, BufferResource> m_buffers;
        std::unordered_map<uint64_t, ImageResource> m_images;
        std::unordered_map<uint64_t, ImageViewResource> m_imageViews;
        std::unordered_map<uint64_t, SamplerResource> m_samplers;
        std::unordered_map<uint64_t, ShaderResource> m_shaders;
        std::unordered_map<uint64_t, PipelineLayoutResource> m_pipelineLayouts;
        std::unordered_map<uint64_t, PipelineResource> m_pipelines;
        std::unordered_map<uint64_t, DescriptorSetLayoutResource> m_descriptorSetLayouts;
        std::unordered_map<uint64_t, DescriptorSetResource> m_descriptorSets;
        std::vector<DescriptorPoolResource> m_descriptorPools;
        std::vector<vk::DescriptorPoolSize> m_basePoolSizes;
        uint64_t m_frameEpochCounter{0};
        uint64_t m_activeFrameEpoch{0};
        uint64_t m_lastSubmittedTimelineValue{0};
        std::deque<FrameResetRequest> m_pendingFrameResets;
        bool m_frameActive{false};
        TextureFormat m_swapchainTextureFormat{TextureFormat::UNKNOWN};

        struct DeferredFree
        {
            uint64_t timelineValue{0};
            std::function<void()> deleter{};
        };
        std::deque<DeferredFree> m_deferredFrees;
        mutable std::mutex m_garbageMutex{};

        static vk::UniqueInstance m_instance;

        static const char *kValidationLayer;

        void createSurface(SDL_Window *window);
        void createLogicalDevice(const DeviceCreateInfo &info,
                                 const vk::PhysicalDeviceFeatures2 &features2,
                                 const vk::PhysicalDeviceVulkan12Features &features12,
                                 const vk::PhysicalDeviceVulkan13Features &features13,
                                 const vk::PhysicalDeviceMultiviewFeatures &multiviewFeatures,
                                 uint32_t apiVersion,
                                 const std::vector<vk::ExtensionProperties> &extensions);
        void initializeAllocator();
        void destroySwapchain();
        void createSwapchain(TextureFormat preferredFormat);
        void createSyncObjects();
        void setupQueues(const device::RequestQueues &queues);
        void enqueueGarbage(uint64_t timelineValue, std::function<void()> &&deleter);
        void collectGarbage(bool force = false);
        uint64_t queryCompletedTimeline() const;

        vk::ImageView getVkImageView(ImageView view) const;
        vk::PipelineLayout getVkPipelineLayout(PipelineLayout layout);
        vk::Pipeline getVkPipeline(Pipeline pipeline, vk::PipelineBindPoint &bindPoint);
        const ShaderResource &getShaderResource(Shader shader) const;
        BufferResource &requireBuffer(Buffer buffer);
        ImageResource &requireImage(Image image);
        ImageViewResource &requireImageView(ImageView view);
        SamplerResource &requireSampler(Sampler sampler);
        DescriptorSetLayoutResource &requireDescriptorSetLayout(DescriptorSetLayout layout);
        DescriptorSetResource &requireDescriptorSet(DescriptorSet set);
        vk::DescriptorSetLayout getVkDescriptorSetLayoutHandle(DescriptorSetLayout layout);
        vk::DescriptorSet getVkDescriptorSetHandle(DescriptorSet set);
        vk::DescriptorType toVkDescriptorType(DescType type) const;
        void initializeDescriptorPools();
        DescriptorPoolResource &acquireDescriptorPool(bool transient, bool updateAfterBind, uint64_t epoch);
        vk::UniqueDescriptorPool createDescriptorPool(bool transient, bool updateAfterBind);
        uint64_t ensureActiveEpoch();
        void beginFrameEpoch();
        void enqueueFrameReset(uint64_t epoch, uint64_t timelineValue);
        void processFrameResets(uint64_t completedValue);
        void resetTransientPoolsForEpoch(uint64_t epoch);
        void resetAllTransientPools();
        void releaseSwapchainResources();

        friend class VulkanCommandList;
        friend class VulkanQueue;
    };

    class VulkanCommandList final : public CommandList
    {
    public:
        VulkanCommandList(VulkanDevice &device, QueueType type, uint32_t familyIndex);

        void begin() override;
        void end() override;

        void barrier(uint32_t bufferCount, const BufferBarrier *buffers,
                     uint32_t imageCount, const ImageBarrier *images) override;
        void beginRendering(const RenderingDesc &desc) override;
        void endRendering() override;

        void bindPipeline(Pipeline pipeline) override;
        void bindPipelineLayout(PipelineLayout layout) override;
        void bindDescriptorSet(const PipelineLayout &layout, DescSetIndex set, const DescriptorSet &descriptorSet,
                               std::span<const uint32_t> dynamicOffsets) override;
        void bindVertexBuffers(uint32_t firstBinding, uint32_t count,
                               const Buffer *buffers, const uint64_t *offsets) override;
        void bindIndexBuffer(Buffer buffer, uint64_t offset, IndexType type) override;
        void pushConstants(PipelineLayout layout, ShaderStage stages,
                           uint32_t offset, uint32_t size, const void *data) override;

        void draw(uint32_t vtxCount, uint32_t instCount,
                  uint32_t firstVtx, uint32_t firstInst) override;
        void drawIndexed(uint32_t idxCount, uint32_t instCount,
                         uint32_t firstIdx, int32_t vtxOffset, uint32_t firstInst) override;

        void drawIndirect(Buffer cmdBuf, uint64_t cmdOffset,
                          uint32_t drawCount, uint32_t stride) override;
        void drawIndexedIndirect(Buffer cmdBuf, uint64_t cmdOffset,
                                 uint32_t drawCount, uint32_t stride) override;
        void drawIndexedIndirectCount(Buffer cmdBuf, uint64_t cmdOffset,
                                      Buffer countBuf, uint64_t countOffset,
                                      uint32_t maxDraws, uint32_t stride) override;

        void dispatch(uint32_t gx, uint32_t gy, uint32_t gz) override;

        void copyBuffer(Buffer src, uint64_t srcOff,
                        Buffer dst, uint64_t dstOff, uint64_t size) override;
        void copyBufferToImage(Buffer src, uint64_t srcOff,
                               Image dst, ImageLayout layout,
                               uint32_t width, uint32_t height) override;

        void beginLabel(const char *name,
                        float r, float g, float b, float a) override;
        void endLabel() override;

        vk::CommandBuffer getCommandBuffer() const { return m_commandBuffer; }
        bool isReadyForSubmit() const { return !m_recording && m_hasWork; }
        void markSubmitted() { m_hasWork = false; }

    private:
        VulkanDevice &m_device;
        QueueType m_queueType;
        vk::UniqueCommandPool m_commandPool{};
        vk::CommandBuffer m_commandBuffer{};
        bool m_recording{false};
        bool m_hasWork{false};
        bool m_rendering{false};
        PipelineLayout m_boundLayout{};
        vk::PipelineBindPoint m_boundBindPoint{vk::PipelineBindPoint::eGraphics};

        void ensureRecording(const char *action) const;
        const VulkanDevice::ImageViewResource &getImageViewResource(ImageView view) const;
    };

    class VulkanQueue final : public Queue
    {
    public:
        VulkanQueue(VulkanDevice &device, QueueType type, uint32_t familyIndex, vk::Queue queue);

        void submit(CommandList **lists, uint32_t count) override;
        void present() override;

    private:
        VulkanDevice &m_device;
        QueueType m_type;
        uint32_t m_familyIndex;
        vk::Queue m_queue;
    };
}
