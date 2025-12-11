#include <algorithm>
#include <core/Errors.hpp>
#include <cstring>
#include <gfx/Gpu.hpp>
#include <gfx/Window.hpp>
#include <optional>
#include <utility>

using namespace pixl::gfx;

namespace
{
    SDL_GPUTextureFormat ToGPUFormat(PixelFormat fmt)
    {
        switch (fmt)
        {
        case PixelFormat::RGBA8:
            return SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
        case PixelFormat::BGRA8:
            return SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM;
        case PixelFormat::RGBA16F:
            return SDL_GPU_TEXTUREFORMAT_R16G16B16A16_FLOAT;
        case PixelFormat::R8:
            return SDL_GPU_TEXTUREFORMAT_R8_UNORM;
        case PixelFormat::R16F:
            return SDL_GPU_TEXTUREFORMAT_R16_FLOAT;
        case PixelFormat::R32F:
            return SDL_GPU_TEXTUREFORMAT_R32_FLOAT;
        default:
            return SDL_GPU_TEXTUREFORMAT_INVALID;
        }
    }

    SDL_GPUBufferUsageFlags ToGPUUsage(BufferUsage usage)
    {
        switch (usage)
        {
        case BufferUsage::Vertex:
            return SDL_GPU_BUFFERUSAGE_VERTEX;
        case BufferUsage::Index:
            return SDL_GPU_BUFFERUSAGE_INDEX;
        case BufferUsage::Uniform:
            return SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ;
        case BufferUsage::Storage:
            return SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ | SDL_GPU_BUFFERUSAGE_COMPUTE_STORAGE_READ | SDL_GPU_BUFFERUSAGE_COMPUTE_STORAGE_WRITE;
        default:
            return SDL_GPU_BUFFERUSAGE_VERTEX;
        }
    }

    SDL_GPUShaderFormat ToGPUShaderFormat(ShaderLanguage lang)
    {
        switch (lang)
        {
        case ShaderLanguage::SPV:
            return SDL_GPU_SHADERFORMAT_SPIRV; // expects SPIR-V blobs
        case ShaderLanguage::HLSL:
            return SDL_GPU_SHADERFORMAT_DXIL;
        case ShaderLanguage::MSL:
            return SDL_GPU_SHADERFORMAT_MSL;
        default:
            return SDL_GPU_SHADERFORMAT_INVALID;
        }
    }

    SDL_GPUShaderStage ToGPUShaderStage(ShaderStage stage)
    {
        switch (stage)
        {
        case ShaderStage::Vertex:
            return SDL_GPU_SHADERSTAGE_VERTEX;
        case ShaderStage::Fragment:
            return SDL_GPU_SHADERSTAGE_FRAGMENT;
        default:
            return SDL_GPU_SHADERSTAGE_VERTEX;
        }
    }

    SDL_GPUFilter ToGPUFilter(TextureFilter f)
    {
        return f == TextureFilter::Nearest ? SDL_GPU_FILTER_NEAREST : SDL_GPU_FILTER_LINEAR;
    }

    SDL_GPUSamplerAddressMode ToGPUAddress(TextureWrap w)
    {
        return w == TextureWrap::Repeat ? SDL_GPU_SAMPLERADDRESSMODE_REPEAT : SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    }

    SDL_GPUVertexElementFormat ToGPUVertexFormat(VertexFormat fmt)
    {
        switch (fmt)
        {
        case VertexFormat::Float:
            return SDL_GPU_VERTEXELEMENTFORMAT_FLOAT;
        case VertexFormat::Float2:
            return SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
        case VertexFormat::Float3:
            return SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
        case VertexFormat::Float4:
            return SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4;
        case VertexFormat::UInt:
            return SDL_GPU_VERTEXELEMENTFORMAT_UINT;
        case VertexFormat::UInt2:
            return SDL_GPU_VERTEXELEMENTFORMAT_UINT2;
        case VertexFormat::UInt3:
            return SDL_GPU_VERTEXELEMENTFORMAT_UINT3;
        case VertexFormat::UInt4:
            return SDL_GPU_VERTEXELEMENTFORMAT_UINT4;
        default:
            return SDL_GPU_VERTEXELEMENTFORMAT_INVALID;
        }
    }

    SDL_GPUPrimitiveType ToGPUPrimitive(PrimitiveType p)
    {
        switch (p)
        {
        case PrimitiveType::TriangleList:
            return SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
        case PrimitiveType::TriangleStrip:
            return SDL_GPU_PRIMITIVETYPE_TRIANGLESTRIP;
        case PrimitiveType::LineList:
            return SDL_GPU_PRIMITIVETYPE_LINELIST;
        case PrimitiveType::LineStrip:
            return SDL_GPU_PRIMITIVETYPE_LINESTRIP;
        case PrimitiveType::PointList:
            return SDL_GPU_PRIMITIVETYPE_POINTLIST;
        default:
            return SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
        }
    }

    template <typename T>
    uint32_t AllocateHandle(std::vector<std::optional<T>> &pool, std::vector<uint32_t> &freeList, T &&value)
    {
        if (!freeList.empty())
        {
            uint32_t idx = freeList.back();
            freeList.pop_back();
            pool[idx] = std::move(value);
            return idx + 1;
        }

        pool.emplace_back(std::move(value));
        return static_cast<uint32_t>(pool.size());
    }

    template <typename T>
    T *ResolveHandle(std::vector<std::optional<T>> &pool, uint32_t handle)
    {
        if (handle == InvalidHandle)
        {
            return nullptr;
        }
        uint32_t idx = handle - 1;
        if (idx >= pool.size())
        {
            return nullptr;
        }
        if (!pool[idx].has_value())
        {
            return nullptr;
        }
        return &pool[idx].value();
    }

    template <typename T>
    void FreeHandle(std::vector<std::optional<T>> &pool, std::vector<uint32_t> &freeList, uint32_t handle)
    {
        if (handle == InvalidHandle)
        {
            return;
        }
        uint32_t idx = handle - 1;
        if (idx >= pool.size())
        {
            return;
        }
        pool[idx].reset();
        freeList.push_back(idx);
    }

    size_t BytesPerPixel(PixelFormat fmt)
    {
        switch (fmt)
        {
        case PixelFormat::RGBA8:
        case PixelFormat::BGRA8:
            return 4;
        case PixelFormat::RGBA16F:
            return 8;
        case PixelFormat::R8:
            return 1;
        case PixelFormat::R16F:
            return 2;
        case PixelFormat::R32F:
            return 4;
        default:
            return 0;
        }
    }
}

Gpu::Gpu()
    : device_(nullptr),
      activeCommandBuffer_(nullptr),
      activeRenderPass_(nullptr),
      swapchainTexture_(nullptr),
      swapchainFormat_(SDL_GPU_TEXTUREFORMAT_INVALID),
      swapchainWidth_(0),
      swapchainHeight_(0),
      depthTexture_(nullptr),
      depthFormat_(SDL_GPU_TEXTUREFORMAT_D16_UNORM),
      depthWidth_(0),
      depthHeight_(0),
      fallbackColorTexture_(nullptr),
      fallbackWidth_(0),
      fallbackHeight_(0),
      window_(nullptr),
      isInitialized_(false),
      currentUseGBuffer_(false),
      currentLightingBlit_(false),
      indexBuffer_(nullptr),
      indexElementSize_(SDL_GPU_INDEXELEMENTSIZE_16BIT),
      indexOffsetBytes_(0),
      boundPipeline_(nullptr)
{
}

Gpu::~Gpu()
{
    Shutdown();
}

int Gpu::Init(const Window &window, bool enableVsync)
{
    if (isInitialized_)
    {
        return 0;
    }

    // We currently ship SPIR-V shaders; request a backend that supports SPIR-V.
    SDL_GPUShaderFormat shaderFormats = SDL_GPU_SHADERFORMAT_SPIRV;
    device_ = SDL_CreateGPUDevice(shaderFormats, false, "vulkan");
    if (!device_)
    {
        PIXL_LOG_AND_RETURN_ERROR(-1, pixl::core::Errc::UnknownError, "gfx::Gpu", "SDL_CreateGPUDevice failed: {}", SDL_GetError());
    }

    if (!SDL_ClaimWindowForGPUDevice(device_, window.GetSDLWindow()))
    {
        SDL_DestroyGPUDevice(device_);
        device_ = nullptr;
        PIXL_LOG_AND_RETURN_ERROR(-1, pixl::core::Errc::InvalidParameter, "gfx::Gpu", "Failed to claim window for GPU device: {}", SDL_GetError());
    }

    if (!SDL_SetGPUSwapchainParameters(device_, window.GetSDLWindow(), SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_VSYNC))
    {
        SDL_DestroyGPUDevice(device_);
        device_ = nullptr;
        PIXL_LOG_AND_RETURN_ERROR(-1, pixl::core::Errc::UnknownError, "gfx::Gpu", "Failed to set swapchain parameters: {}", SDL_GetError());
    }

    swapchainFormat_ = SDL_GetGPUSwapchainTextureFormat(device_, window.GetSDLWindow());
    if (swapchainFormat_ == SDL_GPU_TEXTUREFORMAT_INVALID)
    {
        SDL_DestroyGPUDevice(device_);
        device_ = nullptr;
        PIXL_LOG_AND_RETURN_ERROR(-1, pixl::core::Errc::UnknownError, "gfx::Gpu", "Failed to query swapchain format");
    }

    window_ = &window;
    isInitialized_ = true;
    (void)enableVsync; // TODO: allow configuring present mode/swapchain parameters
    return 0;
}

int Gpu::Shutdown()
{
    if (!isInitialized_)
    {
        return 0;
    }

    for (auto &tex : textures_)
    {
        if (tex.has_value())
        {
            if (tex->texture)
            {
                SDL_ReleaseGPUTexture(device_, tex->texture);
            }
            if (tex->sampler)
            {
                SDL_ReleaseGPUSampler(device_, tex->sampler);
            }
        }
    }
    for (auto &buf : buffers_)
    {
        if (buf.has_value() && buf->buffer)
        {
            SDL_ReleaseGPUBuffer(device_, buf->buffer);
        }
    }
    for (auto &sh : shaders_)
    {
        if (sh.has_value() && sh->shader)
        {
            SDL_ReleaseGPUShader(device_, sh->shader);
        }
    }
    for (auto &p : pipelines_)
    {
        if (p.has_value() && p->pipeline)
        {
            SDL_ReleaseGPUGraphicsPipeline(device_, p->pipeline);
        }
    }
    for (auto &s : samplers_)
    {
        if (s.has_value() && s->sampler)
        {
            SDL_ReleaseGPUSampler(device_, s->sampler);
        }
    }

    textures_.clear();
    buffers_.clear();
    shaders_.clear();
    pipelines_.clear();
    samplers_.clear();
    materials_.clear();
    gbuffer_.reset();
    freeTextureSlots_.clear();
    freeBufferSlots_.clear();
    freeShaderSlots_.clear();
    freePipelineSlots_.clear();
    freeSamplerSlots_.clear();
    freeMaterialSlots_.clear();
    vertexBindings_.clear();

    if (depthTexture_)
    {
        SDL_ReleaseGPUTexture(device_, depthTexture_);
        depthTexture_ = nullptr;
    }
    if (fallbackColorTexture_)
    {
        SDL_ReleaseGPUTexture(device_, fallbackColorTexture_);
        fallbackColorTexture_ = nullptr;
        fallbackWidth_ = 0;
        fallbackHeight_ = 0;
    }

    if (device_)
    {
        SDL_DestroyGPUDevice(device_);
        device_ = nullptr;
    }

    activeCommandBuffer_ = nullptr;
    activeRenderPass_ = nullptr;
    swapchainTexture_ = nullptr;
    depthWidth_ = 0;
    depthHeight_ = 0;
    window_ = nullptr;
    boundPipeline_ = nullptr;
    isInitialized_ = false;
    return 0;
}

TextureHandle Gpu::CreateTexture(const TextureDesc &desc, const void *initialData, size_t dataSize)
{
    if (!isInitialized_ || !device_)
    {
        PIXL_LOG_AND_RETURN_ERROR(InvalidHandle, pixl::core::Errc::UninitialisedSubsystem, "gfx::Gpu", "GPU not initialized");
    }

    SDL_GPUTextureFormat fmt = ToGPUFormat(desc.format);
    if (fmt == SDL_GPU_TEXTUREFORMAT_INVALID)
    {
        PIXL_LOG_AND_RETURN_ERROR(InvalidHandle, pixl::core::Errc::InvalidParameter, "gfx::Gpu", "Unsupported pixel format");
    }

    SDL_GPUTextureCreateInfo createInfo{};
    createInfo.type = SDL_GPU_TEXTURETYPE_2D;
    createInfo.format = fmt;
    createInfo.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER | (desc.renderTarget ? SDL_GPU_TEXTUREUSAGE_COLOR_TARGET : 0);
    createInfo.width = static_cast<Uint32>(desc.width);
    createInfo.height = static_cast<Uint32>(desc.height);
    createInfo.layer_count_or_depth = 1;
    createInfo.num_levels = 1;
    createInfo.sample_count = SDL_GPU_SAMPLECOUNT_1;
    createInfo.props = 0;

    SDL_GPUTexture *texture = SDL_CreateGPUTexture(device_, &createInfo);
    if (!texture)
    {
        PIXL_LOG_AND_RETURN_ERROR(InvalidHandle, pixl::core::Errc::UnknownError, "gfx::Gpu", "Failed to create GPU texture: {}", SDL_GetError());
    }

    SDL_GPUSamplerCreateInfo sinfo{};
    sinfo.min_filter = ToGPUFilter(desc.minFilter);
    sinfo.mag_filter = ToGPUFilter(desc.magFilter);
    sinfo.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
    sinfo.address_mode_u = ToGPUAddress(desc.wrapU);
    sinfo.address_mode_v = ToGPUAddress(desc.wrapV);
    sinfo.address_mode_w = ToGPUAddress(desc.wrapV);
    sinfo.mip_lod_bias = 0.0f;
    sinfo.max_anisotropy = 1.0f;
    sinfo.compare_op = SDL_GPU_COMPAREOP_ALWAYS;
    sinfo.min_lod = 0.0f;
    sinfo.max_lod = 0.0f;
    sinfo.enable_anisotropy = false;
    sinfo.enable_compare = false;
    sinfo.props = 0;

    SDL_GPUSampler *sampler = SDL_CreateGPUSampler(device_, &sinfo);
    if (!sampler)
    {
        SDL_ReleaseGPUTexture(device_, texture);
        PIXL_LOG_AND_RETURN_ERROR(InvalidHandle, pixl::core::Errc::UnknownError, "gfx::Gpu", "Failed to create GPU sampler: {}", SDL_GetError());
    }

    TextureResource resource{};
    resource.texture = texture;
    resource.sampler = sampler;
    resource.desc = desc;
    resource.defaultSampler = InvalidHandle;

    TextureHandle handle = AllocateHandle(textures_, freeTextureSlots_, std::move(resource));

    if (initialData && dataSize > 0)
    {
        TextureResource *res = GetTextureResource(handle);
        if (res && !UploadTextureData(*res, initialData, dataSize))
        {
            DestroyTexture(handle);
            return InvalidHandle;
        }
    }

    return handle;
}

bool Gpu::UpdateTexture(TextureHandle handle, const void *data, size_t dataSize)
{
    TextureResource *res = GetTextureResource(handle);
    if (!res || !res->texture)
    {
        PIXL_LOG_ERROR(pixl::core::Errc::InvalidParameter, "gfx::Gpu", "UpdateTexture called with invalid handle");
        return false;
    }

    return UploadTextureData(*res, data, dataSize);
}

void Gpu::DestroyTexture(TextureHandle handle)
{
    TextureResource *res = GetTextureResource(handle);
    if (res)
    {
        if (res->texture)
        {
            SDL_ReleaseGPUTexture(device_, res->texture);
        }
        if (res->sampler)
        {
            SDL_ReleaseGPUSampler(device_, res->sampler);
        }
    }
    FreeHandle(textures_, freeTextureSlots_, handle);
}

BufferHandle Gpu::CreateBuffer(const BufferDesc &desc, const void *initialData, size_t dataSize)
{
    if (!isInitialized_ || !device_)
    {
        PIXL_LOG_AND_RETURN_ERROR(InvalidHandle, pixl::core::Errc::UninitialisedSubsystem, "gfx::Gpu", "GPU not initialized");
    }

    SDL_GPUBufferCreateInfo createInfo{};
    createInfo.usage = ToGPUUsage(desc.usage);
    createInfo.size = static_cast<Uint32>(desc.size);
    createInfo.props = 0;

    SDL_GPUBuffer *buffer = SDL_CreateGPUBuffer(device_, &createInfo);
    if (!buffer)
    {
        PIXL_LOG_AND_RETURN_ERROR(InvalidHandle, pixl::core::Errc::UnknownError, "gfx::Gpu", "Failed to create GPU buffer: {}", SDL_GetError());
    }

    BufferResource resource{};
    resource.buffer = buffer;
    resource.desc = desc;

    BufferHandle handle = AllocateHandle(buffers_, freeBufferSlots_, std::move(resource));

    if (initialData && dataSize > 0)
    {
        BufferResource *res = GetBufferResource(handle);
        if (res && !UploadBufferData(*res, initialData, dataSize))
        {
            DestroyBuffer(handle);
            return InvalidHandle;
        }
    }

    return handle;
}

bool Gpu::UpdateBuffer(BufferHandle handle, const void *data, size_t dataSize)
{
    BufferResource *res = GetBufferResource(handle);
    if (!res || !res->buffer)
    {
        PIXL_LOG_ERROR(pixl::core::Errc::InvalidParameter, "gfx::Gpu", "UpdateBuffer called with invalid handle");
        return false;
    }

    return UploadBufferData(*res, data, dataSize);
}

void Gpu::DestroyBuffer(BufferHandle handle)
{
    BufferResource *res = GetBufferResource(handle);
    if (res && res->buffer)
    {
        SDL_ReleaseGPUBuffer(device_, res->buffer);
    }
    FreeHandle(buffers_, freeBufferSlots_, handle);
}

ShaderHandle Gpu::CreateShader(const ShaderDesc &desc)
{
    if (!isInitialized_ || !device_)
    {
        PIXL_LOG_AND_RETURN_ERROR(InvalidHandle, pixl::core::Errc::UninitialisedSubsystem, "gfx::Gpu", "GPU not initialized");
    }

    if (desc.stages.size() != 1)
    {
        PIXL_LOG_AND_RETURN_ERROR(InvalidHandle, pixl::core::Errc::InvalidParameter, "gfx::Gpu", "ShaderDesc must contain exactly one stage; create one handle per stage");
    }

    const ShaderStageSource &stage = desc.stages.front();
    if (stage.stage == ShaderStage::Compute)
    {
        PIXL_LOG_AND_RETURN_ERROR(InvalidHandle, pixl::core::Errc::InvalidParameter, "gfx::Gpu", "Compute shaders not yet supported");
    }

    SDL_GPUShaderCreateInfo createInfo{};
    createInfo.code_size = stage.source.size();
    createInfo.code = reinterpret_cast<const Uint8 *>(stage.source.data());
    createInfo.entrypoint = stage.entryPoint.c_str();
    createInfo.format = ToGPUShaderFormat(stage.language);
    createInfo.stage = ToGPUShaderStage(stage.stage);
    createInfo.num_samplers = 0;
    createInfo.num_storage_textures = 0;
    createInfo.num_storage_buffers = 0;
    createInfo.num_uniform_buffers = 0;
    createInfo.props = 0;

    if (createInfo.format == SDL_GPU_SHADERFORMAT_INVALID)
    {
        PIXL_LOG_AND_RETURN_ERROR(InvalidHandle, pixl::core::Errc::InvalidParameter, "gfx::Gpu", "Invalid shader language/format");
    }

    SDL_GPUShader *shader = SDL_CreateGPUShader(device_, &createInfo);
    if (!shader)
    {
        PIXL_LOG_AND_RETURN_ERROR(InvalidHandle, pixl::core::Errc::UnknownError, "gfx::Gpu", "SDL_CreateGPUShader failed: {}", SDL_GetError());
    }

    ShaderResource resource{};
    resource.shader = shader;
    resource.desc = desc;

    return AllocateHandle(shaders_, freeShaderSlots_, std::move(resource));
}

void Gpu::DestroyShader(ShaderHandle handle)
{
    ShaderResource *res = GetShaderResource(handle);
    if (res && res->shader)
    {
        SDL_ReleaseGPUShader(device_, res->shader);
    }
    FreeHandle(shaders_, freeShaderSlots_, handle);
}

PipelineHandle Gpu::CreateGraphicsPipeline(const GraphicsPipelineDesc &desc)
{
    if (!isInitialized_ || !device_)
    {
        PIXL_LOG_AND_RETURN_ERROR(InvalidHandle, pixl::core::Errc::UninitialisedSubsystem, "gfx::Gpu", "GPU not initialized");
    }

    ShaderResource *vs = GetShaderResource(desc.vertexShader);
    ShaderResource *fs = GetShaderResource(desc.fragmentShader);
    if (!vs || !fs)
    {
        PIXL_LOG_AND_RETURN_ERROR(InvalidHandle, pixl::core::Errc::InvalidParameter, "gfx::Gpu", "Invalid shader handles for pipeline");
    }

    std::vector<SDL_GPUVertexBufferDescription> bufferDescs;
    bufferDescs.reserve(desc.vertexLayout.buffers.size());
    for (uint32_t i = 0; i < desc.vertexLayout.buffers.size(); ++i)
    {
        SDL_GPUVertexBufferDescription b{};
        b.slot = i;
        b.pitch = desc.vertexLayout.buffers[i].stride;
        b.input_rate = desc.vertexLayout.buffers[i].rate == VertexInputRate::PerInstance ? SDL_GPU_VERTEXINPUTRATE_INSTANCE : SDL_GPU_VERTEXINPUTRATE_VERTEX;
        bufferDescs.push_back(b);
    }

    std::vector<SDL_GPUVertexAttribute> attrDescs;
    attrDescs.reserve(desc.vertexLayout.attributes.size());
    for (const auto &attr : desc.vertexLayout.attributes)
    {
        SDL_GPUVertexAttribute a{};
        a.location = attr.location;
        a.buffer_slot = attr.bufferSlot;
        a.format = ToGPUVertexFormat(attr.format);
        a.offset = attr.offset;
        if (a.format == SDL_GPU_VERTEXELEMENTFORMAT_INVALID)
        {
            PIXL_LOG_AND_RETURN_ERROR(InvalidHandle, pixl::core::Errc::InvalidParameter, "gfx::Gpu", "Invalid vertex format in pipeline");
        }
        attrDescs.push_back(a);
    }

    SDL_GPUColorTargetDescription colorTarget{};
    colorTarget.format = swapchainFormat_;
    colorTarget.blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
    colorTarget.blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
    colorTarget.blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
    colorTarget.blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
    colorTarget.blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
    colorTarget.blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
    colorTarget.blend_state.color_write_mask = SDL_GPU_COLORCOMPONENT_R | SDL_GPU_COLORCOMPONENT_G | SDL_GPU_COLORCOMPONENT_B | SDL_GPU_COLORCOMPONENT_A;
    colorTarget.blend_state.enable_blend = false;

    SDL_GPUGraphicsPipelineTargetInfo targetInfo{};
    targetInfo.color_target_descriptions = &colorTarget;
    targetInfo.num_color_targets = 1;
    targetInfo.depth_stencil_format = desc.depthTest ? depthFormat_ : SDL_GPU_TEXTUREFORMAT_INVALID;
    targetInfo.has_depth_stencil_target = desc.depthTest;

    SDL_GPURasterizerState rast{};
    rast.fill_mode = SDL_GPU_FILLMODE_FILL;
    rast.cull_mode = SDL_GPU_CULLMODE_NONE;
    rast.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;
    rast.depth_bias_constant_factor = 0.0f;
    rast.depth_bias_clamp = 0.0f;
    rast.depth_bias_slope_factor = 0.0f;
    rast.enable_depth_bias = false;
    rast.enable_depth_clip = true;

    SDL_GPUMultisampleState ms{};
    ms.sample_count = SDL_GPU_SAMPLECOUNT_1;
    ms.sample_mask = 0;
    ms.enable_mask = false;

    SDL_GPUDepthStencilState ds{};
    ds.enable_depth_test = desc.depthTest;
    ds.enable_depth_write = desc.depthWrite && desc.depthTest;
    ds.enable_stencil_test = false;
    ds.compare_op = SDL_GPU_COMPAREOP_LESS;
    ds.front_stencil_state = {};
    ds.back_stencil_state = {};
    ds.compare_mask = 0xFF;
    ds.write_mask = 0xFF;

    SDL_GPUGraphicsPipelineCreateInfo pinfo{};
    pinfo.vertex_shader = vs->shader;
    pinfo.fragment_shader = fs->shader;
    pinfo.vertex_input_state.vertex_buffer_descriptions = bufferDescs.data();
    pinfo.vertex_input_state.num_vertex_buffers = static_cast<Uint32>(bufferDescs.size());
    pinfo.vertex_input_state.vertex_attributes = attrDescs.data();
    pinfo.vertex_input_state.num_vertex_attributes = static_cast<Uint32>(attrDescs.size());
    pinfo.primitive_type = ToGPUPrimitive(desc.primitive);
    pinfo.rasterizer_state = rast;
    pinfo.multisample_state = ms;
    pinfo.depth_stencil_state = ds;
    pinfo.target_info = targetInfo;
    pinfo.props = 0;

    SDL_GPUGraphicsPipeline *pipeline = SDL_CreateGPUGraphicsPipeline(device_, &pinfo);
    if (!pipeline)
    {
        PIXL_LOG_AND_RETURN_ERROR(InvalidHandle, pixl::core::Errc::UnknownError, "gfx::Gpu", "Failed to create graphics pipeline: {}", SDL_GetError());
    }

    PipelineResource res{};
    res.pipeline = pipeline;
    res.desc = desc;
    return AllocateHandle(pipelines_, freePipelineSlots_, std::move(res));
}

void Gpu::DestroyPipeline(PipelineHandle handle)
{
    PipelineResource *res = GetPipelineResource(handle);
    if (res && res->pipeline)
    {
        SDL_ReleaseGPUGraphicsPipeline(device_, res->pipeline);
    }
    FreeHandle(pipelines_, freePipelineSlots_, handle);
}

SamplerHandle Gpu::CreateSampler(TextureFilter min, TextureFilter mag, TextureWrap wrapU, TextureWrap wrapV)
{
    if (!isInitialized_ || !device_)
    {
        PIXL_LOG_AND_RETURN_ERROR(InvalidHandle, pixl::core::Errc::UninitialisedSubsystem, "gfx::Gpu", "GPU not initialized");
    }

    SDL_GPUSamplerCreateInfo sinfo{};
    sinfo.min_filter = ToGPUFilter(min);
    sinfo.mag_filter = ToGPUFilter(mag);
    sinfo.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
    sinfo.address_mode_u = ToGPUAddress(wrapU);
    sinfo.address_mode_v = ToGPUAddress(wrapV);
    sinfo.address_mode_w = ToGPUAddress(wrapV);
    sinfo.mip_lod_bias = 0.0f;
    sinfo.max_anisotropy = 1.0f;
    sinfo.compare_op = SDL_GPU_COMPAREOP_ALWAYS;
    sinfo.min_lod = 0.0f;
    sinfo.max_lod = 0.0f;
    sinfo.enable_anisotropy = false;
    sinfo.enable_compare = false;
    sinfo.props = 0;

    SDL_GPUSampler *sampler = SDL_CreateGPUSampler(device_, &sinfo);
    if (!sampler)
    {
        PIXL_LOG_AND_RETURN_ERROR(InvalidHandle, pixl::core::Errc::UnknownError, "gfx::Gpu", "Failed to create sampler: {}", SDL_GetError());
    }

    SamplerResource res{};
    res.sampler = sampler;
    return AllocateHandle(samplers_, freeSamplerSlots_, std::move(res));
}

void Gpu::DestroySampler(SamplerHandle handle)
{
    SamplerResource *res = GetSamplerResource(handle);
    if (res && res->sampler)
    {
        SDL_ReleaseGPUSampler(device_, res->sampler);
    }
    FreeHandle(samplers_, freeSamplerSlots_, handle);
}

bool Gpu::CreateGBuffer(uint32_t width, uint32_t height)
{
    DestroyGBuffer();

    TextureDesc common{};
    common.width = static_cast<int>(width);
    common.height = static_cast<int>(height);
    common.wrapU = TextureWrap::ClampToEdge;
    common.wrapV = TextureWrap::ClampToEdge;
    common.renderTarget = true;
    common.minFilter = TextureFilter::Nearest;
    common.magFilter = TextureFilter::Nearest;

    TextureDesc albedoDesc = common;
    albedoDesc.format = PixelFormat::RGBA8;
    TextureHandle albedo = CreateTexture(albedoDesc);
    if (albedo == InvalidHandle)
    {
        return false;
    }

    TextureDesc normalDesc = common;
    normalDesc.format = PixelFormat::RGBA16F;
    TextureHandle normal = CreateTexture(normalDesc);
    if (normal == InvalidHandle)
    {
        DestroyTexture(albedo);
        return false;
    }

    TextureDesc materialDesc = common;
    materialDesc.format = PixelFormat::RGBA8;
    TextureHandle material = CreateTexture(materialDesc);
    if (material == InvalidHandle)
    {
        DestroyTexture(albedo);
        DestroyTexture(normal);
        return false;
    }

    gbuffer_ = GBuffer{albedo, normal, material, width, height};
    return true;
}

void Gpu::DestroyGBuffer()
{
    if (!gbuffer_.has_value())
    {
        return;
    }
    DestroyTexture(gbuffer_->albedo);
    DestroyTexture(gbuffer_->normal);
    DestroyTexture(gbuffer_->material);
    gbuffer_.reset();
}

int Gpu::BeginPass(const RenderPassDesc &desc, const std::function<void(SDL_GPUCommandBuffer *)> &prePass)
{
    if (!device_ || !window_)
    {
        PIXL_LOG_AND_RETURN_ERROR(-1, pixl::core::Errc::UninitialisedSubsystem, "gfx::Gpu", "GPU not initialized");
    }

    if (activeRenderPass_ != nullptr || activeCommandBuffer_ != nullptr)
    {
        PIXL_LOG_AND_RETURN_ERROR(-1, pixl::core::Errc::InvalidParameter, "gfx::Gpu", "BeginPass called while a pass is already active");
    }

    activeCommandBuffer_ = SDL_AcquireGPUCommandBuffer(device_);
    if (!activeCommandBuffer_)
    {
        PIXL_LOG_AND_RETURN_ERROR(-1, pixl::core::Errc::UnknownError, "gfx::Gpu", "Failed to acquire command buffer: {}", SDL_GetError());
    }

    SDL_GPUColorTargetInfo colorTargets[3];
    Uint32 numTargets = 0;
    Uint32 passWidth = 0;
    Uint32 passHeight = 0;
    auto getWindowSize = [&]() -> std::pair<Uint32, Uint32>
    {
        int w = 0;
        int h = 0;
        if (window_ && window_->GetSDLWindow())
        {
            SDL_GetWindowSizeInPixels(window_->GetSDLWindow(), &w, &h);
        }
        return {static_cast<Uint32>(std::max(1, w)), static_cast<Uint32>(std::max(1, h))};
    };

    currentUseGBuffer_ = desc.useGBuffer;
    currentLightingBlit_ = desc.lightingBlit && desc.useGBuffer;

    if (desc.useGBuffer)
    {
        auto [winW, winH] = getWindowSize();
        passWidth = winW;
        passHeight = winH;
        if (!gbuffer_.has_value() || gbuffer_->width != passWidth || gbuffer_->height != passHeight)
        {
            if (!CreateGBuffer(passWidth, passHeight))
            {
                activeCommandBuffer_ = nullptr;
                PIXL_LOG_AND_RETURN_ERROR(-1, pixl::core::Errc::UnknownError, "gfx::Gpu", "Failed to create GBuffer");
            }
        }

        if (!gbuffer_.has_value())
        {
            activeCommandBuffer_ = nullptr;
            PIXL_LOG_AND_RETURN_ERROR(-1, pixl::core::Errc::InvalidParameter, "gfx::Gpu", "useGBuffer requested but no GBuffer created");
        }

        const GBuffer &gb = gbuffer_.value();
        TextureResource *albedo = GetTextureResource(gb.albedo);
        TextureResource *normal = GetTextureResource(gb.normal);
        TextureResource *material = GetTextureResource(gb.material);
        if (!albedo || !normal || !material)
        {
            activeCommandBuffer_ = nullptr;
            PIXL_LOG_AND_RETURN_ERROR(-1, pixl::core::Errc::InvalidParameter, "gfx::Gpu", "GBuffer textures are invalid");
        }

        auto fillTarget = [&](SDL_GPUColorTargetInfo &tgt, TextureResource *res)
        {
            tgt.texture = res->texture;
            tgt.mip_level = 0;
            tgt.layer_or_depth_plane = 0;
            tgt.clear_color = SDL_FColor{desc.clearColorValue[0], desc.clearColorValue[1], desc.clearColorValue[2], desc.clearColorValue[3]};
            tgt.load_op = desc.clearColor ? SDL_GPU_LOADOP_CLEAR : SDL_GPU_LOADOP_LOAD;
            tgt.store_op = SDL_GPU_STOREOP_STORE;
            tgt.resolve_texture = nullptr;
            tgt.resolve_mip_level = 0;
            tgt.resolve_layer = 0;
            tgt.cycle = true;
            tgt.cycle_resolve_texture = false;
        };

        fillTarget(colorTargets[0], albedo);
        fillTarget(colorTargets[1], normal);
        fillTarget(colorTargets[2], material);
        numTargets = 3;
        passWidth = gb.width;
        passHeight = gb.height;
    }
    else
    {
        SDL_GPUColorTargetInfo target{};
        target.mip_level = 0;
        target.layer_or_depth_plane = 0;
        target.clear_color = SDL_FColor{desc.clearColorValue[0], desc.clearColorValue[1], desc.clearColorValue[2], desc.clearColorValue[3]};
        target.load_op = desc.clearColor ? SDL_GPU_LOADOP_CLEAR : SDL_GPU_LOADOP_LOAD;
        target.store_op = SDL_GPU_STOREOP_STORE;
        target.resolve_texture = nullptr;
        target.resolve_mip_level = 0;
        target.resolve_layer = 0;
        target.cycle = true;
        target.cycle_resolve_texture = false;

        int acquire = AcquireSwapchain(&swapchainTexture_, swapchainWidth_, swapchainHeight_);
        if (acquire == 1)
        {
            auto [winW, winH] = getWindowSize();
            if (!EnsureFallbackColorTarget(winW, winH))
            {
                activeCommandBuffer_ = nullptr;
                PIXL_LOG_AND_RETURN_ERROR(-1, pixl::core::Errc::UnknownError, "gfx::Gpu", "Failed to create fallback render target");
            }
            target.texture = fallbackColorTexture_;
            passWidth = fallbackWidth_;
            passHeight = fallbackHeight_;
            swapchainTexture_ = nullptr;
        }
        else if (acquire != 0)
        {
            activeCommandBuffer_ = nullptr;
            PIXL_LOG_AND_RETURN_ERROR(-1, pixl::core::Errc::UnknownError, "gfx::Gpu", "Failed to acquire swapchain texture: {}", SDL_GetError());
        }

        if (!target.texture)
        {
            target.texture = swapchainTexture_;
        }
        if (passWidth == 0 || passHeight == 0)
        {
            passWidth = swapchainWidth_;
            passHeight = swapchainHeight_;
        }

        colorTargets[0] = target;
        numTargets = 1;
    }

    swapchainWidth_ = passWidth;
    swapchainHeight_ = passHeight;

    SDL_GPUDepthStencilTargetInfo depthInfo{};
    SDL_GPUDepthStencilTargetInfo *depthPtr = nullptr;
    if (desc.useDepth)
    {
        if (!EnsureDepthTexture(passWidth, passHeight))
        {
            PIXL_LOG_AND_RETURN_ERROR(-1, pixl::core::Errc::UnknownError, "gfx::Gpu", "Failed to ensure depth texture");
        }

        depthInfo.texture = depthTexture_;
        depthInfo.clear_depth = desc.clearDepth ? desc.clearDepthValue : 1.0f;
        depthInfo.load_op = desc.clearDepth ? SDL_GPU_LOADOP_CLEAR : SDL_GPU_LOADOP_LOAD;
        depthInfo.store_op = SDL_GPU_STOREOP_STORE;
        depthInfo.stencil_load_op = desc.clearStencil ? SDL_GPU_LOADOP_CLEAR : SDL_GPU_LOADOP_LOAD;
        depthInfo.stencil_store_op = SDL_GPU_STOREOP_STORE;
        depthInfo.cycle = true;
        depthPtr = &depthInfo;
    }

    if (prePass)
    {
        prePass(activeCommandBuffer_);
    }

    activeRenderPass_ = SDL_BeginGPURenderPass(activeCommandBuffer_, colorTargets, numTargets, depthPtr);
    if (!activeRenderPass_)
    {
        activeCommandBuffer_ = nullptr;
        PIXL_LOG_AND_RETURN_ERROR(-1, pixl::core::Errc::UnknownError, "gfx::Gpu", "Failed to begin render pass: {}", SDL_GetError());
    }

    return 0;
}

int Gpu::EndPass()
{
    if (!activeRenderPass_ || !activeCommandBuffer_)
    {
        PIXL_LOG_AND_RETURN_ERROR(-1, pixl::core::Errc::InvalidParameter, "gfx::Gpu", "EndPass called without an active pass");
    }

    SDL_EndGPURenderPass(activeRenderPass_);
    activeRenderPass_ = nullptr;

    if (!SDL_SubmitGPUCommandBuffer(activeCommandBuffer_))
    {
        activeCommandBuffer_ = nullptr;
        PIXL_LOG_AND_RETURN_ERROR(-1, pixl::core::Errc::UnknownError, "gfx::Gpu", "Failed to submit command buffer: {}", SDL_GetError());
    }

    activeCommandBuffer_ = nullptr;
    swapchainTexture_ = nullptr;
    swapchainWidth_ = 0;
    swapchainHeight_ = 0;
    ReleaseSwapchain();

    if (currentLightingBlit_ && gbuffer_.has_value())
    {
        BlitToSwapchain(gbuffer_->albedo);
    }
    currentUseGBuffer_ = false;
    currentLightingBlit_ = false;
    return 0;
}

bool Gpu::BindPipeline(PipelineHandle handle)
{
    PipelineResource *res = GetPipelineResource(handle);
    if (!res || !res->pipeline || !activeRenderPass_)
    {
        PIXL_LOG_ERROR(pixl::core::Errc::InvalidParameter, "gfx::Gpu", "BindPipeline called with invalid handle or no active pass");
        return false;
    }

    SDL_BindGPUGraphicsPipeline(activeRenderPass_, res->pipeline);
    boundPipeline_ = res->pipeline;
    return true;
}

bool Gpu::BindVertexBuffer(BufferHandle handle, uint32_t slot, uint32_t offsetBytes)
{
    BufferResource *res = GetBufferResource(handle);
    if (!res || !res->buffer)
    {
        PIXL_LOG_ERROR(pixl::core::Errc::InvalidParameter, "gfx::Gpu", "BindVertexBuffer called with invalid buffer");
        return false;
    }

    if (slot >= vertexBindings_.size())
    {
        vertexBindings_.resize(slot + 1);
    }

    SDL_GPUBufferBinding binding{};
    binding.buffer = res->buffer;
    binding.offset = offsetBytes;
    vertexBindings_[slot] = binding;
    return true;
}

bool Gpu::BindIndexBuffer(BufferHandle handle, bool indexSize16Bit, uint32_t offsetBytes)
{
    BufferResource *res = GetBufferResource(handle);
    if (!res || !res->buffer)
    {
        PIXL_LOG_ERROR(pixl::core::Errc::InvalidParameter, "gfx::Gpu", "BindIndexBuffer called with invalid buffer");
        return false;
    }

    indexBuffer_ = res->buffer;
    indexElementSize_ = indexSize16Bit ? SDL_GPU_INDEXELEMENTSIZE_16BIT : SDL_GPU_INDEXELEMENTSIZE_32BIT;
    indexOffsetBytes_ = offsetBytes;
    return true;
}

bool Gpu::BindFragmentTexture(uint32_t slot, TextureHandle textureHandle, SamplerHandle samplerHandle)
{
    if (!activeRenderPass_)
    {
        PIXL_LOG_ERROR(pixl::core::Errc::InvalidParameter, "gfx::Gpu", "BindFragmentTexture called without an active pass");
        return false;
    }

    TextureResource *tex = GetTextureResource(textureHandle);
    if (!tex || !tex->texture)
    {
        PIXL_LOG_ERROR(pixl::core::Errc::InvalidParameter, "gfx::Gpu", "BindFragmentTexture invalid texture handle");
        return false;
    }

    SamplerResource *samplerRes = nullptr;
    if (samplerHandle != InvalidHandle)
    {
        samplerRes = GetSamplerResource(samplerHandle);
        if (!samplerRes || !samplerRes->sampler)
        {
            PIXL_LOG_ERROR(pixl::core::Errc::InvalidParameter, "gfx::Gpu", "BindFragmentTexture invalid sampler handle");
            return false;
        }
    }

    SDL_GPUTextureSamplerBinding binding{};
    binding.texture = tex->texture;
    binding.sampler = samplerHandle != InvalidHandle ? samplerRes->sampler : tex->sampler;

    SDL_BindGPUFragmentSamplers(activeRenderPass_, slot, &binding, 1);
    return true;
}

MaterialHandle Gpu::CreateMaterial(const MaterialDesc &desc)
{
    if (!isInitialized_)
    {
        PIXL_LOG_AND_RETURN_ERROR(InvalidHandle, pixl::core::Errc::UninitialisedSubsystem, "gfx::Gpu", "GPU not initialized");
    }
    MaterialResource res{};
    res.desc = desc;
    return AllocateHandle(materials_, freeMaterialSlots_, std::move(res));
}

void Gpu::DestroyMaterial(MaterialHandle handle)
{
    FreeHandle(materials_, freeMaterialSlots_, handle);
}

bool Gpu::BindMaterial(MaterialHandle handle)
{
    if (!activeRenderPass_)
    {
        PIXL_LOG_ERROR(pixl::core::Errc::InvalidParameter, "gfx::Gpu", "BindMaterial called without an active pass");
        return false;
    }

    MaterialResource *mat = GetMaterialResource(handle);
    if (!mat)
    {
        PIXL_LOG_ERROR(pixl::core::Errc::InvalidParameter, "gfx::Gpu", "BindMaterial invalid handle");
        return false;
    }

    if (mat->desc.pipeline != InvalidHandle)
    {
        if (!BindPipeline(mat->desc.pipeline))
        {
            return false;
        }
    }

    for (uint32_t i = 0; i < mat->desc.textures.size(); ++i)
    {
        TextureHandle handle = InvalidHandle;
        if (mat->desc.textures[i])
        {
            handle = mat->desc.textures[i]->handle;
        }
        if (handle != InvalidHandle)
        {
            BindFragmentTexture(i, handle, mat->desc.samplers[i]);
        }
    }
    return true;
}

void Gpu::Draw(uint32_t vertexCount, uint32_t firstVertex, uint32_t instanceCount)
{
    if (!activeRenderPass_ || !boundPipeline_)
    {
        PIXL_LOG_ERROR(pixl::core::Errc::InvalidParameter, "gfx::Gpu", "Draw called without active pass/pipeline");
        return;
    }

    if (!vertexBindings_.empty())
    {
        Uint32 count = static_cast<Uint32>(vertexBindings_.size());
        SDL_BindGPUVertexBuffers(activeRenderPass_, 0, vertexBindings_.data(), count);
    }

    SDL_DrawGPUPrimitives(activeRenderPass_, vertexCount, instanceCount, firstVertex, 0);
}

void Gpu::DrawIndexed(uint32_t indexCount, uint32_t firstIndex, uint32_t instanceCount, int32_t vertexOffset)
{
    if (!activeRenderPass_ || !boundPipeline_ || !indexBuffer_)
    {
        PIXL_LOG_ERROR(pixl::core::Errc::InvalidParameter, "gfx::Gpu", "DrawIndexed missing pass/pipeline/index buffer");
        return;
    }

    if (!vertexBindings_.empty())
    {
        Uint32 count = static_cast<Uint32>(vertexBindings_.size());
        SDL_BindGPUVertexBuffers(activeRenderPass_, 0, vertexBindings_.data(), count);
    }

    SDL_GPUBufferBinding ibo{};
    ibo.buffer = indexBuffer_;
    ibo.offset = indexOffsetBytes_;
    SDL_BindGPUIndexBuffer(activeRenderPass_, &ibo, indexElementSize_);
    SDL_DrawGPUIndexedPrimitives(activeRenderPass_, indexCount, instanceCount, firstIndex, vertexOffset, 0);
}

bool Gpu::SetViewport(float x, float y, float w, float h, float minDepth, float maxDepth)
{
    if (!activeRenderPass_)
    {
        return false;
    }
    SDL_GPUViewport vp{};
    vp.x = x;
    vp.y = y;
    vp.w = w;
    vp.h = h;
    vp.min_depth = minDepth;
    vp.max_depth = maxDepth;
    SDL_SetGPUViewport(activeRenderPass_, &vp);
    return true;
}

bool Gpu::ResetViewport()
{
    if (!activeRenderPass_)
    {
        return false;
    }
    SDL_GPUViewport vp{};
    vp.x = 0.0f;
    vp.y = 0.0f;
    vp.w = static_cast<float>(swapchainWidth_);
    vp.h = static_cast<float>(swapchainHeight_);
    vp.min_depth = 0.0f;
    vp.max_depth = 1.0f;
    SDL_SetGPUViewport(activeRenderPass_, &vp);
    return true;
}

bool Gpu::EnsureDepthTexture(uint32_t width, uint32_t height)
{
    if (depthTexture_ && depthWidth_ == width && depthHeight_ == height)
    {
        return true;
    }

    if (depthTexture_)
    {
        SDL_ReleaseGPUTexture(device_, depthTexture_);
        depthTexture_ = nullptr;
    }

    SDL_GPUTextureCreateInfo info{};
    info.type = SDL_GPU_TEXTURETYPE_2D;
    info.format = depthFormat_;
    info.usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET;
    info.width = width;
    info.height = height;
    info.layer_count_or_depth = 1;
    info.num_levels = 1;
    info.sample_count = SDL_GPU_SAMPLECOUNT_1;
    info.props = 0;

    depthTexture_ = SDL_CreateGPUTexture(device_, &info);
    if (!depthTexture_)
    {
        PIXL_LOG_ERROR(pixl::core::Errc::UnknownError, "gfx::Gpu", "Failed to create depth texture: {}", SDL_GetError());
        return false;
    }
    depthWidth_ = width;
    depthHeight_ = height;
    return true;
}

bool Gpu::EnsureFallbackColorTarget(uint32_t width, uint32_t height)
{
    if (!device_)
    {
        PIXL_LOG_ERROR(pixl::core::Errc::UninitialisedSubsystem, "gfx::Gpu", "GPU not initialized for fallback target");
        return false;
    }

    Uint32 targetW = width > 0 ? width : 1u;
    Uint32 targetH = height > 0 ? height : 1u;

    if (fallbackColorTexture_ && fallbackWidth_ == targetW && fallbackHeight_ == targetH)
    {
        return true;
    }

    if (fallbackColorTexture_)
    {
        SDL_ReleaseGPUTexture(device_, fallbackColorTexture_);
        fallbackColorTexture_ = nullptr;
    }

    SDL_GPUTextureCreateInfo info{};
    info.type = SDL_GPU_TEXTURETYPE_2D;
    info.format = swapchainFormat_;
    info.usage = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET;
    info.width = targetW;
    info.height = targetH;
    info.layer_count_or_depth = 1;
    info.num_levels = 1;
    info.sample_count = SDL_GPU_SAMPLECOUNT_1;
    info.props = 0;

    fallbackColorTexture_ = SDL_CreateGPUTexture(device_, &info);
    if (!fallbackColorTexture_)
    {
        PIXL_LOG_ERROR(pixl::core::Errc::UnknownError, "gfx::Gpu", "Failed to create fallback color target: {}", SDL_GetError());
        return false;
    }

    fallbackWidth_ = targetW;
    fallbackHeight_ = targetH;
    return true;
}

int Gpu::AcquireSwapchain(SDL_GPUTexture **outTex, Uint32 &outW, Uint32 &outH)
{
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(activeCommandBuffer_, window_->GetSDLWindow(), outTex, &outW, &outH))
    {
        activeCommandBuffer_ = nullptr;
        return -1;
    }

    if (*outTex == nullptr)
    {
        // Window may be occluded or too many frames in flight; use an offscreen target instead.
        outW = 0;
        outH = 0;
        return 1;
    }

    return 0;
}

void Gpu::ReleaseSwapchain()
{
    // SDL_SubmitGPUCommandBuffer will present automatically; nothing explicit required here.
}

int Gpu::BlitToSwapchain(TextureHandle source)
{
    TextureResource *src = GetTextureResource(source);
    if (!src || !src->texture)
    {
        PIXL_LOG_AND_RETURN_ERROR(-1, pixl::core::Errc::InvalidParameter, "gfx::Gpu", "BlitToSwapchain invalid source texture");
    }

    SDL_GPUCommandBuffer *cmd = SDL_AcquireGPUCommandBuffer(device_);
    if (!cmd)
    {
        PIXL_LOG_AND_RETURN_ERROR(-1, pixl::core::Errc::UnknownError, "gfx::Gpu", "Failed to acquire command buffer for blit: {}", SDL_GetError());
    }

    SDL_GPUTexture *swapTex = nullptr;
    Uint32 w = 0, h = 0;
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmd, window_->GetSDLWindow(), &swapTex, &w, &h))
    {
        PIXL_LOG_AND_RETURN_ERROR(-1, pixl::core::Errc::UnknownError, "gfx::Gpu", "Failed to acquire swapchain texture for blit: {}", SDL_GetError());
    }

    SDL_GPUBlitInfo blit{};
    blit.source.texture = src->texture;
    blit.source.mip_level = 0;
    blit.source.layer_or_depth_plane = 0;
    blit.source.x = 0;
    blit.source.y = 0;
    blit.source.w = static_cast<Uint32>(src->desc.width);
    blit.source.h = static_cast<Uint32>(src->desc.height);

    blit.destination.texture = swapTex;
    blit.destination.mip_level = 0;
    blit.destination.layer_or_depth_plane = 0;
    blit.destination.x = 0;
    blit.destination.y = 0;
    blit.destination.w = w;
    blit.destination.h = h;

    blit.load_op = SDL_GPU_LOADOP_CLEAR;
    blit.clear_color = SDL_FColor{0, 0, 0, 1};

    SDL_BlitGPUTexture(cmd, &blit);

    if (!SDL_SubmitGPUCommandBuffer(cmd))
    {
        PIXL_LOG_AND_RETURN_ERROR(-1, pixl::core::Errc::UnknownError, "gfx::Gpu", "Failed to submit blit command buffer: {}", SDL_GetError());
    }

    return 0;
}

Gpu::TextureResource *Gpu::GetTextureResource(TextureHandle handle)
{
    return ResolveHandle(textures_, handle);
}

Gpu::BufferResource *Gpu::GetBufferResource(BufferHandle handle)
{
    return ResolveHandle(buffers_, handle);
}

Gpu::ShaderResource *Gpu::GetShaderResource(ShaderHandle handle)
{
    return ResolveHandle(shaders_, handle);
}

Gpu::PipelineResource *Gpu::GetPipelineResource(PipelineHandle handle)
{
    return ResolveHandle(pipelines_, handle);
}

Gpu::SamplerResource *Gpu::GetSamplerResource(SamplerHandle handle)
{
    return ResolveHandle(samplers_, handle);
}

Gpu::MaterialResource *Gpu::GetMaterialResource(MaterialHandle handle)
{
    return ResolveHandle(materials_, handle);
}

bool Gpu::UploadTextureData(TextureResource &resource, const void *data, size_t dataSize)
{
    if (!data || dataSize == 0)
    {
        return false;
    }

    size_t bpp = BytesPerPixel(resource.desc.format);
    size_t expected = static_cast<size_t>(resource.desc.width) * static_cast<size_t>(resource.desc.height) * bpp;
    if (expected == 0 || dataSize < expected)
    {
        PIXL_LOG_ERROR(pixl::core::Errc::InvalidParameter, "gfx::Gpu", "UploadTextureData data size mismatch ({} < {})", dataSize, expected);
        return false;
    }

    SDL_GPUTransferBufferCreateInfo tinfo{};
    tinfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    tinfo.size = static_cast<Uint32>(dataSize);
    tinfo.props = 0;

    SDL_GPUTransferBuffer *transfer = SDL_CreateGPUTransferBuffer(device_, &tinfo);
    if (!transfer)
    {
        PIXL_LOG_ERROR(pixl::core::Errc::UnknownError, "gfx::Gpu", "Failed to create transfer buffer: {}", SDL_GetError());
        return false;
    }

    void *mapped = SDL_MapGPUTransferBuffer(device_, transfer, true);
    if (!mapped)
    {
        SDL_ReleaseGPUTransferBuffer(device_, transfer);
        PIXL_LOG_ERROR(pixl::core::Errc::UnknownError, "gfx::Gpu", "Failed to map transfer buffer: {}", SDL_GetError());
        return false;
    }

    std::memcpy(mapped, data, dataSize);
    SDL_UnmapGPUTransferBuffer(device_, transfer);

    SDL_GPUCommandBuffer *cmd = SDL_AcquireGPUCommandBuffer(device_);
    if (!cmd)
    {
        SDL_ReleaseGPUTransferBuffer(device_, transfer);
        PIXL_LOG_ERROR(pixl::core::Errc::UnknownError, "gfx::Gpu", "Failed to acquire command buffer for upload: {}", SDL_GetError());
        return false;
    }

    SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(cmd);
    SDL_GPUTextureTransferInfo src{};
    src.transfer_buffer = transfer;
    src.offset = 0;
    src.pixels_per_row = static_cast<Uint32>(resource.desc.width);
    src.rows_per_layer = static_cast<Uint32>(resource.desc.height);

    SDL_GPUTextureRegion dst{};
    dst.texture = resource.texture;
    dst.mip_level = 0;
    dst.layer = 0;
    dst.x = 0;
    dst.y = 0;
    dst.z = 0;
    dst.w = static_cast<Uint32>(resource.desc.width);
    dst.h = static_cast<Uint32>(resource.desc.height);
    dst.d = 1;

    SDL_UploadToGPUTexture(copyPass, &src, &dst, true);
    SDL_EndGPUCopyPass(copyPass);

    bool submitOk = SDL_SubmitGPUCommandBuffer(cmd);
    SDL_ReleaseGPUTransferBuffer(device_, transfer);

    if (!submitOk)
    {
        PIXL_LOG_ERROR(pixl::core::Errc::UnknownError, "gfx::Gpu", "Failed to submit texture upload: {}", SDL_GetError());
        return false;
    }

    return true;
}

bool Gpu::UploadBufferData(BufferResource &resource, const void *data, size_t dataSize)
{
    if (!data || dataSize == 0)
    {
        return false;
    }

    size_t copySize = std::min(static_cast<size_t>(resource.desc.size), dataSize);
    if (copySize == 0)
    {
        PIXL_LOG_ERROR(pixl::core::Errc::InvalidParameter, "gfx::Gpu", "UploadBufferData size is zero");
        return false;
    }

    SDL_GPUTransferBufferCreateInfo tinfo{};
    tinfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    tinfo.size = static_cast<Uint32>(copySize);
    tinfo.props = 0;

    SDL_GPUTransferBuffer *transfer = SDL_CreateGPUTransferBuffer(device_, &tinfo);
    if (!transfer)
    {
        PIXL_LOG_ERROR(pixl::core::Errc::UnknownError, "gfx::Gpu", "Failed to create transfer buffer: {}", SDL_GetError());
        return false;
    }

    void *mapped = SDL_MapGPUTransferBuffer(device_, transfer, true);
    if (!mapped)
    {
        SDL_ReleaseGPUTransferBuffer(device_, transfer);
        PIXL_LOG_ERROR(pixl::core::Errc::UnknownError, "gfx::Gpu", "Failed to map transfer buffer: {}", SDL_GetError());
        return false;
    }

    std::memcpy(mapped, data, copySize);
    SDL_UnmapGPUTransferBuffer(device_, transfer);

    SDL_GPUCommandBuffer *cmd = SDL_AcquireGPUCommandBuffer(device_);
    if (!cmd)
    {
        SDL_ReleaseGPUTransferBuffer(device_, transfer);
        PIXL_LOG_ERROR(pixl::core::Errc::UnknownError, "gfx::Gpu", "Failed to acquire command buffer for buffer upload: {}", SDL_GetError());
        return false;
    }

    SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(cmd);

    SDL_GPUTransferBufferLocation source{};
    source.transfer_buffer = transfer;
    source.offset = 0;

    SDL_GPUBufferRegion dest{};
    dest.buffer = resource.buffer;
    dest.offset = 0;
    dest.size = static_cast<Uint32>(copySize);

    SDL_UploadToGPUBuffer(copyPass, &source, &dest, true);
    SDL_EndGPUCopyPass(copyPass);

    bool submitOk = SDL_SubmitGPUCommandBuffer(cmd);
    SDL_ReleaseGPUTransferBuffer(device_, transfer);

    if (!submitOk)
    {
        PIXL_LOG_ERROR(pixl::core::Errc::UnknownError, "gfx::Gpu", "Failed to submit buffer upload: {}", SDL_GetError());
        return false;
    }

    return true;
}
