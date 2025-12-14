#include <algorithm>
#include <array>
#include <cmath>
#include <fstream>
#include <map>
#include <retro_renderer/retro_renderer.hpp>
#include <vector>

RetroRenderer::RetroRenderer()
    : gpu(nullptr),
      window(nullptr),
      screen_target(nullptr),
      depth_texture(nullptr),
      depth_format(SDL_GPU_TEXTUREFORMAT_INVALID),
      color_format(SDL_GPU_TEXTUREFORMAT_INVALID),
      depth_width(0),
      depth_height(0),
      frame_command_buffer(nullptr),
      frame_render_pass(nullptr),
      frame_swapchain_texture(nullptr),
      frame_active(false),
      color_vert_shader(nullptr),
      color_frag_shader(nullptr),
      textured_vert_shader(nullptr),
      textured_frag_shader(nullptr),
      color_pipeline(nullptr),
      color_pipeline_transparent(nullptr),
      textured_pipeline(nullptr),
      textured_pipeline_transparent(nullptr),
      line_pipeline(nullptr),
      texture_sampler(nullptr),
      color_vertex_buffer(nullptr),
      color_vertex_buffer_size(0),
      textured_vertex_buffer(nullptr),
      textured_vertex_buffer_size(0),
      color_index_buffer(nullptr),
      color_index_buffer_size(0),
      textured_index_buffer(nullptr),
      textured_index_buffer_size(0),
      directional_light_enabled(false),
      point_light_count(0),
      spot_light_count(0),
      view_matrix(glm::mat4(1.0f)),
      projection_matrix(glm::mat4(1.0f)),
      model_matrix(glm::mat4(1.0f)) {}

static constexpr size_t MAX_TRIANGLES_PER_BATCH = 1024;

static_assert(sizeof(Vertex) == 28, "Vertex layout mismatch");
static_assert(sizeof(TexturedVertex) == 36, "TexturedVertex layout mismatch");

struct alignas(16) MatricesUBO
{
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 model;
    glm::mat4 normal;
};

struct alignas(16) PointLightUBO
{
    glm::vec4 color_intensity;
    glm::vec4 position_constant;
    glm::vec4 attenuation;
};

struct alignas(16) SpotLightUBO
{
    glm::vec4 color_intensity;
    glm::vec4 position_constant;
    glm::vec4 direction_cutoff;
    glm::vec4 attenuation;
};

struct alignas(16) LightsUBO
{
    glm::vec4 ambient_light;
    glm::vec4 directional_color_intensity;
    glm::vec4 directional_direction;
    glm::ivec4 counts; // x: directional_enabled, y: point_count, z: spot_count
    std::array<PointLightUBO, 16> point_lights;
    std::array<SpotLightUBO, 16> spot_lights;
};

static bool ReadFile(const std::string &path, std::vector<Uint8> &out)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open())
        return false;
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    out.resize(static_cast<size_t>(size));
    return file.read(reinterpret_cast<char *>(out.data()), size).good();
}

static SDL_GPUShader *CreateShader(SDL_GPUDevice *gpu, const std::string &path, SDL_GPUShaderStage stage, Uint32 num_samplers, Uint32 num_uniform_buffers)
{
    std::vector<Uint8> code;
    if (!ReadFile(path, code))
    {
        SDL_Log("Failed to read shader file: %s", path.c_str());
        return nullptr;
    }

    SDL_GPUShaderCreateInfo info{
        .code_size = static_cast<size_t>(code.size()),
        .code = code.data(),
        .entrypoint = "main",
        .format = SDL_GPU_SHADERFORMAT_SPIRV,
        .stage = stage,
        .num_samplers = num_samplers,
        .num_storage_textures = 0,
        .num_storage_buffers = 0,
        .num_uniform_buffers = num_uniform_buffers,
        .props = 0,
    };

    SDL_GPUShader *shader = SDL_CreateGPUShader(gpu, &info);
    if (shader == nullptr)
    {
        SDL_Log("SDL_CreateGPUShader failed for %s: %s", path.c_str(), SDL_GetError());
    }
    return shader;
}

static SDL_GPUTextureFormat ChooseDepthFormat(SDL_GPUDevice *gpu)
{
    const SDL_GPUTextureFormat candidates[] = {
        SDL_GPU_TEXTUREFORMAT_D24_UNORM_S8_UINT,
        SDL_GPU_TEXTUREFORMAT_D32_FLOAT_S8_UINT,
        SDL_GPU_TEXTUREFORMAT_D32_FLOAT,
        SDL_GPU_TEXTUREFORMAT_D24_UNORM,
    };

    for (SDL_GPUTextureFormat fmt : candidates)
    {
        if (SDL_GPUTextureSupportsFormat(gpu, fmt, SDL_GPU_TEXTURETYPE_2D, SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET))
            return fmt;
    }
    return SDL_GPU_TEXTUREFORMAT_INVALID;
}

static SDL_GPUColorTargetBlendState MakeBlendState(bool enable_blend)
{
    SDL_GPUColorTargetBlendState blend{};
    blend.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    blend.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    blend.color_blend_op = SDL_GPU_BLENDOP_ADD;
    blend.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
    blend.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    blend.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
    blend.color_write_mask = SDL_GPU_COLORCOMPONENT_R | SDL_GPU_COLORCOMPONENT_G | SDL_GPU_COLORCOMPONENT_B | SDL_GPU_COLORCOMPONENT_A;
    blend.enable_blend = enable_blend;
    blend.enable_color_write_mask = true;
    return blend;
}

static SDL_GPUGraphicsPipeline *CreatePipeline(SDL_GPUDevice *gpu,
                                               SDL_GPUShader *vert,
                                               SDL_GPUShader *frag,
                                               const SDL_GPUVertexBufferDescription &vb_desc,
                                               const std::vector<SDL_GPUVertexAttribute> &attrs,
                                               SDL_GPUTextureFormat color_fmt,
                                               SDL_GPUTextureFormat depth_fmt,
                                               bool enable_blend,
                                               SDL_GPUCompareOp compare_op,
                                               bool enable_depth_test,
                                               bool enable_depth_write,
                                               bool need_depth_buffer,
                                               SDL_GPUPrimitiveType prim_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST)
{
    SDL_GPUColorTargetBlendState blend = MakeBlendState(enable_blend);
    SDL_GPUColorTargetDescription color_target{
        .format = color_fmt,
        .blend_state = blend,
    };

    if (vert == nullptr || frag == nullptr)
    {
        SDL_Log("CreatePipeline: null shader (vert=%p, frag=%p)", vert, frag);
        return nullptr;
    }

    SDL_GPUGraphicsPipelineTargetInfo target_info{
        .color_target_descriptions = &color_target,
        .num_color_targets = 1,
        .depth_stencil_format = need_depth_buffer ? depth_fmt : SDL_GPU_TEXTUREFORMAT_INVALID,
        .has_depth_stencil_target = need_depth_buffer,
    };

    SDL_GPURasterizerState raster_state{
        .fill_mode = SDL_GPU_FILLMODE_FILL,
        .cull_mode = SDL_GPU_CULLMODE_BACK,
        .front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE,
        .depth_bias_constant_factor = 0.0f,
        .depth_bias_clamp = 0.0f,
        .depth_bias_slope_factor = 0.0f,
        .enable_depth_bias = false,
        .enable_depth_clip = true,
    };

    SDL_GPUMultisampleState ms_state{
        .sample_count = SDL_GPU_SAMPLECOUNT_1,
        .sample_mask = 0,
        .enable_mask = false,
    };

    SDL_GPUDepthStencilState depth_state{};
    if (need_depth_buffer)
    {
        depth_state.compare_op = compare_op;
        depth_state.compare_mask = 0xFF;
        depth_state.write_mask = 0xFF;
        depth_state.enable_depth_test = enable_depth_test;
        depth_state.enable_depth_write = enable_depth_write;
        depth_state.enable_stencil_test = false;
    }

    SDL_GPUVertexInputState input_state{
        .vertex_buffer_descriptions = &vb_desc,
        .num_vertex_buffers = 1,
        .vertex_attributes = attrs.data(),
        .num_vertex_attributes = static_cast<Uint32>(attrs.size()),
    };

    SDL_GPUGraphicsPipelineCreateInfo info{
        .vertex_shader = vert,
        .fragment_shader = frag,
        .vertex_input_state = input_state,
        .primitive_type = prim_type,
        .rasterizer_state = raster_state,
        .multisample_state = ms_state,
        .depth_stencil_state = depth_state,
        .target_info = target_info,
        .props = 0,
    };

    SDL_GPUGraphicsPipeline *pipeline = SDL_CreateGPUGraphicsPipeline(gpu, &info);
    if (pipeline == nullptr)
    {
        SDL_Log("SDL_CreateGPUGraphicsPipeline failed: %s", SDL_GetError());
    }
    return pipeline;
}

static bool EnsureBufferWithUsage(SDL_GPUDevice *gpu, SDL_GPUBuffer *&buffer, size_t &current_size, size_t required_size, SDL_GPUBufferUsageFlags usage)
{
    if (buffer != nullptr && current_size >= required_size)
        return true;

    if (buffer != nullptr)
    {
        SDL_ReleaseGPUBuffer(gpu, buffer);
        buffer = nullptr;
    }

    const SDL_GPUBufferCreateInfo create_info{
        .usage = usage,
        .size = static_cast<Uint32>(required_size),
        .props = 0,
    };

    buffer = SDL_CreateGPUBuffer(gpu, &create_info);
    if (buffer != nullptr)
    {
        current_size = required_size;
        return true;
    }
    current_size = 0;
    return false;
}

static bool UploadVertexData(SDL_GPUDevice *gpu,
                             SDL_GPUCommandBuffer *command_buffer,
                             SDL_GPUBuffer *vertex_buffer,
                             const void *data,
                             size_t data_size)
{
    if (data_size == 0)
        return true;

    const SDL_GPUTransferBufferCreateInfo transfer_info{
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = static_cast<Uint32>(data_size),
        .props = 0,
    };

    SDL_GPUTransferBuffer *transfer = SDL_CreateGPUTransferBuffer(gpu, &transfer_info);
    if (transfer == nullptr)
        return false;

    void *mapped = SDL_MapGPUTransferBuffer(gpu, transfer, false);
    if (mapped == nullptr)
    {
        SDL_ReleaseGPUTransferBuffer(gpu, transfer);
        return false;
    }

    SDL_memcpy(mapped, data, data_size);
    SDL_UnmapGPUTransferBuffer(gpu, transfer);

    SDL_GPUCopyPass *copy_pass = SDL_BeginGPUCopyPass(command_buffer);
    if (copy_pass == nullptr)
    {
        SDL_ReleaseGPUTransferBuffer(gpu, transfer);
        return false;
    }

    SDL_GPUTransferBufferLocation src{
        .transfer_buffer = transfer,
        .offset = 0,
    };
    SDL_GPUBufferRegion dst{
        .buffer = vertex_buffer,
        .offset = 0,
        .size = static_cast<Uint32>(data_size),
    };

    SDL_UploadToGPUBuffer(copy_pass, &src, &dst, false);

    SDL_EndGPUCopyPass(copy_pass);

    SDL_ReleaseGPUTransferBuffer(gpu, transfer);
    return true;
}

static SDL_GPUBuffer *CreateAndUploadStaticBuffer(SDL_GPUDevice *gpu,
                                                  const void *data,
                                                  size_t data_size,
                                                  SDL_GPUBufferUsageFlags usage)
{
    if (data_size == 0 || data == nullptr)
        return nullptr;

    const SDL_GPUBufferCreateInfo create_info{
        .usage = usage,
        .size = static_cast<Uint32>(data_size),
        .props = 0,
    };

    SDL_GPUBuffer *buffer = SDL_CreateGPUBuffer(gpu, &create_info);
    if (buffer == nullptr)
        return nullptr;

    SDL_GPUCommandBuffer *command_buffer = SDL_AcquireGPUCommandBuffer(gpu);
    if (command_buffer == nullptr)
    {
        SDL_ReleaseGPUBuffer(gpu, buffer);
        return nullptr;
    }

    if (!UploadVertexData(gpu, command_buffer, buffer, data, data_size))
    {
        SDL_CancelGPUCommandBuffer(command_buffer);
        SDL_ReleaseGPUBuffer(gpu, buffer);
        return nullptr;
    }

    SDL_SubmitGPUCommandBuffer(command_buffer);
    return buffer;
}

static SDL_GPUTexture *CreateTextureFromSurface(SDL_GPUDevice *gpu, SDL_Surface *rgba_surface, int &error_code)
{
    error_code = 0;

    const Uint32 texture_width = static_cast<Uint32>(rgba_surface->w);
    const Uint32 texture_height = static_cast<Uint32>(rgba_surface->h);
    const Uint32 pitch = static_cast<Uint32>(rgba_surface->pitch);
    constexpr Uint32 bytes_per_pixel = 4; // SDL_PIXELFORMAT_RGBA32
    const Uint32 upload_size = pitch * texture_height;

    const SDL_GPUTextureCreateInfo texture_info{
        .type = SDL_GPU_TEXTURETYPE_2D,
        .format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
        .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER,
        .width = texture_width,
        .height = texture_height,
        .layer_count_or_depth = 1,
        .num_levels = 1,
        .sample_count = SDL_GPU_SAMPLECOUNT_1,
        .props = 0,
    };

    SDL_GPUTexture *texture = SDL_CreateGPUTexture(gpu, &texture_info);
    if (texture == nullptr)
    {
        error_code = -5;
        return nullptr;
    }

    const SDL_GPUTransferBufferCreateInfo transfer_info{
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = upload_size,
        .props = 0,
    };

    SDL_GPUTransferBuffer *transfer_buffer = SDL_CreateGPUTransferBuffer(gpu, &transfer_info);
    if (transfer_buffer == nullptr)
    {
        SDL_ReleaseGPUTexture(gpu, texture);
        error_code = -6;
        return nullptr;
    }

    void *mapped_buffer = SDL_MapGPUTransferBuffer(gpu, transfer_buffer, false);
    if (mapped_buffer == nullptr)
    {
        SDL_ReleaseGPUTransferBuffer(gpu, transfer_buffer);
        SDL_ReleaseGPUTexture(gpu, texture);
        error_code = -7;
        return nullptr;
    }

    SDL_memcpy(mapped_buffer, rgba_surface->pixels, upload_size);
    SDL_UnmapGPUTransferBuffer(gpu, transfer_buffer);

    SDL_GPUCommandBuffer *command_buffer = SDL_AcquireGPUCommandBuffer(gpu);
    if (command_buffer == nullptr)
    {
        SDL_ReleaseGPUTransferBuffer(gpu, transfer_buffer);
        SDL_ReleaseGPUTexture(gpu, texture);
        error_code = -8;
        return nullptr;
    }

    SDL_GPUCopyPass *copy_pass = SDL_BeginGPUCopyPass(command_buffer);
    if (copy_pass == nullptr)
    {
        SDL_CancelGPUCommandBuffer(command_buffer);
        SDL_ReleaseGPUTransferBuffer(gpu, transfer_buffer);
        SDL_ReleaseGPUTexture(gpu, texture);
        error_code = -9;
        return nullptr;
    }

    const SDL_GPUTextureTransferInfo upload_source{
        .transfer_buffer = transfer_buffer,
        .offset = 0,
        .pixels_per_row = pitch / bytes_per_pixel,
        .rows_per_layer = texture_height,
    };

    const SDL_GPUTextureRegion upload_destination{
        .texture = texture,
        .mip_level = 0,
        .layer = 0,
        .x = 0,
        .y = 0,
        .z = 0,
        .w = texture_width,
        .h = texture_height,
        .d = 1,
    };

    SDL_UploadToGPUTexture(copy_pass, &upload_source, &upload_destination, false);
    SDL_EndGPUCopyPass(copy_pass);

    const bool submit_ok = SDL_SubmitGPUCommandBuffer(command_buffer);
    SDL_ReleaseGPUTransferBuffer(gpu, transfer_buffer);

    if (!submit_ok)
    {
        SDL_ReleaseGPUTexture(gpu, texture);
        error_code = -10;
        return nullptr;
    }

    return texture;
}

RetroRenderer *RetroRenderer::_instance = nullptr;

int RetroRenderer::Init(int width, int height, std::string window_title)
{
    if (_instance != nullptr)
        return -1; // Already initialized

    if (SDL_Init(SDL_INIT_VIDEO) != true)
    {
        return -2; // SDL initialization failed
    }

    SDL_SetHint("SDL_RENDER_VULKAN_DEBUG", "1");

    SDL_Window *window = SDL_CreateWindow(
        window_title.c_str(),
        width,
        height,
        SDL_WINDOW_RESIZABLE);

    if (window == nullptr)
    {
        SDL_Quit();
        return -3; // Window creation failed
    }

    const SDL_GPUShaderFormat shader_formats =
        SDL_GPU_SHADERFORMAT_SPIRV;

    const int gpu_driver_count = SDL_GetNumGPUDrivers();
    SDL_Log("SDL GPU drivers available: %d", gpu_driver_count);
    for (int i = 0; i < gpu_driver_count; ++i)
    {
        SDL_Log("GPU driver [%d]: %s", i, SDL_GetGPUDriver(i));
    }

    SDL_GPUDevice *gpu = SDL_CreateGPUDevice(shader_formats, false, "vulkan");
    if (gpu == nullptr)
    {
        SDL_Log("Vulkan GPU driver unavailable (%s), falling back to default", SDL_GetError());
        gpu = SDL_CreateGPUDevice(shader_formats, false, nullptr);
    }
    if (gpu == nullptr)
    {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -4; // GPU device creation failed
    }

    if (!SDL_ClaimWindowForGPUDevice(gpu, window))
    {
        SDL_DestroyGPUDevice(gpu);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -5; // Failed to claim window for GPU
    }

    _instance = new RetroRenderer();
    _instance->window = window;
    _instance->gpu = gpu;
    SDL_GPUSwapchainComposition comp = SDL_GPU_SWAPCHAINCOMPOSITION_SDR;

    SDL_SetGPUSwapchainParameters(gpu, window, comp, SDL_GPU_PRESENTMODE_IMMEDIATE);

    // Create rendering resources
    _instance->color_format = SDL_GetGPUSwapchainTextureFormat(gpu, window);
    _instance->depth_format = ChooseDepthFormat(gpu);
    if (_instance->depth_format == SDL_GPU_TEXTUREFORMAT_INVALID)
    {
        Quit();
        return -6; // Depth format unsupported
    }

    int window_w = width;
    int window_h = height;
    SDL_GetWindowSizeInPixels(window, &window_w, &window_h);

    // Depth texture
    SDL_GPUTextureCreateInfo depth_info{
        .type = SDL_GPU_TEXTURETYPE_2D,
        .format = _instance->depth_format,
        .usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET,
        .width = static_cast<Uint32>(window_w),
        .height = static_cast<Uint32>(window_h),
        .layer_count_or_depth = 1,
        .num_levels = 1,
        .sample_count = SDL_GPU_SAMPLECOUNT_1,
        .props = 0,
    };
    _instance->depth_texture = SDL_CreateGPUTexture(gpu, &depth_info);
    if (_instance->depth_texture == nullptr)
    {
        SDL_Log("Failed to create depth texture: %s", SDL_GetError());
        Quit();
        return -7; // Depth texture creation failed
    }
    _instance->depth_width = static_cast<Uint32>(window_w);
    _instance->depth_height = static_cast<Uint32>(window_h);

    // Sampler
    SDL_GPUSamplerCreateInfo sampler_info{
        .min_filter = SDL_GPU_FILTER_LINEAR,
        .mag_filter = SDL_GPU_FILTER_LINEAR,
        .mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR,
        .address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
        .address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
        .address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
        .mip_lod_bias = 0.0f,
        .max_anisotropy = 1.0f,
        .compare_op = SDL_GPU_COMPAREOP_ALWAYS,
        .min_lod = 0.0f,
        .max_lod = 16.0f,
        .enable_anisotropy = false,
        .enable_compare = false,
        .props = 0,
    };
    _instance->texture_sampler = SDL_CreateGPUSampler(gpu, &sampler_info);
    if (_instance->texture_sampler == nullptr)
    {
        SDL_Log("Failed to create sampler: %s", SDL_GetError());
        Quit();
        return -8; // Sampler creation failed
    }

    std::string shader_dir = "build/shaders/";
    const char *base_path = SDL_GetBasePath();
    if (base_path != nullptr)
    {
        // Executable is in build/mingw/x86_64/debug, shaders are in build/shaders relative to repo root.
        shader_dir = std::string(base_path) + "..\\..\\..\\shaders\\";
        SDL_free(const_cast<char *>(base_path));
    }

    _instance->color_vert_shader = CreateShader(gpu, shader_dir + "triangle.vert.spv", SDL_GPU_SHADERSTAGE_VERTEX, 0, 1);
    _instance->color_frag_shader = CreateShader(gpu, shader_dir + "triangle.frag.spv", SDL_GPU_SHADERSTAGE_FRAGMENT, 0, 1);
    _instance->textured_vert_shader = CreateShader(gpu, shader_dir + "textured_triangle.vert.spv", SDL_GPU_SHADERSTAGE_VERTEX, 0, 1);
    _instance->textured_frag_shader = CreateShader(gpu, shader_dir + "textured_triangle.frag.spv", SDL_GPU_SHADERSTAGE_FRAGMENT, 1, 1);

    if (_instance->color_vert_shader == nullptr || _instance->color_frag_shader == nullptr ||
        _instance->textured_vert_shader == nullptr || _instance->textured_frag_shader == nullptr)
    {
        Quit();
        return -9; // Shader creation failed
    }

    if (_instance->color_vert_shader == nullptr || _instance->color_frag_shader == nullptr || _instance->textured_vert_shader == nullptr || _instance->textured_frag_shader == nullptr)
    {
        Quit();
        return -9; // Shader creation failed
    }

    SDL_GPUVertexBufferDescription vb_color{
        .slot = 0,
        .pitch = static_cast<Uint32>(sizeof(Vertex)),
        .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
        .instance_step_rate = 0,
    };

    std::vector<SDL_GPUVertexAttribute> attrs_color{
        SDL_GPUVertexAttribute{0, 0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, 0},
        SDL_GPUVertexAttribute{1, 0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, 12},
        SDL_GPUVertexAttribute{2, 0, SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4_NORM, 24},
    };

    SDL_GPUVertexBufferDescription vb_tex{
        .slot = 0,
        .pitch = static_cast<Uint32>(sizeof(TexturedVertex)),
        .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
        .instance_step_rate = 0,
    };

    std::vector<SDL_GPUVertexAttribute> attrs_tex{
        SDL_GPUVertexAttribute{0, 0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, 0},
        SDL_GPUVertexAttribute{1, 0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, 12},
        SDL_GPUVertexAttribute{2, 0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, 20},
        SDL_GPUVertexAttribute{3, 0, SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4_NORM, 32},
    };

    const bool need_depth = (_instance->depth_format != SDL_GPU_TEXTUREFORMAT_INVALID);
    _instance->color_pipeline = CreatePipeline(gpu, _instance->color_vert_shader, _instance->color_frag_shader, vb_color, attrs_color, _instance->color_format, _instance->depth_format, false, SDL_GPU_COMPAREOP_LESS, true, true, need_depth);
    _instance->color_pipeline_transparent = CreatePipeline(gpu, _instance->color_vert_shader, _instance->color_frag_shader, vb_color, attrs_color, _instance->color_format, _instance->depth_format, true, SDL_GPU_COMPAREOP_LESS, true, false, need_depth);
    _instance->textured_pipeline = CreatePipeline(gpu, _instance->textured_vert_shader, _instance->textured_frag_shader, vb_tex, attrs_tex, _instance->color_format, _instance->depth_format, false, SDL_GPU_COMPAREOP_LESS, true, true, need_depth);
    _instance->textured_pipeline_transparent = CreatePipeline(gpu, _instance->textured_vert_shader, _instance->textured_frag_shader, vb_tex, attrs_tex, _instance->color_format, _instance->depth_format, true, SDL_GPU_COMPAREOP_LESS, true, false, need_depth);
    _instance->line_pipeline = CreatePipeline(gpu, _instance->color_vert_shader, _instance->color_frag_shader, vb_color, attrs_color, _instance->color_format, _instance->depth_format, false, SDL_GPU_COMPAREOP_LESS, true, false, need_depth, SDL_GPU_PRIMITIVETYPE_LINELIST);

    if (_instance->color_pipeline == nullptr || _instance->color_pipeline_transparent == nullptr || _instance->textured_pipeline == nullptr || _instance->textured_pipeline_transparent == nullptr || _instance->line_pipeline == nullptr)
    {
        SDL_Log("Pipeline creation failed: color=%p color_transparent=%p tex=%p tex_trans=%p",
                _instance->color_pipeline,
                _instance->color_pipeline_transparent,
                _instance->textured_pipeline,
                _instance->textured_pipeline_transparent);
        Quit();
        return -10; // Pipeline creation failed
    }

    const size_t color_buffer_size = MAX_TRIANGLES_PER_BATCH * 3 * sizeof(Vertex);
    const size_t textured_buffer_size = MAX_TRIANGLES_PER_BATCH * 3 * sizeof(TexturedVertex);
    const size_t color_index_size = MAX_TRIANGLES_PER_BATCH * 3 * sizeof(uint32_t);
    const size_t textured_index_size = MAX_TRIANGLES_PER_BATCH * 3 * sizeof(uint32_t);
    if (!EnsureBufferWithUsage(gpu, _instance->color_vertex_buffer, _instance->color_vertex_buffer_size, color_buffer_size, SDL_GPU_BUFFERUSAGE_VERTEX) ||
        !EnsureBufferWithUsage(gpu, _instance->textured_vertex_buffer, _instance->textured_vertex_buffer_size, textured_buffer_size, SDL_GPU_BUFFERUSAGE_VERTEX) ||
        !EnsureBufferWithUsage(gpu, _instance->color_index_buffer, _instance->color_index_buffer_size, color_index_size, SDL_GPU_BUFFERUSAGE_INDEX) ||
        !EnsureBufferWithUsage(gpu, _instance->textured_index_buffer, _instance->textured_index_buffer_size, textured_index_size, SDL_GPU_BUFFERUSAGE_INDEX))
    {
        Quit();
        return -11; // Vertex buffer creation failed
    }

    return 0;
}

int RetroRenderer::Quit()
{
    if (_instance == nullptr)
        return -1; // Not initialized

    for (auto &entry : _instance->texture_cache)
    {
        SDL_ReleaseGPUTexture(_instance->gpu, entry.second);
    }
    _instance->texture_cache.clear();

    if (_instance->depth_texture)
        SDL_ReleaseGPUTexture(_instance->gpu, _instance->depth_texture);

    for (auto &mesh : _instance->static_meshes)
    {
        if (mesh.vertex_buffer)
            SDL_ReleaseGPUBuffer(_instance->gpu, mesh.vertex_buffer);
        if (mesh.index_buffer)
            SDL_ReleaseGPUBuffer(_instance->gpu, mesh.index_buffer);
    }
    _instance->static_meshes.clear();
    _instance->static_mesh_cmds.clear();

    if (_instance->color_vertex_buffer)
        SDL_ReleaseGPUBuffer(_instance->gpu, _instance->color_vertex_buffer);
    if (_instance->textured_vertex_buffer)
        SDL_ReleaseGPUBuffer(_instance->gpu, _instance->textured_vertex_buffer);
    if (_instance->color_index_buffer)
        SDL_ReleaseGPUBuffer(_instance->gpu, _instance->color_index_buffer);
    if (_instance->textured_index_buffer)
        SDL_ReleaseGPUBuffer(_instance->gpu, _instance->textured_index_buffer);

    if (_instance->texture_sampler)
        SDL_ReleaseGPUSampler(_instance->gpu, _instance->texture_sampler);

    if (_instance->color_pipeline)
        SDL_ReleaseGPUGraphicsPipeline(_instance->gpu, _instance->color_pipeline);
    if (_instance->color_pipeline_transparent)
        SDL_ReleaseGPUGraphicsPipeline(_instance->gpu, _instance->color_pipeline_transparent);
    if (_instance->textured_pipeline)
        SDL_ReleaseGPUGraphicsPipeline(_instance->gpu, _instance->textured_pipeline);
    if (_instance->textured_pipeline_transparent)
        SDL_ReleaseGPUGraphicsPipeline(_instance->gpu, _instance->textured_pipeline_transparent);
    if (_instance->line_pipeline)
        SDL_ReleaseGPUGraphicsPipeline(_instance->gpu, _instance->line_pipeline);

    if (_instance->color_vert_shader)
        SDL_ReleaseGPUShader(_instance->gpu, _instance->color_vert_shader);
    if (_instance->color_frag_shader)
        SDL_ReleaseGPUShader(_instance->gpu, _instance->color_frag_shader);
    if (_instance->textured_vert_shader)
        SDL_ReleaseGPUShader(_instance->gpu, _instance->textured_vert_shader);
    if (_instance->textured_frag_shader)
        SDL_ReleaseGPUShader(_instance->gpu, _instance->textured_frag_shader);

    SDL_DestroyGPUDevice(_instance->gpu);
    SDL_DestroyWindow(_instance->window);
    delete _instance;
    _instance = nullptr;

    return 0;
}

SDL_GPUDevice *RetroRenderer::GetGPUDevice()
{
    if (_instance == nullptr)
        return nullptr; // Not initialized

    return _instance->gpu;
}

SDL_Window *RetroRenderer::GetWindow()
{
    if (_instance == nullptr)
        return nullptr; // Not initialized

    return _instance->window;
}

int RetroRenderer::BeginFrame()
{
    if (_instance == nullptr)
        return -1; // Not initialized
    if (_instance->frame_active)
        return 0; // Already begun

    _instance->frame_command_buffer = SDL_AcquireGPUCommandBuffer(_instance->gpu);
    if (_instance->frame_command_buffer == nullptr)
    {
        SDL_Log("Failed to acquire command buffer: %s", SDL_GetError());
        return -2;
    }

    Uint32 swap_w = 0;
    Uint32 swap_h = 0;
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(_instance->frame_command_buffer, _instance->window, &_instance->frame_swapchain_texture, &swap_w, &swap_h) || _instance->frame_swapchain_texture == nullptr)
    {
        SDL_Log("Failed to acquire swapchain texture: %s", SDL_GetError());
        SDL_CancelGPUCommandBuffer(_instance->frame_command_buffer);
        _instance->frame_command_buffer = nullptr;
        return -3;
    }

    if (_instance->depth_texture == nullptr ||
        _instance->depth_width != swap_w ||
        _instance->depth_height != swap_h)
    {
        if (_instance->depth_texture)
            SDL_ReleaseGPUTexture(_instance->gpu, _instance->depth_texture);

        SDL_GPUTextureCreateInfo depth_info{
            .type = SDL_GPU_TEXTURETYPE_2D,
            .format = _instance->depth_format,
            .usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET,
            .width = swap_w,
            .height = swap_h,
            .layer_count_or_depth = 1,
            .num_levels = 1,
            .sample_count = SDL_GPU_SAMPLECOUNT_1,
            .props = 0,
        };
        _instance->depth_texture = SDL_CreateGPUTexture(_instance->gpu, &depth_info);
        if (_instance->depth_texture == nullptr)
        {
            SDL_Log("Failed to recreate depth texture: %s", SDL_GetError());
            SDL_CancelGPUCommandBuffer(_instance->frame_command_buffer);
            _instance->frame_command_buffer = nullptr;
            return -4;
        }
        _instance->depth_width = swap_w;
        _instance->depth_height = swap_h;
    }

    // Push global uniforms before the render pass
    MatricesUBO matrices{
        .view = _instance->view_matrix,
        .proj = _instance->projection_matrix,
        .model = _instance->model_matrix,
        .normal = glm::mat4(glm::transpose(glm::inverse(_instance->model_matrix))),
    };

    LightsUBO lights{};
    lights.ambient_light = glm::vec4(
        _instance->ambiant_light.r / 255.0f,
        _instance->ambiant_light.g / 255.0f,
        _instance->ambiant_light.b / 255.0f,
        _instance->ambiant_light.intensity);

    if (_instance->directional_light_enabled)
    {
        glm::vec3 dir = glm::normalize(glm::vec3(
            _instance->directional_light.direction_x,
            _instance->directional_light.direction_y,
            _instance->directional_light.direction_z));
        lights.directional_color_intensity = glm::vec4(
            _instance->directional_light.r / 255.0f,
            _instance->directional_light.g / 255.0f,
            _instance->directional_light.b / 255.0f,
            _instance->directional_light.intensity);
        lights.directional_direction = glm::vec4(dir, 0.0f);
    }
    lights.counts.x = _instance->directional_light_enabled ? 1 : 0;
    lights.counts.y = _instance->point_light_count;
    lights.counts.z = _instance->spot_light_count;

    for (uint8_t i = 0; i < _instance->point_light_count; ++i)
    {
        const auto &pl = _instance->point_lights[i];
        glm::vec3 pos = glm::vec3(pl.position_x, pl.position_y, pl.position_z);
        lights.point_lights[i].color_intensity = glm::vec4(
            pl.r / 255.0f,
            pl.g / 255.0f,
            pl.b / 255.0f,
            pl.intensity);
        lights.point_lights[i].position_constant = glm::vec4(pos, pl.constant);
        lights.point_lights[i].attenuation = glm::vec4(pl.linear, pl.quadratic, 0.0f, 0.0f);
    }
    for (uint8_t i = 0; i < _instance->spot_light_count; ++i)
    {
        const auto &sl = _instance->spot_lights[i];
        glm::vec3 pos = glm::vec3(sl.position_x, sl.position_y, sl.position_z);
        glm::vec3 dir = glm::normalize(glm::vec3(sl.direction_x, sl.direction_y, sl.direction_z));
        lights.spot_lights[i].color_intensity = glm::vec4(
            sl.r / 255.0f,
            sl.g / 255.0f,
            sl.b / 255.0f,
            sl.intensity);
        lights.spot_lights[i].position_constant = glm::vec4(pos, sl.constant);
        lights.spot_lights[i].direction_cutoff = glm::vec4(dir, sl.cutoff_angle);
        lights.spot_lights[i].attenuation = glm::vec4(sl.linear, sl.quadratic, 0.0f, 0.0f);
    }

    SDL_PushGPUVertexUniformData(_instance->frame_command_buffer, 0, &matrices, sizeof(MatricesUBO));
    SDL_PushGPUFragmentUniformData(_instance->frame_command_buffer, 0, &lights, sizeof(LightsUBO));

    SDL_GPUColorTargetInfo color_target{
        .texture = _instance->frame_swapchain_texture,
        .mip_level = 0,
        .layer_or_depth_plane = 0,
        .clear_color = {0.0f, 0.0f, 0.0f, 1.0f},
        .load_op = SDL_GPU_LOADOP_CLEAR,
        .store_op = SDL_GPU_STOREOP_STORE,
        .resolve_texture = nullptr,
        .resolve_mip_level = 0,
        .resolve_layer = 0,
        .cycle = false,
        .cycle_resolve_texture = false,
    };

    SDL_GPUDepthStencilTargetInfo depth_target{
        .texture = _instance->depth_texture,
        .clear_depth = 1.0f,
        .load_op = SDL_GPU_LOADOP_CLEAR,
        .store_op = SDL_GPU_STOREOP_STORE,
        .stencil_load_op = SDL_GPU_LOADOP_CLEAR,
        .stencil_store_op = SDL_GPU_STOREOP_DONT_CARE,
        .cycle = false,
        .clear_stencil = 0,
    };

    _instance->frame_render_pass = SDL_BeginGPURenderPass(_instance->frame_command_buffer, &color_target, 1, &depth_target);
    if (_instance->frame_render_pass == nullptr)
    {
        SDL_CancelGPUCommandBuffer(_instance->frame_command_buffer);
        _instance->frame_command_buffer = nullptr;
        return -5;
    }

    _instance->frame_active = true;
    return 0;
}
int RetroRenderer::LoadTexture(char *bitmap, size_t size, int w, int h, TextureID &out_id)
{
    if (_instance == nullptr)
        return -1; // Not initialized

    // R8G8B8A8 Array
    SDL_Surface *rgba_surface = SDL_CreateSurfaceFrom(
        w,
        h,
        SDL_PIXELFORMAT_RGBA8888,
        bitmap,
        // pitch
        4 * w);
    if (rgba_surface == nullptr)
    {
        return -3; // Failed to create surface
    }

    int error_code = 0;
    SDL_GPUTexture *gpu_texture = CreateTextureFromSurface(_instance->gpu, rgba_surface, error_code);
    SDL_DestroySurface(rgba_surface);
    if (gpu_texture == nullptr)
    {
        return error_code; // Texture creation failed
    }
    static TextureID next_id = 1;
    TextureID id = next_id++;
    _instance->texture_cache[id] = gpu_texture;
    out_id = id;
    return 0;
}

int RetroRenderer::UnloadTexture(TextureID texture_id)
{
    if (_instance == nullptr)
        return -1; // Not initialized

    auto it = _instance->texture_cache.find(texture_id);
    if (it == _instance->texture_cache.end())
    {
        return -2; // Texture not found
    }

    SDL_ReleaseGPUTexture(_instance->gpu, it->second);
    _instance->texture_cache.erase(it);

    return 0;
}

int RetroRenderer::SetAmbiantLight(AmbiantLightInfo light_info)
{
    if (_instance == nullptr)
        return -1; // Not initialized

    _instance->ambiant_light = light_info;

    return 0;
}

int RetroRenderer::SetDirectionalLight(DirectionalLightInfo light_info)
{
    if (_instance == nullptr)
        return -1; // Not initialized

    _instance->directional_light = light_info;
    _instance->directional_light_enabled = true;

    return 0;
}

int RetroRenderer::SetPointLight(PointLightInfo light_info, uint8_t index)
{
    if (_instance == nullptr)
        return -1; // Not initialized

    if (index >= 16)
        return -2; // Invalid index

    _instance->point_lights[index] = light_info;
    if (index + 1 > _instance->point_light_count)
    {
        _instance->point_light_count = static_cast<uint8_t>(index + 1);
    }

    return 0;
}

int RetroRenderer::SetSpotLight(SpotLightInfo light_info, uint8_t index)
{
    if (_instance == nullptr)
        return -1; // Not initialized

    if (index >= 16)
        return -2; // Invalid index

    _instance->spot_lights[index] = light_info;
    if (index + 1 > _instance->spot_light_count)
    {
        _instance->spot_light_count = static_cast<uint8_t>(index + 1);
    }

    return 0;
}

int RetroRenderer::DisableDirectionalLight()
{
    if (_instance == nullptr)
        return -1; // Not initialized

    _instance->directional_light_enabled = false;
    return 0;
}

int RetroRenderer::ClearPointLights()
{
    if (_instance == nullptr)
        return -1; // Not initialized

    _instance->point_light_count = 0;
    SDL_memset(_instance->point_lights, 0, sizeof(_instance->point_lights));
    return 0;
}

int RetroRenderer::ClearSpotLights()
{
    if (_instance == nullptr)
        return -1; // Not initialized

    _instance->spot_light_count = 0;
    SDL_memset(_instance->spot_lights, 0, sizeof(_instance->spot_lights));
    return 0;
}

int RetroRenderer::SetViewMatrix(glm::mat4 view)
{
    if (_instance == nullptr)
        return -1; // Not initialized

    _instance->view_matrix = view;
    return 0;
}

int RetroRenderer::SetProjectionMatrix(glm::mat4 projection)
{
    if (_instance == nullptr)
        return -1; // Not initialized

    _instance->projection_matrix = projection;
    return 0;
}

int RetroRenderer::SetModelMatrix(glm::mat4 model)
{
    if (_instance == nullptr)
        return -1; // Not initialized

    _instance->model_matrix = model;
    return 0;
}

int RetroRenderer::DrawTriangleArray(Triangle *triangles, size_t triangle_count, bool transparent)
{
    if (_instance == nullptr)
        return -1; // Not initialized

    if (transparent)
    {
        _instance->transparent_triangle_buffer.insert(
            _instance->transparent_triangle_buffer.end(),
            triangles,
            triangles + triangle_count);
    }
    else
    {
        _instance->triangle_buffer.insert(
            _instance->triangle_buffer.end(),
            triangles,
            triangles + triangle_count);
    }

    return 0;
}

int RetroRenderer::DrawTexturedTriangleArray(TexturedTriangle *triangles, size_t triangle_count, TextureID texture, bool transparent)
{
    if (_instance == nullptr)
        return -1; // Not initialized
    if (texture == 0)
        return -2; // Invalid texture

    SDL_GPUTexture *gpu_texture = nullptr;
    auto it = _instance->texture_cache.find(texture);
    if (it != _instance->texture_cache.end())
    {
        gpu_texture = it->second;
    }
    else
    {
        return -3; // Texture not found
    }

    if (transparent)
    {
        for (size_t i = 0; i < triangle_count; ++i)
        {
            _instance->transparent_textured_triangle_buffer.emplace_back(triangles[i], gpu_texture);
        }
    }
    else
    {
        for (size_t i = 0; i < triangle_count; ++i)
        {
            _instance->textured_triangle_buffer.emplace_back(triangles[i], gpu_texture);
        }
    }

    return 0;
}

int RetroRenderer::DrawIndexedTriangleArray(Vertex *vertices, size_t vertex_count, uint32_t *indices, size_t index_count, bool transparent)
{
    if (_instance == nullptr)
        return -1; // Not initialized

    if (vertices == nullptr || indices == nullptr || vertex_count == 0 || index_count == 0)
        return -2; // Invalid input

    const uint32_t base_vertex = static_cast<uint32_t>(_instance->indexed_color_vertices.size());
    _instance->indexed_color_vertices.insert(_instance->indexed_color_vertices.end(), vertices, vertices + vertex_count);

    const uint32_t first_index = static_cast<uint32_t>(_instance->indexed_color_indices.size());
    _instance->indexed_color_indices.reserve(_instance->indexed_color_indices.size() + index_count);
    for (size_t i = 0; i < index_count; ++i)
    {
        uint32_t idx = indices[i];
        if (idx >= vertex_count)
            return -3; // Index out of range
        _instance->indexed_color_indices.push_back(base_vertex + idx);
    }

    _instance->indexed_color_cmds.push_back(IndexedCmd{
        .transparent = transparent,
        .texture = nullptr,
        .first_index = first_index,
        .index_count = static_cast<uint32_t>(index_count),
        .has_model = false,
        .model = glm::mat4(1.0f),
    });

    return 0;
}

int RetroRenderer::DrawIndexedTexturedTriangleArray(TexturedVertex *vertices, size_t vertex_count, uint32_t *indices, size_t index_count, TextureID texture, bool transparent)
{
    if (_instance == nullptr)
        return -1; // Not initialized

    if (vertices == nullptr || indices == nullptr || vertex_count == 0 || index_count == 0 || texture == 0)
        return -2; // Invalid input

    SDL_GPUTexture *gpu_texture = nullptr;
    auto it = _instance->texture_cache.find(texture);
    if (it != _instance->texture_cache.end())
    {
        gpu_texture = it->second;
    }
    else
    {
        return -3; // Texture not found
    }

    const uint32_t base_vertex = static_cast<uint32_t>(_instance->indexed_textured_vertices.size());
    _instance->indexed_textured_vertices.insert(_instance->indexed_textured_vertices.end(), vertices, vertices + vertex_count);

    const uint32_t first_index = static_cast<uint32_t>(_instance->indexed_textured_indices.size());
    _instance->indexed_textured_indices.reserve(_instance->indexed_textured_indices.size() + index_count);
    for (size_t i = 0; i < index_count; ++i)
    {
        uint32_t idx = indices[i];
        if (idx >= vertex_count)
            return -4; // Index out of range
        _instance->indexed_textured_indices.push_back(base_vertex + idx);
    }

    _instance->indexed_textured_cmds.push_back(IndexedCmd{
        .transparent = transparent,
        .texture = gpu_texture,
        .first_index = first_index,
        .index_count = static_cast<uint32_t>(index_count),
        .has_model = false,
        .model = glm::mat4(1.0f),
    });

    return 0;
}

int RetroRenderer::DrawIndexedTriangleArrayModel(Vertex *vertices, size_t vertex_count, uint32_t *indices, size_t index_count, const glm::mat4 *model, bool transparent)
{
    if (_instance == nullptr)
        return -1; // Not initialized

    if (vertices == nullptr || indices == nullptr || vertex_count == 0 || index_count == 0 || model == nullptr)
        return -2; // Invalid input

    const uint32_t base_vertex = static_cast<uint32_t>(_instance->indexed_color_vertices.size());
    _instance->indexed_color_vertices.insert(_instance->indexed_color_vertices.end(), vertices, vertices + vertex_count);

    const uint32_t first_index = static_cast<uint32_t>(_instance->indexed_color_indices.size());
    _instance->indexed_color_indices.reserve(_instance->indexed_color_indices.size() + index_count);
    for (size_t i = 0; i < index_count; ++i)
    {
        uint32_t idx = indices[i];
        if (idx >= vertex_count)
            return -3; // Index out of range
        _instance->indexed_color_indices.push_back(base_vertex + idx);
    }

    _instance->indexed_color_cmds.push_back(IndexedCmd{
        .transparent = transparent,
        .texture = nullptr,
        .first_index = first_index,
        .index_count = static_cast<uint32_t>(index_count),
        .has_model = true,
        .model = *model,
    });

    return 0;
}

int RetroRenderer::DrawIndexedTexturedTriangleArrayModel(TexturedVertex *vertices, size_t vertex_count, uint32_t *indices, size_t index_count, TextureID texture, const glm::mat4 *model, bool transparent)
{
    if (_instance == nullptr)
        return -1; // Not initialized

    if (vertices == nullptr || indices == nullptr || vertex_count == 0 || index_count == 0 || texture == 0 || model == nullptr)
        return -2; // Invalid input

    SDL_GPUTexture *gpu_texture = nullptr;
    auto it = _instance->texture_cache.find(texture);
    if (it != _instance->texture_cache.end())
    {
        gpu_texture = it->second;
    }
    else
    {
        return -3; // Texture not found
    }

    const uint32_t base_vertex = static_cast<uint32_t>(_instance->indexed_textured_vertices.size());
    _instance->indexed_textured_vertices.insert(_instance->indexed_textured_vertices.end(), vertices, vertices + vertex_count);

    const uint32_t first_index = static_cast<uint32_t>(_instance->indexed_textured_indices.size());
    _instance->indexed_textured_indices.reserve(_instance->indexed_textured_indices.size() + index_count);
    for (size_t i = 0; i < index_count; ++i)
    {
        uint32_t idx = indices[i];
        if (idx >= vertex_count)
            return -4; // Index out of range
        _instance->indexed_textured_indices.push_back(base_vertex + idx);
    }

    _instance->indexed_textured_cmds.push_back(IndexedCmd{
        .transparent = transparent,
        .texture = gpu_texture,
        .first_index = first_index,
        .index_count = static_cast<uint32_t>(index_count),
        .has_model = true,
        .model = *model,
    });

    return 0;
}

int RetroRenderer::RegisterIndexedTriangleMesh(Vertex *vertices, size_t vertex_count, uint32_t *indices, size_t index_count, int &out_handle)
{
    if (_instance == nullptr)
        return -1;
    if (vertices == nullptr || indices == nullptr || vertex_count == 0 || index_count == 0)
        return -2;

    size_t vbytes = vertex_count * sizeof(Vertex);
    size_t ibytes = index_count * sizeof(uint32_t);

    SDL_GPUBuffer *vbo = CreateAndUploadStaticBuffer(_instance->gpu, vertices, vbytes, SDL_GPU_BUFFERUSAGE_VERTEX);
    if (vbo == nullptr)
        return -3;
    SDL_GPUBuffer *ibo = CreateAndUploadStaticBuffer(_instance->gpu, indices, ibytes, SDL_GPU_BUFFERUSAGE_INDEX);
    if (ibo == nullptr)
    {
        SDL_ReleaseGPUBuffer(_instance->gpu, vbo);
        return -4;
    }

    StaticMesh mesh;
    mesh.vertex_buffer = vbo;
    mesh.index_buffer = ibo;
    mesh.index_count = static_cast<uint32_t>(index_count);
    mesh.vertex_stride = sizeof(Vertex);
    mesh.textured = false;
    mesh.texture = nullptr;

    _instance->static_meshes.push_back(mesh);
    out_handle = static_cast<int>(_instance->static_meshes.size() - 1);
    return 0;
}

int RetroRenderer::RegisterIndexedTexturedTriangleMesh(TexturedVertex *vertices, size_t vertex_count, uint32_t *indices, size_t index_count, TextureID texture, int &out_handle)
{
    if (_instance == nullptr)
        return -1;
    if (vertices == nullptr || indices == nullptr || vertex_count == 0 || index_count == 0 || texture == 0)
        return -2;

    SDL_GPUTexture *gpu_texture = nullptr;
    auto it = _instance->texture_cache.find(texture);
    if (it != _instance->texture_cache.end())
    {
        gpu_texture = it->second;
    }
    else
    {
        return -3; // Texture not found
    }

    size_t vbytes = vertex_count * sizeof(TexturedVertex);
    size_t ibytes = index_count * sizeof(uint32_t);

    SDL_GPUBuffer *vbo = CreateAndUploadStaticBuffer(_instance->gpu, vertices, vbytes, SDL_GPU_BUFFERUSAGE_VERTEX);
    if (vbo == nullptr)
        return -4;
    SDL_GPUBuffer *ibo = CreateAndUploadStaticBuffer(_instance->gpu, indices, ibytes, SDL_GPU_BUFFERUSAGE_INDEX);
    if (ibo == nullptr)
    {
        SDL_ReleaseGPUBuffer(_instance->gpu, vbo);
        return -5;
    }

    StaticMesh mesh;
    mesh.vertex_buffer = vbo;
    mesh.index_buffer = ibo;
    mesh.index_count = static_cast<uint32_t>(index_count);
    mesh.vertex_stride = sizeof(TexturedVertex);
    mesh.textured = true;
    mesh.texture = gpu_texture;

    _instance->static_meshes.push_back(mesh);
    out_handle = static_cast<int>(_instance->static_meshes.size() - 1);
    return 0;
}

int RetroRenderer::DrawRegisteredMesh(int handle, const glm::mat4 *model, bool transparent)
{
    if (_instance == nullptr)
        return -1;
    if (model == nullptr || handle < 0 || static_cast<size_t>(handle) >= _instance->static_meshes.size())
        return -2;

    const auto &mesh = _instance->static_meshes[handle];
    if (mesh.vertex_buffer == nullptr || mesh.index_buffer == nullptr || mesh.index_count == 0)
        return -3; // already freed or invalid

    // If a frame is active and the draw is opaque, emit it immediately.
    if (_instance->frame_active && _instance->frame_render_pass != nullptr && !transparent)
    {
        SDL_GPUBufferBinding vb{mesh.vertex_buffer, 0};
        SDL_GPUBufferBinding ib{mesh.index_buffer, 0};
        SDL_BindGPUVertexBuffers(_instance->frame_render_pass, 0, &vb, 1);
        SDL_BindGPUIndexBuffer(_instance->frame_render_pass, &ib, SDL_GPU_INDEXELEMENTSIZE_32BIT);

        MatricesUBO mats{
            .view = _instance->view_matrix,
            .proj = _instance->projection_matrix,
            .model = *model,
            .normal = glm::mat4(glm::transpose(glm::inverse(*model))),
        };
        SDL_PushGPUVertexUniformData(_instance->frame_command_buffer, 0, &mats, sizeof(MatricesUBO));

        if (mesh.textured)
        {
            SDL_BindGPUGraphicsPipeline(_instance->frame_render_pass, _instance->textured_pipeline);
            SDL_GPUTextureSamplerBinding sampler_binding{
                .texture = mesh.texture,
                .sampler = _instance->texture_sampler,
            };
            SDL_BindGPUFragmentSamplers(_instance->frame_render_pass, 0, &sampler_binding, 1);
        }
        else
        {
            SDL_BindGPUGraphicsPipeline(_instance->frame_render_pass, _instance->color_pipeline);
        }

        SDL_DrawGPUIndexedPrimitives(_instance->frame_render_pass, mesh.index_count, 1, 0, 0, 0);
        return 0;
    }

    StaticMeshCmd cmd;
    cmd.handle = handle;
    cmd.transparent = transparent;
    cmd.model = *model;
    _instance->static_mesh_cmds.push_back(cmd);
    return 0;
}

int RetroRenderer::UnregisterMesh(int handle)
{
    if (_instance == nullptr)
        return -1;
    if (handle < 0 || static_cast<size_t>(handle) >= _instance->static_meshes.size())
        return -2;

    auto &mesh = _instance->static_meshes[handle];
    if (mesh.vertex_buffer)
    {
        SDL_ReleaseGPUBuffer(_instance->gpu, mesh.vertex_buffer);
        mesh.vertex_buffer = nullptr;
    }
    if (mesh.index_buffer)
    {
        SDL_ReleaseGPUBuffer(_instance->gpu, mesh.index_buffer);
        mesh.index_buffer = nullptr;
    }
    mesh.index_count = 0;
    mesh.vertex_stride = 0;
    mesh.textured = false;
    mesh.texture = nullptr;
    return 0;
}

int RetroRenderer::DrawDebugLines(const std::vector<glm::vec3> &points, const std::vector<uint32_t> &indices, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    if (_instance == nullptr)
        return -1;
    if (indices.size() % 2 != 0)
        return -2;

    size_t needed = indices.size();
    _instance->debug_line_vertices.reserve(_instance->debug_line_vertices.size() + needed);

    for (size_t i = 0; i + 1 < indices.size(); i += 2)
    {
        uint32_t i0 = indices[i];
        uint32_t i1 = indices[i + 1];
        if (i0 >= points.size() || i1 >= points.size())
            return -3;
        glm::vec3 p0 = points[i0];
        glm::vec3 p1 = points[i1];

        Vertex v0{};
        v0.x = p0.x;
        v0.y = p0.y;
        v0.z = p0.z;
        v0.nx = 0.0f;
        v0.ny = 1.0f;
        v0.nz = 0.0f;
        v0.r = r;
        v0.g = g;
        v0.b = b;
        v0.a = a;

        Vertex v1 = v0;
        v1.x = p1.x;
        v1.y = p1.y;
        v1.z = p1.z;

        _instance->debug_line_vertices.push_back(v0);
        _instance->debug_line_vertices.push_back(v1);
    }

    return 0;
}

int RetroRenderer::RenderFrame()
{
    if (_instance == nullptr)
        return -1; // Not initialized

    if (!_instance->frame_active)
    {
        int err = RetroRenderer::BeginFrame();
        if (err != 0)
            return err;
    }

    SDL_GPUCommandBuffer *command_buffer = _instance->frame_command_buffer;
    SDL_GPURenderPass *render_pass = _instance->frame_render_pass;
    if (command_buffer == nullptr || render_pass == nullptr)
        return -6;

    // Build vertex streams and draw commands
    struct DrawCmd
    {
        bool textured;
        bool transparent;
        SDL_GPUTexture *texture;
        uint32_t first_vertex;
        uint32_t vertex_count;
    };

    std::vector<Vertex> color_vertices;
    std::vector<TexturedVertex> textured_vertices;
    std::vector<DrawCmd> draw_cmds_color;
    std::vector<DrawCmd> draw_cmds_textured;

    auto append_color = [&](const std::vector<Triangle> &triangles, bool transparent)
    {
        uint32_t start_vertex = static_cast<uint32_t>(color_vertices.size());
        color_vertices.reserve(color_vertices.size() + triangles.size() * 3);
        for (const auto &t : triangles)
        {
            Vertex verts[3];
            for (int i = 0; i < 3; ++i)
            {
                verts[i].x = t.v[i].x;
                verts[i].y = t.v[i].y;
                verts[i].z = t.v[i].z;
                verts[i].nx = t.v[i].nx;
                verts[i].ny = t.v[i].ny;
                verts[i].nz = t.v[i].nz;
                verts[i].r = t.v[i].r;
                verts[i].g = t.v[i].g;
                verts[i].b = t.v[i].b;
                verts[i].a = t.v[i].a;
                color_vertices.push_back(verts[i]);
            }
        }
        uint32_t total_vertices = static_cast<uint32_t>(color_vertices.size()) - start_vertex;
        uint32_t tri_count = total_vertices / 3;
        uint32_t emitted = 0;
        while (emitted < tri_count)
        {
            uint32_t batch_tri = std::min<uint32_t>(MAX_TRIANGLES_PER_BATCH, tri_count - emitted);
            draw_cmds_color.push_back(DrawCmd{
                .textured = false,
                .transparent = transparent,
                .texture = nullptr,
                .first_vertex = start_vertex + emitted * 3,
                .vertex_count = batch_tri * 3,
            });
            emitted += batch_tri;
        }
    };

    auto append_textured_group = [&](const std::vector<std::pair<TexturedTriangle, SDL_GPUTexture *>> &tris, bool transparent)
    {
        // group by texture to reduce binds
        std::map<SDL_GPUTexture *, std::vector<const TexturedTriangle *>> grouped;
        for (const auto &entry : tris)
        {
            grouped[entry.second].push_back(&entry.first);
        }

        for (auto &group : grouped)
        {
            uint32_t start_vertex = static_cast<uint32_t>(textured_vertices.size());
            auto &vec = group.second;
            textured_vertices.reserve(textured_vertices.size() + vec.size() * 3);
            for (const TexturedTriangle *t : vec)
            {
                TexturedVertex verts[3];
                for (int i = 0; i < 3; ++i)
                {
                    verts[i].x = t->v[i].x;
                    verts[i].y = t->v[i].y;
                    verts[i].z = t->v[i].z;
                    verts[i].u = t->v[i].u;
                    verts[i].v = t->v[i].v;
                    verts[i].nx = t->v[i].nx;
                    verts[i].ny = t->v[i].ny;
                    verts[i].nz = t->v[i].nz;
                    verts[i].r = t->v[i].r;
                    verts[i].g = t->v[i].g;
                    verts[i].b = t->v[i].b;
                    verts[i].a = t->v[i].a;
                    textured_vertices.push_back(verts[i]);
                }
            }

            uint32_t total_vertices = static_cast<uint32_t>(textured_vertices.size()) - start_vertex;
            uint32_t tri_count = total_vertices / 3;
            uint32_t emitted = 0;
            while (emitted < tri_count)
            {
                uint32_t batch_tri = std::min<uint32_t>(MAX_TRIANGLES_PER_BATCH, tri_count - emitted);
                draw_cmds_textured.push_back(DrawCmd{
                    .textured = true,
                    .transparent = transparent,
                    .texture = group.first,
                    .first_vertex = start_vertex + emitted * 3,
                    .vertex_count = batch_tri * 3,
                });
                emitted += batch_tri;
            }
        }
    };

    append_color(_instance->triangle_buffer, false);
    append_color(_instance->transparent_triangle_buffer, true);
    append_textured_group(_instance->textured_triangle_buffer, false);
    append_textured_group(_instance->transparent_textured_triangle_buffer, true);

    // Append indexed data
    std::vector<uint32_t> color_indices;
    if (!_instance->indexed_color_vertices.empty())
    {
        uint32_t base = static_cast<uint32_t>(color_vertices.size());
        color_vertices.insert(color_vertices.end(), _instance->indexed_color_vertices.begin(), _instance->indexed_color_vertices.end());
        color_indices.reserve(_instance->indexed_color_indices.size());
        for (uint32_t idx : _instance->indexed_color_indices)
        {
            color_indices.push_back(base + idx);
        }
    }

    std::vector<uint32_t> textured_indices;
    if (!_instance->indexed_textured_vertices.empty())
    {
        uint32_t base = static_cast<uint32_t>(textured_vertices.size());
        textured_vertices.insert(textured_vertices.end(), _instance->indexed_textured_vertices.begin(), _instance->indexed_textured_vertices.end());
        textured_indices.reserve(_instance->indexed_textured_indices.size());
        for (uint32_t idx : _instance->indexed_textured_indices)
        {
            textured_indices.push_back(base + idx);
        }
    }

    uint32_t debug_line_start = static_cast<uint32_t>(color_vertices.size());
    if (!_instance->debug_line_vertices.empty())
    {
        color_vertices.insert(color_vertices.end(), _instance->debug_line_vertices.begin(), _instance->debug_line_vertices.end());
    }
    uint32_t debug_line_count = static_cast<uint32_t>(color_vertices.size()) - debug_line_start;

    // Upload vertex data
    if (!EnsureBufferWithUsage(_instance->gpu, _instance->color_vertex_buffer, _instance->color_vertex_buffer_size, color_vertices.size() * sizeof(Vertex), SDL_GPU_BUFFERUSAGE_VERTEX) ||
        !EnsureBufferWithUsage(_instance->gpu, _instance->textured_vertex_buffer, _instance->textured_vertex_buffer_size, textured_vertices.size() * sizeof(TexturedVertex), SDL_GPU_BUFFERUSAGE_VERTEX))
    {
        SDL_CancelGPUCommandBuffer(command_buffer);
        return -5; // Vertex buffer allocation failed
    }

    if (!color_vertices.empty() && !UploadVertexData(_instance->gpu, command_buffer, _instance->color_vertex_buffer, color_vertices.data(), color_vertices.size() * sizeof(Vertex)))
    {
        SDL_CancelGPUCommandBuffer(command_buffer);
        return -6; // Upload color vertices failed
    }

    if (!textured_vertices.empty() && !UploadVertexData(_instance->gpu, command_buffer, _instance->textured_vertex_buffer, textured_vertices.data(), textured_vertices.size() * sizeof(TexturedVertex)))
    {
        SDL_CancelGPUCommandBuffer(command_buffer);
        return -7; // Upload textured vertices failed
    }

    // Upload index data for indexed draws
    if (!color_indices.empty())
    {
        if (!EnsureBufferWithUsage(_instance->gpu, _instance->color_index_buffer, _instance->color_index_buffer_size, color_indices.size() * sizeof(uint32_t), SDL_GPU_BUFFERUSAGE_INDEX) ||
            !UploadVertexData(_instance->gpu, command_buffer, _instance->color_index_buffer, color_indices.data(), color_indices.size() * sizeof(uint32_t)))
        {
            SDL_CancelGPUCommandBuffer(command_buffer);
            return -6; // Upload color indices failed
        }
    }
    if (!textured_indices.empty())
    {
        if (!EnsureBufferWithUsage(_instance->gpu, _instance->textured_index_buffer, _instance->textured_index_buffer_size, textured_indices.size() * sizeof(uint32_t), SDL_GPU_BUFFERUSAGE_INDEX) ||
            !UploadVertexData(_instance->gpu, command_buffer, _instance->textured_index_buffer, textured_indices.data(), textured_indices.size() * sizeof(uint32_t)))
        {
            SDL_CancelGPUCommandBuffer(command_buffer);
            return -7; // Upload textured indices failed
        }
    }

    uint32_t draw_calls = 0;
    uint32_t triangles_drawn = 0;

    // Opaque colored/textured first
    if (!draw_cmds_color.empty())
    {
        SDL_GPUBufferBinding vb{_instance->color_vertex_buffer, 0};
        SDL_BindGPUVertexBuffers(render_pass, 0, &vb, 1);
        for (const auto &cmd : draw_cmds_color)
        {
            if (cmd.transparent)
                continue;
            SDL_BindGPUGraphicsPipeline(render_pass, _instance->color_pipeline);
            SDL_DrawGPUPrimitives(render_pass, cmd.vertex_count, 1, cmd.first_vertex, 0);
            draw_calls++;
            triangles_drawn += cmd.vertex_count / 3;
        }
        for (const auto &cmd : draw_cmds_color)
        {
            if (!cmd.transparent)
                continue;
            SDL_BindGPUGraphicsPipeline(render_pass, _instance->color_pipeline_transparent);
            SDL_DrawGPUPrimitives(render_pass, cmd.vertex_count, 1, cmd.first_vertex, 0);
            draw_calls++;
            triangles_drawn += cmd.vertex_count / 3;
        }
    }

    if (!_instance->indexed_color_cmds.empty() && !color_indices.empty())
    {
        SDL_GPUBufferBinding vb{_instance->color_vertex_buffer, 0};
        SDL_GPUBufferBinding ib{_instance->color_index_buffer, 0};
        SDL_BindGPUVertexBuffers(render_pass, 0, &vb, 1);
        SDL_BindGPUIndexBuffer(render_pass, &ib, SDL_GPU_INDEXELEMENTSIZE_32BIT);
        for (const auto &cmd : _instance->indexed_color_cmds)
        {
            if (cmd.transparent)
                continue;
            glm::mat4 model = cmd.has_model ? cmd.model : _instance->model_matrix;
            MatricesUBO mats{
                .view = _instance->view_matrix,
                .proj = _instance->projection_matrix,
                .model = model,
                .normal = glm::mat4(glm::transpose(glm::inverse(model))),
            };
            SDL_PushGPUVertexUniformData(command_buffer, 0, &mats, sizeof(MatricesUBO));
            SDL_BindGPUGraphicsPipeline(render_pass, _instance->color_pipeline);
            SDL_DrawGPUIndexedPrimitives(render_pass, cmd.index_count, 1, cmd.first_index, 0, 0);
            draw_calls++;
            triangles_drawn += cmd.index_count / 3;
        }
        for (const auto &cmd : _instance->indexed_color_cmds)
        {
            if (!cmd.transparent)
                continue;
            glm::mat4 model = cmd.has_model ? cmd.model : _instance->model_matrix;
            MatricesUBO mats{
                .view = _instance->view_matrix,
                .proj = _instance->projection_matrix,
                .model = model,
                .normal = glm::mat4(glm::transpose(glm::inverse(model))),
            };
            SDL_PushGPUVertexUniformData(command_buffer, 0, &mats, sizeof(MatricesUBO));
            SDL_BindGPUGraphicsPipeline(render_pass, _instance->color_pipeline_transparent);
            SDL_DrawGPUIndexedPrimitives(render_pass, cmd.index_count, 1, cmd.first_index, 0, 0);
            draw_calls++;
            triangles_drawn += cmd.index_count / 3;
        }
    }

    if (!draw_cmds_textured.empty())
    {
        SDL_GPUBufferBinding vb{_instance->textured_vertex_buffer, 0};
        SDL_BindGPUVertexBuffers(render_pass, 0, &vb, 1);
        for (const auto &cmd : draw_cmds_textured)
        {
            if (cmd.transparent)
                continue;
            SDL_BindGPUGraphicsPipeline(render_pass, _instance->textured_pipeline);
            SDL_GPUTextureSamplerBinding sampler_binding{
                .texture = cmd.texture,
                .sampler = _instance->texture_sampler,
            };
            SDL_BindGPUFragmentSamplers(render_pass, 0, &sampler_binding, 1);
            SDL_DrawGPUPrimitives(render_pass, cmd.vertex_count, 1, cmd.first_vertex, 0);
            draw_calls++;
            triangles_drawn += cmd.vertex_count / 3;
        }
        for (const auto &cmd : draw_cmds_textured)
        {
            if (!cmd.transparent)
                continue;
            SDL_BindGPUGraphicsPipeline(render_pass, _instance->textured_pipeline_transparent);
            SDL_GPUTextureSamplerBinding sampler_binding{
                .texture = cmd.texture,
                .sampler = _instance->texture_sampler,
            };
            SDL_BindGPUFragmentSamplers(render_pass, 0, &sampler_binding, 1);
            SDL_DrawGPUPrimitives(render_pass, cmd.vertex_count, 1, cmd.first_vertex, 0);
            draw_calls++;
            triangles_drawn += cmd.vertex_count / 3;
        }
    }

    if (!_instance->indexed_textured_cmds.empty() && !textured_indices.empty())
    {
        std::stable_sort(_instance->indexed_textured_cmds.begin(), _instance->indexed_textured_cmds.end(), [](const IndexedCmd &a, const IndexedCmd &b)
                         { return a.texture < b.texture; });

        SDL_GPUBufferBinding vb{_instance->textured_vertex_buffer, 0};
        SDL_GPUBufferBinding ib{_instance->textured_index_buffer, 0};
        SDL_BindGPUVertexBuffers(render_pass, 0, &vb, 1);
        SDL_BindGPUIndexBuffer(render_pass, &ib, SDL_GPU_INDEXELEMENTSIZE_32BIT);
        for (const auto &cmd : _instance->indexed_textured_cmds)
        {
            if (cmd.transparent)
                continue;
            glm::mat4 model = cmd.has_model ? cmd.model : _instance->model_matrix;
            MatricesUBO mats{
                .view = _instance->view_matrix,
                .proj = _instance->projection_matrix,
                .model = model,
                .normal = glm::mat4(glm::transpose(glm::inverse(model))),
            };
            SDL_PushGPUVertexUniformData(command_buffer, 0, &mats, sizeof(MatricesUBO));
            SDL_BindGPUGraphicsPipeline(render_pass, _instance->textured_pipeline);
            SDL_GPUTextureSamplerBinding sampler_binding{
                .texture = cmd.texture,
                .sampler = _instance->texture_sampler,
            };
            SDL_BindGPUFragmentSamplers(render_pass, 0, &sampler_binding, 1);
            SDL_DrawGPUIndexedPrimitives(render_pass, cmd.index_count, 1, cmd.first_index, 0, 0);
            draw_calls++;
            triangles_drawn += cmd.index_count / 3;
        }
        for (const auto &cmd : _instance->indexed_textured_cmds)
        {
            if (!cmd.transparent)
                continue;
            glm::mat4 model = cmd.has_model ? cmd.model : _instance->model_matrix;
            MatricesUBO mats{
                .view = _instance->view_matrix,
                .proj = _instance->projection_matrix,
                .model = model,
                .normal = glm::mat4(glm::transpose(glm::inverse(model))),
            };
            SDL_PushGPUVertexUniformData(command_buffer, 0, &mats, sizeof(MatricesUBO));
            SDL_BindGPUGraphicsPipeline(render_pass, _instance->textured_pipeline_transparent);
            SDL_GPUTextureSamplerBinding sampler_binding{
                .texture = cmd.texture,
                .sampler = _instance->texture_sampler,
            };
            SDL_BindGPUFragmentSamplers(render_pass, 0, &sampler_binding, 1);
            SDL_DrawGPUIndexedPrimitives(render_pass, cmd.index_count, 1, cmd.first_index, 0, 0);
            draw_calls++;
            triangles_drawn += cmd.index_count / 3;
        }
    }

    if (!_instance->static_mesh_cmds.empty())
    {
        std::vector<StaticMeshCmd> static_color;
        std::vector<StaticMeshCmd> static_textured;
        static_color.reserve(_instance->static_mesh_cmds.size());
        static_textured.reserve(_instance->static_mesh_cmds.size());

        for (const auto &cmd : _instance->static_mesh_cmds)
        {
            if (cmd.handle < 0 || static_cast<size_t>(cmd.handle) >= _instance->static_meshes.size())
                continue;
            const auto &mesh = _instance->static_meshes[cmd.handle];
            if (mesh.textured)
                static_textured.push_back(cmd);
            else
                static_color.push_back(cmd);
        }

        std::stable_sort(static_textured.begin(), static_textured.end(), [&](const StaticMeshCmd &a, const StaticMeshCmd &b)
                         {
            const auto &ma = _instance->static_meshes[a.handle];
            const auto &mb = _instance->static_meshes[b.handle];
            return ma.texture < mb.texture; });

        std::vector<StaticMeshCmd> static_color_opaque;
        std::vector<StaticMeshCmd> static_color_transparent;
        for (const auto &cmd : static_color)
        {
            (cmd.transparent ? static_color_transparent : static_color_opaque).push_back(cmd);
        }

        for (const auto &cmd : static_color_opaque)
        {
            const auto &mesh = _instance->static_meshes[cmd.handle];
            SDL_GPUBufferBinding vb{mesh.vertex_buffer, 0};
            SDL_GPUBufferBinding ib{mesh.index_buffer, 0};
            SDL_BindGPUVertexBuffers(render_pass, 0, &vb, 1);
            SDL_BindGPUIndexBuffer(render_pass, &ib, SDL_GPU_INDEXELEMENTSIZE_32BIT);

            MatricesUBO mats{
                .view = _instance->view_matrix,
                .proj = _instance->projection_matrix,
                .model = cmd.model,
                .normal = glm::mat4(glm::transpose(glm::inverse(cmd.model))),
            };
            SDL_PushGPUVertexUniformData(command_buffer, 0, &mats, sizeof(MatricesUBO));
            SDL_BindGPUGraphicsPipeline(render_pass, cmd.transparent ? _instance->color_pipeline_transparent : _instance->color_pipeline);
            SDL_DrawGPUIndexedPrimitives(render_pass, mesh.index_count, 1, 0, 0, 0);
            draw_calls++;
            triangles_drawn += mesh.index_count / 3;
        }

        for (const auto &cmd : static_color_transparent)
        {
            const auto &mesh = _instance->static_meshes[cmd.handle];
            SDL_GPUBufferBinding vb{mesh.vertex_buffer, 0};
            SDL_GPUBufferBinding ib{mesh.index_buffer, 0};
            SDL_BindGPUVertexBuffers(render_pass, 0, &vb, 1);
            SDL_BindGPUIndexBuffer(render_pass, &ib, SDL_GPU_INDEXELEMENTSIZE_32BIT);

            MatricesUBO mats{
                .view = _instance->view_matrix,
                .proj = _instance->projection_matrix,
                .model = cmd.model,
                .normal = glm::mat4(glm::transpose(glm::inverse(cmd.model))),
            };
            SDL_PushGPUVertexUniformData(command_buffer, 0, &mats, sizeof(MatricesUBO));
            SDL_BindGPUGraphicsPipeline(render_pass, _instance->color_pipeline_transparent);
            SDL_DrawGPUIndexedPrimitives(render_pass, mesh.index_count, 1, 0, 0, 0);
            draw_calls++;
            triangles_drawn += mesh.index_count / 3;
        }

        std::vector<StaticMeshCmd> static_textured_opaque;
        std::vector<StaticMeshCmd> static_textured_transparent;
        for (const auto &cmd : static_textured)
        {
            (cmd.transparent ? static_textured_transparent : static_textured_opaque).push_back(cmd);
        }

        for (const auto &cmd : static_textured_opaque)
        {
            const auto &mesh = _instance->static_meshes[cmd.handle];
            SDL_GPUBufferBinding vb{mesh.vertex_buffer, 0};
            SDL_GPUBufferBinding ib{mesh.index_buffer, 0};
            SDL_BindGPUVertexBuffers(render_pass, 0, &vb, 1);
            SDL_BindGPUIndexBuffer(render_pass, &ib, SDL_GPU_INDEXELEMENTSIZE_32BIT);

            MatricesUBO mats{
                .view = _instance->view_matrix,
                .proj = _instance->projection_matrix,
                .model = cmd.model,
                .normal = glm::mat4(glm::transpose(glm::inverse(cmd.model))),
            };
            SDL_PushGPUVertexUniformData(command_buffer, 0, &mats, sizeof(MatricesUBO));
            SDL_BindGPUGraphicsPipeline(render_pass, cmd.transparent ? _instance->textured_pipeline_transparent : _instance->textured_pipeline);
            SDL_GPUTextureSamplerBinding sampler_binding{
                .texture = mesh.texture,
                .sampler = _instance->texture_sampler,
            };
            SDL_BindGPUFragmentSamplers(render_pass, 0, &sampler_binding, 1);
            SDL_DrawGPUIndexedPrimitives(render_pass, mesh.index_count, 1, 0, 0, 0);
            draw_calls++;
            triangles_drawn += mesh.index_count / 3;
        }

        for (const auto &cmd : static_textured_transparent)
        {
            const auto &mesh = _instance->static_meshes[cmd.handle];
            SDL_GPUBufferBinding vb{mesh.vertex_buffer, 0};
            SDL_GPUBufferBinding ib{mesh.index_buffer, 0};
            SDL_BindGPUVertexBuffers(render_pass, 0, &vb, 1);
            SDL_BindGPUIndexBuffer(render_pass, &ib, SDL_GPU_INDEXELEMENTSIZE_32BIT);

            MatricesUBO mats{
                .view = _instance->view_matrix,
                .proj = _instance->projection_matrix,
                .model = cmd.model,
                .normal = glm::mat4(glm::transpose(glm::inverse(cmd.model))),
            };
            SDL_PushGPUVertexUniformData(command_buffer, 0, &mats, sizeof(MatricesUBO));
            SDL_BindGPUGraphicsPipeline(render_pass, _instance->textured_pipeline_transparent);
            SDL_GPUTextureSamplerBinding sampler_binding{
                .texture = mesh.texture,
                .sampler = _instance->texture_sampler,
            };
            SDL_BindGPUFragmentSamplers(render_pass, 0, &sampler_binding, 1);
            SDL_DrawGPUIndexedPrimitives(render_pass, mesh.index_count, 1, 0, 0, 0);
            draw_calls++;
            triangles_drawn += mesh.index_count / 3;
        }
    }

    if (debug_line_count > 0 && _instance->line_pipeline != nullptr)
    {
        SDL_GPUBufferBinding vb{_instance->color_vertex_buffer, 0};
        SDL_BindGPUVertexBuffers(render_pass, 0, &vb, 1);

        MatricesUBO mats{
            .view = _instance->view_matrix,
            .proj = _instance->projection_matrix,
            .model = _instance->model_matrix,
            .normal = glm::mat4(glm::transpose(glm::inverse(_instance->model_matrix))),
        };
        SDL_PushGPUVertexUniformData(command_buffer, 0, &mats, sizeof(MatricesUBO));
        SDL_BindGPUGraphicsPipeline(render_pass, _instance->line_pipeline);
        SDL_DrawGPUPrimitives(render_pass, debug_line_count, 1, debug_line_start, 0);
        draw_calls++;
    }

    SDL_EndGPURenderPass(render_pass);

    SDL_SubmitGPUCommandBuffer(command_buffer);
    _instance->frame_render_pass = nullptr;
    _instance->frame_command_buffer = nullptr;
    _instance->frame_swapchain_texture = nullptr;
    _instance->frame_active = false;

    // Clear buffers for next frame
    _instance->triangle_buffer.clear();
    _instance->transparent_triangle_buffer.clear();
    _instance->textured_triangle_buffer.clear();
    _instance->transparent_textured_triangle_buffer.clear();
    _instance->indexed_color_vertices.clear();
    _instance->indexed_color_indices.clear();
    _instance->indexed_textured_vertices.clear();
    _instance->indexed_textured_indices.clear();
    _instance->indexed_color_cmds.clear();
    _instance->indexed_textured_cmds.clear();
    _instance->static_mesh_cmds.clear();
    _instance->debug_line_vertices.clear();

    return 0;
}
