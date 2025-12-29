#pragma once
#include "SDL3/SDL.h"
#include "SDL3/SDL_gpu.h"
#include <cstddef>
#include <cstdint>
#include <glm/glm.hpp>
#include <map>
#include <string>
#include <utility>
#include <vector>

struct AmbiantLightInfo
{
    uint8_t r, g, b;
    float intensity;
};

struct DirectionalLightInfo
{
    uint8_t r, g, b;
    float intensity;
    float direction_x, direction_y, direction_z;
};

struct PointLightInfo
{
    uint8_t r, g, b;
    float intensity;
    float position_x, position_y, position_z;
    float constant;
    float linear;
    float quadratic;
};

struct SpotLightInfo
{
    uint8_t r, g, b;
    float intensity;
    float position_x, position_y, position_z;
    float direction_x, direction_y, direction_z;
    float cutoff_angle;
    float constant;
    float linear;
    float quadratic;
};

struct Vertex
{
    float x, y, z;
    float nx, ny, nz;
    uint8_t r, g, b, a;
};

struct TexturedVertex
{
    float x, y, z = 0.0f;
    float u, v;
    float nx, ny, nz;
    uint8_t r, g, b, a = 255;
};

struct Triangle
{
    Vertex v[3];
};

struct TexturedTriangle
{
    TexturedVertex v[3];
};

using TextureID = uint64_t;

enum AlphaMode
{
    OPAQUE,
    MASK,
    BLEND
};

struct TextureInfo
{
    SDL_GPUTexture *tex;
    AlphaMode alphaMode;
    bool is_cubemap = false;
};

struct PBRMaterial
{
    TextureID albedo_texture = 0;
    TextureID normal_texture = 0;
    TextureID metallic_roughness_texture = 0;
    TextureID ao_texture = 0;
    TextureID emissive_texture = 0;
    float metallic_factor = 0.0f; 
    float roughness_factor = 1.0f;
    float ao_factor = 1.0f;
    float emissive_strength = 0.0f;

    inline void ApplyFallbacks()
    {
        if (metallic_roughness_texture == 0)
        {
            metallic_factor = 0.0f;
            roughness_factor = 1.0f;
        }
        if (ao_texture == 0)
        {
            ao_factor = 1.0f;
        }
        if (emissive_texture == 0)
        {
            emissive_strength = 0.0f;
        }
    }
};

class RetroRenderer
{
    RetroRenderer();
    static RetroRenderer *_instance;

public:
    static int Init(int width, int height, std::string window_title);
    static int Quit();

    
    static SDL_GPUDevice *GetGPUDevice();
    static SDL_Window *GetWindow();

    
    static int LoadTexture(char *bitmap, size_t size, int w, int h, TextureID &out_id, AlphaMode alpha_mode = OPAQUE);
    static int UnloadTexture(TextureID texture_id);
    static int LoadCubeMapTexture(char *bitmaps[6], size_t sizes[6], int w, int h, TextureID &out_id);
    static int UnloadCubeMapTexture(TextureID texture_id);
    static int LoadHDRTexture(const float *rgba, int w, int h, TextureID &out_id);

    
    static int LoadTextureFromFile(const char *filename, TextureID &out_id);
    static int LoadCubeMapTextureFromFiles(const char *filenames[6], TextureID &out_id);
    static int LoadHDRTextureFromFile(const char *filename, TextureID &out_id);

    
    static int DrawSkybox(TextureID cubemap_texture);
    
    static int DrawSkySphere(TextureID texture_2d);
    
    static int SetAmbiantLight(AmbiantLightInfo light_info);
    static int SetDirectionalLight(DirectionalLightInfo light_info);
    static int SetPointLight(PointLightInfo light_info, uint8_t index = 0);
    static int SetSpotLight(SpotLightInfo light_info, uint8_t index = 0);
    static int DisableDirectionalLight();
    static int ClearPointLights();
    static int ClearSpotLights();
    
    static int BeginFrame();
    
    static int SetViewMatrix(glm::mat4 view);
    static int SetProjectionMatrix(glm::mat4 projection);
    static int SetModelMatrix(glm::mat4 model);
    
    static int DrawTriangleArray(Triangle *triangles, size_t triangle_count, bool transparent = false);
    static int DrawTexturedTriangleArray(TexturedTriangle *triangles, size_t triangle_count, TextureID texture, bool transparent = false);
    static int DrawIndexedTriangleArray(Vertex *vertices, size_t vertex_count, uint32_t *indices, size_t index_count, bool transparent = false);
    static int DrawIndexedTexturedTriangleArray(TexturedVertex *vertices, size_t vertex_count, uint32_t *indices, size_t index_count, TextureID texture, bool transparent = false);
    static int DrawIndexedTriangleArrayModel(Vertex *vertices, size_t vertex_count, uint32_t *indices, size_t index_count, const glm::mat4 *model, bool transparent = false);
    static int DrawIndexedTexturedTriangleArrayModel(TexturedVertex *vertices, size_t vertex_count, uint32_t *indices, size_t index_count, TextureID texture, const glm::mat4 *model, bool transparent = false);
    static int DrawIndexedTexturedTriangleArrayPBR(TexturedVertex *vertices, size_t vertex_count, uint32_t *indices, size_t index_count, const glm::mat4 *model, const PBRMaterial &material, bool transparent = false);
    struct Advanced
    {
        int DrawPBRTriangleArray(const std::vector<Triangle> &triangles, PBRMaterial material, bool transparent = false);
        int DrawPBRTexturedTriangleArray(const std::vector<TexturedTriangle> &triangles, PBRMaterial material, bool transparent = false);
    };

    
    static int RegisterIndexedTriangleMesh(Vertex *vertices, size_t vertex_count, uint32_t *indices, size_t index_count, int &out_handle);
    static int RegisterIndexedTexturedTriangleMesh(TexturedVertex *vertices, size_t vertex_count, uint32_t *indices, size_t index_count, TextureID texture, int &out_handle);
    static int RegisterIndexedTexturedTriangleMeshPBR(TexturedVertex *vertices, size_t vertex_count, uint32_t *indices, size_t index_count, const PBRMaterial &material, int &out_handle);
    static int UnregisterMesh(int handle);
    static int UnregisterPBRMesh(int handle);
    static int DrawRegisteredMesh(int handle, const glm::mat4 *model, bool transparent = false);
    static int DrawRegisteredMeshPBR(int handle, const glm::mat4 *model, bool transparent = false);
    static int DrawDebugLines(const std::vector<glm::vec3> &points, const std::vector<uint32_t> &indices, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    static int RenderFrame();

    
    enum class DebugViewMode
    {
        Final = 0,       
        GBufferPosition, 
        GBufferNormal,   
        GBufferAlbedo,   
        GBufferMaterial, 
        GBufferEmissive, 
        Depth,           
        COUNT
    };

    static void Debug_SetViewMode(DebugViewMode mode);
    static DebugViewMode Debug_GetViewMode();
    static const char *Debug_GetViewModeName(DebugViewMode mode);
    static void Debug_SetDepthRange(float near_plane, float far_plane);

    static void SetShadowsEnabled(bool enabled);
    static bool GetShadowsEnabled();

private:
    SDL_GPUDevice *gpu;
    SDL_Window *window;
    SDL_GPUTexture *screen_target;
    SDL_GPUTexture *depth_texture;
    SDL_GPUTextureFormat depth_format;
    SDL_GPUTextureFormat color_format;
    Uint32 depth_width;
    Uint32 depth_height;

    
    SDL_GPUTexture *gbuffer_position = nullptr; 
    SDL_GPUTexture *gbuffer_normal = nullptr;   
    SDL_GPUTexture *gbuffer_albedo = nullptr;   
    SDL_GPUTexture *gbuffer_material = nullptr; 
    SDL_GPUTexture *gbuffer_emissive = nullptr; 
    Uint32 gbuffer_width = 0;
    Uint32 gbuffer_height = 0;
    SDL_GPUCommandBuffer *frame_command_buffer = nullptr;
    SDL_GPURenderPass *frame_render_pass = nullptr;
    SDL_GPUTexture *frame_swapchain_texture = nullptr;
    bool frame_active = false;

    SDL_GPUShader *color_vert_shader;
    SDL_GPUShader *color_frag_shader;
    SDL_GPUShader *textured_vert_shader;
    SDL_GPUShader *textured_frag_shader;
    SDL_GPUShader *textured_frag_shader_mask;
    SDL_GPUShader *pbr_vert_shader;
    SDL_GPUShader *pbr_frag_shader;
    SDL_GPUShader *pbr_frag_shader_mask;
    SDL_GPUShader *skybox_vert_shader;
    SDL_GPUShader *skybox_frag_shader;
    SDL_GPUShader *skysphere_vert_shader;
    SDL_GPUShader *skysphere_frag_shader;

    
    SDL_GPUShader *gbuffer_vert_shader = nullptr;
    SDL_GPUShader *gbuffer_frag_shader = nullptr;
    SDL_GPUShader *gbuffer_frag_shader_mask = nullptr;
    SDL_GPUShader *deferred_vert_shader = nullptr;
    SDL_GPUShader *deferred_frag_shader = nullptr;

    SDL_GPUGraphicsPipeline *color_pipeline;
    SDL_GPUGraphicsPipeline *color_pipeline_transparent;
    SDL_GPUGraphicsPipeline *textured_pipeline;
    SDL_GPUGraphicsPipeline *textured_pipeline_transparent;
    SDL_GPUGraphicsPipeline *textured_pipeline_mask;
    SDL_GPUGraphicsPipeline *line_pipeline;
    SDL_GPUGraphicsPipeline *pbr_pipeline;
    SDL_GPUGraphicsPipeline *pbr_pipeline_transparent;
    SDL_GPUGraphicsPipeline *pbr_pipeline_mask;
    SDL_GPUGraphicsPipeline *skybox_pipeline;
    SDL_GPUGraphicsPipeline *skysphere_pipeline;

    
    SDL_GPUGraphicsPipeline *gbuffer_pipeline = nullptr;           
    SDL_GPUGraphicsPipeline *gbuffer_pipeline_mask = nullptr;      
    SDL_GPUGraphicsPipeline *deferred_lighting_pipeline = nullptr; 

    
    SDL_GPUShader *debug_view_frag_shader = nullptr;
    SDL_GPUGraphicsPipeline *debug_view_pipeline = nullptr;
    DebugViewMode debug_view_mode = DebugViewMode::Final;
    float debug_depth_near = 1.0f;
    float debug_depth_far = 5000.0f;

    
    SDL_GPUTexture *shadow_map_dir = nullptr;  
    SDL_GPUTexture *shadow_map_spot = nullptr; 
    SDL_GPUShader *shadow_vert_shader = nullptr;
    SDL_GPUShader *shadow_frag_shader = nullptr;
    SDL_GPUGraphicsPipeline *shadow_pipeline = nullptr;
    SDL_GPUSampler *shadow_sampler = nullptr;
    static const Uint32 SHADOW_MAP_SIZE = 8192;
    bool shadows_enabled = true;

    SDL_GPUSampler *texture_sampler;

    SDL_GPUBuffer *color_vertex_buffer;
    size_t color_vertex_buffer_size;

    SDL_GPUBuffer *textured_vertex_buffer;
    size_t textured_vertex_buffer_size;
    SDL_GPUBuffer *color_index_buffer;
    size_t color_index_buffer_size;
    SDL_GPUBuffer *textured_index_buffer;
    size_t textured_index_buffer_size;
    SDL_GPUBuffer *skysphere_vertex_buffer;
    size_t skysphere_vertex_buffer_size;
    SDL_GPUBuffer *skysphere_index_buffer;
    size_t skysphere_index_buffer_size;
    uint32_t skysphere_index_count = 0;
    SDL_GPUBuffer *skybox_vertex_buffer;
    size_t skybox_vertex_buffer_size;
    TextureInfo fallback_white_texture{nullptr, OPAQUE};
    TextureInfo fallback_black_texture{nullptr, OPAQUE};
    TextureInfo fallback_mr_texture{nullptr, OPAQUE};
    TextureInfo fallback_normal_texture{nullptr, OPAQUE};

    
    AmbiantLightInfo ambiant_light;
    DirectionalLightInfo directional_light;
    bool directional_light_enabled;
    uint8_t point_light_count;
    uint8_t spot_light_count;
    PointLightInfo point_lights[16];
    SpotLightInfo spot_lights[16];
    glm::mat4 view_matrix;
    glm::mat4 projection_matrix;
    glm::mat4 model_matrix;

    
    std::vector<Triangle> triangle_buffer;
    std::vector<std::pair<TexturedTriangle, TextureInfo>> textured_triangle_buffer;

    std::vector<Triangle> transparent_triangle_buffer;
    std::vector<std::pair<TexturedTriangle, TextureInfo>> transparent_textured_triangle_buffer;

    
    std::vector<Vertex> indexed_color_vertices;
    std::vector<uint32_t> indexed_color_indices;
    std::vector<TexturedVertex> indexed_textured_vertices;
    std::vector<uint32_t> indexed_textured_indices;
    std::vector<Vertex> debug_line_vertices;

    struct IndexedCmd
    {
        bool transparent;
        TextureInfo texture{nullptr, OPAQUE};
        uint32_t first_index;
        uint32_t index_count;
        bool has_model;
        glm::mat4 model;
    };
    std::vector<IndexedCmd> indexed_color_cmds;
    std::vector<IndexedCmd> indexed_textured_cmds;
    struct PBRIndexedCmd
    {
        bool transparent;
        uint32_t first_index;
        uint32_t index_count;
        bool has_model;
        glm::mat4 model;
        TextureInfo albedo{nullptr, OPAQUE};
        TextureInfo normal{nullptr, OPAQUE};
        TextureInfo metallic_roughness{nullptr, OPAQUE};
        TextureInfo ao{nullptr, OPAQUE};
        TextureInfo emissive{nullptr, OPAQUE};
        glm::vec4 factors; 
        glm::ivec4 flags;  
        int has_normal_map = 0;
    };
    std::vector<PBRIndexedCmd> pbr_cmds;

    
    std::map<TextureID, TextureInfo> texture_cache;

    struct StaticMesh
    {
        SDL_GPUBuffer *vertex_buffer = nullptr;
        SDL_GPUBuffer *index_buffer = nullptr;
        uint32_t index_count = 0;
        uint32_t vertex_stride = 0;
        bool textured = false;
        TextureInfo texture{nullptr, OPAQUE};
    };

    struct StaticMeshCmd
    {
        int handle = -1;
        bool transparent = false;
        glm::mat4 model{1.0f};
    };

    std::vector<StaticMesh> static_meshes;
    std::vector<StaticMeshCmd> static_mesh_cmds;

    
    struct StaticPBRMesh
    {
        SDL_GPUBuffer *vertex_buffer = nullptr;
        SDL_GPUBuffer *index_buffer = nullptr;
        uint32_t index_count = 0;
        TextureInfo albedo{nullptr, OPAQUE};
        TextureInfo normal{nullptr, OPAQUE};
        TextureInfo metallic_roughness{nullptr, OPAQUE};
        TextureInfo ao{nullptr, OPAQUE};
        TextureInfo emissive{nullptr, OPAQUE};
        glm::vec4 factors{0.0f, 1.0f, 1.0f, 0.0f}; 
        glm::ivec4 flags{0, 0, 0, 0};              
        int has_normal_map = 0;
    };

    struct StaticPBRMeshCmd
    {
        int handle = -1;
        bool transparent = false;
        glm::mat4 model{1.0f};
    };

    std::vector<StaticPBRMesh> static_pbr_meshes;
    std::vector<StaticPBRMeshCmd> static_pbr_mesh_cmds;

    struct SkyboxCmd
    {
        TextureInfo cubemap_texture{nullptr, OPAQUE};
    };
    SkyboxCmd skybox_cmd;

    struct SkySphereCmd
    {
        TextureInfo texture{nullptr, OPAQUE};
    };
    SkySphereCmd skysphere_cmd;
};
