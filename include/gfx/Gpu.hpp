#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <cstddef>
#include <functional>
#include <gfx/Buffer.hpp>
#include <gfx/GpuTypes.hpp>
#include <gfx/Material.hpp>
#include <gfx/Pass.hpp>
#include <gfx/Pipeline.hpp>
#include <gfx/Shader.hpp>
#include <gfx/Texture.hpp>
#include <optional>
#include <vector>

namespace pixl::gfx
{
    class Window;

    struct GBuffer
    {
        TextureHandle albedo = InvalidHandle;
        TextureHandle normal = InvalidHandle;
        TextureHandle material = InvalidHandle;
        uint32_t width = 0;
        uint32_t height = 0;
    };

    class Gpu
    {
    public:
        Gpu();
        ~Gpu();

        int Init(const Window &window, bool enableVsync);
        int Shutdown();

        TextureHandle CreateTexture(const TextureDesc &desc, const void *initialData = nullptr, size_t dataSize = 0);
        bool UpdateTexture(TextureHandle handle, const void *data, size_t dataSize);
        void DestroyTexture(TextureHandle handle);

        BufferHandle CreateBuffer(const BufferDesc &desc, const void *initialData = nullptr, size_t dataSize = 0);
        bool UpdateBuffer(BufferHandle handle, const void *data, size_t dataSize);
        void DestroyBuffer(BufferHandle handle);

        ShaderHandle CreateShader(const ShaderDesc &desc);
        void DestroyShader(ShaderHandle handle);

        PipelineHandle CreateGraphicsPipeline(const GraphicsPipelineDesc &desc);
        void DestroyPipeline(PipelineHandle handle);

        SamplerHandle CreateSampler(TextureFilter min, TextureFilter mag, TextureWrap wrapU, TextureWrap wrapV);
        void DestroySampler(SamplerHandle handle);

        bool CreateGBuffer(uint32_t width, uint32_t height);
        void DestroyGBuffer();
        const GBuffer *GetGBuffer() const { return gbuffer_.has_value() ? &gbuffer_.value() : nullptr; }

        int BeginPass(const RenderPassDesc &desc, const std::function<void(SDL_GPUCommandBuffer *)> &prePass = {});
        int EndPass();

        bool BindPipeline(PipelineHandle handle);
        bool BindVertexBuffer(BufferHandle handle, uint32_t slot, uint32_t offsetBytes = 0);
        bool BindIndexBuffer(BufferHandle handle, bool indexSize16Bit, uint32_t offsetBytes = 0);
        bool BindFragmentTexture(uint32_t slot, TextureHandle textureHandle, SamplerHandle samplerHandle = InvalidHandle);
        MaterialHandle CreateMaterial(const MaterialDesc &desc);
        void DestroyMaterial(MaterialHandle handle);
        bool BindMaterial(MaterialHandle handle);
        bool SetViewport(float x, float y, float w, float h, float minDepth = 0.0f, float maxDepth = 1.0f);
        bool ResetViewport();

        void Draw(uint32_t vertexCount, uint32_t firstVertex = 0, uint32_t instanceCount = 1);
        void DrawIndexed(uint32_t indexCount, uint32_t firstIndex = 0, uint32_t instanceCount = 1, int32_t vertexOffset = 0);

        SDL_GPUDevice *GetDevice() const { return device_; }
        SDL_GPUCommandBuffer *GetActiveCommandBuffer() const { return activeCommandBuffer_; }
        SDL_GPURenderPass *GetActiveRenderPass() const { return activeRenderPass_; }
        SDL_GPUTextureFormat GetSwapchainFormat() const { return swapchainFormat_; }

    private:
        struct TextureResource
        {
            SDL_GPUTexture *texture = nullptr;
            SDL_GPUSampler *sampler = nullptr;
            TextureDesc desc{};
            SamplerHandle defaultSampler = InvalidHandle;
        };

        struct BufferResource
        {
            SDL_GPUBuffer *buffer = nullptr;
            BufferDesc desc{};
        };

        struct ShaderResource
        {
            SDL_GPUShader *shader = nullptr;
            ShaderDesc desc{};
        };

        struct PipelineResource
        {
            SDL_GPUGraphicsPipeline *pipeline = nullptr;
            GraphicsPipelineDesc desc{};
        };

        struct SamplerResource
        {
            SDL_GPUSampler *sampler = nullptr;
        };

        struct MaterialResource
        {
            MaterialDesc desc{};
        };

        TextureResource *GetTextureResource(TextureHandle handle);
        BufferResource *GetBufferResource(BufferHandle handle);
        ShaderResource *GetShaderResource(ShaderHandle handle);
        PipelineResource *GetPipelineResource(PipelineHandle handle);
        SamplerResource *GetSamplerResource(SamplerHandle handle);
        MaterialResource *GetMaterialResource(MaterialHandle handle);

        bool UploadTextureData(TextureResource &resource, const void *data, size_t dataSize);
        bool UploadBufferData(BufferResource &resource, const void *data, size_t dataSize);
        bool EnsureDepthTexture(uint32_t width, uint32_t height);
        bool EnsureFallbackColorTarget(uint32_t width, uint32_t height);
        int BlitToSwapchain(TextureHandle source);
        int AcquireSwapchain(SDL_GPUTexture **outTex, Uint32 &outW, Uint32 &outH);
        void ReleaseSwapchain();

        SDL_GPUDevice *device_;
        SDL_GPUCommandBuffer *activeCommandBuffer_;
        SDL_GPURenderPass *activeRenderPass_;
        SDL_GPUTexture *swapchainTexture_;
        SDL_GPUTextureFormat swapchainFormat_;
        Uint32 swapchainWidth_;
        Uint32 swapchainHeight_;
        SDL_GPUTexture *depthTexture_;
        SDL_GPUTextureFormat depthFormat_;
        Uint32 depthWidth_;
        Uint32 depthHeight_;
        SDL_GPUTexture *fallbackColorTexture_;
        Uint32 fallbackWidth_;
        Uint32 fallbackHeight_;
        SDL_GPUTexture *lightingTexture_; // temporary target for lighting pass if needed
        const Window *window_;
        bool isInitialized_;
        bool currentUseGBuffer_;
        bool currentLightingBlit_;

        std::vector<std::optional<TextureResource>> textures_;
        std::vector<std::optional<BufferResource>> buffers_;
        std::vector<std::optional<ShaderResource>> shaders_;
        std::vector<std::optional<PipelineResource>> pipelines_;
        std::vector<std::optional<SamplerResource>> samplers_;
        std::vector<std::optional<MaterialResource>> materials_;
        std::optional<GBuffer> gbuffer_;

        std::vector<uint32_t> freeTextureSlots_;
        std::vector<uint32_t> freeBufferSlots_;
        std::vector<uint32_t> freeShaderSlots_;
        std::vector<uint32_t> freePipelineSlots_;
        std::vector<uint32_t> freeSamplerSlots_;
        std::vector<uint32_t> freeMaterialSlots_;

        std::vector<SDL_GPUBufferBinding> vertexBindings_;
        SDL_GPUBuffer *indexBuffer_;
        SDL_GPUIndexElementSize indexElementSize_;
        Uint32 indexOffsetBytes_;
        SDL_GPUGraphicsPipeline *boundPipeline_;
    };
}
