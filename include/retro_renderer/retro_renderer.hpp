#pragma once
#include "SDL3/SDL.h"
#include "SDL3/SDL_gpu.h"
#include <cstddef>
#include <cstdint>
#include <glm/glm.hpp>
#include <map>
#include <string>
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

class RetroRenderer
{
    RetroRenderer();
    static RetroRenderer *_instance;

public:
    static int Init(int width, int height, std::string window_title);
    static int Quit();

    // SDL
    static SDL_GPUDevice *GetGPUDevice();
    static SDL_Window *GetWindow();

    // Texture Methods
    static int LoadTexture(char *bitmap, size_t size, int w, int h, TextureID &out_id);
    static int UnloadTexture(TextureID texture_id);

    // Light Methods
    static int SetAmbiantLight(AmbiantLightInfo light_info);
    static int SetDirectionalLight(DirectionalLightInfo light_info);
    static int SetPointLight(PointLightInfo light_info, uint8_t index = 0);
    static int SetSpotLight(SpotLightInfo light_info, uint8_t index = 0);
    static int DisableDirectionalLight();
    static int ClearPointLights();
    static int ClearSpotLights();
    // Frame lifecycle
    static int BeginFrame();
    // Matrix Methods
    static int SetViewMatrix(glm::mat4 view);
    static int SetProjectionMatrix(glm::mat4 projection);
    static int SetModelMatrix(glm::mat4 model);
    // Drawing Methods
    static int DrawTriangleArray(Triangle *triangles, size_t triangle_count, bool transparent = false);
    static int DrawTexturedTriangleArray(TexturedTriangle *triangles, size_t triangle_count, TextureID texture, bool transparent = false);
    static int DrawIndexedTriangleArray(Vertex *vertices, size_t vertex_count, uint32_t *indices, size_t index_count, bool transparent = false);
    static int DrawIndexedTexturedTriangleArray(TexturedVertex *vertices, size_t vertex_count, uint32_t *indices, size_t index_count, TextureID texture, bool transparent = false);
    static int DrawIndexedTriangleArrayModel(Vertex *vertices, size_t vertex_count, uint32_t *indices, size_t index_count, const glm::mat4 *model, bool transparent = false);
    static int DrawIndexedTexturedTriangleArrayModel(TexturedVertex *vertices, size_t vertex_count, uint32_t *indices, size_t index_count, TextureID texture, const glm::mat4 *model, bool transparent = false);
    // GPU-resident meshes
    static int RegisterIndexedTriangleMesh(Vertex *vertices, size_t vertex_count, uint32_t *indices, size_t index_count, int &out_handle);
    static int RegisterIndexedTexturedTriangleMesh(TexturedVertex *vertices, size_t vertex_count, uint32_t *indices, size_t index_count, TextureID texture, int &out_handle);
    static int UnregisterMesh(int handle);
    static int DrawRegisteredMesh(int handle, const glm::mat4 *model, bool transparent = false);
    static int DrawDebugLines(const std::vector<glm::vec3> &points, const std::vector<uint32_t> &indices, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    static int RenderFrame();

private:
    SDL_GPUDevice *gpu;
    SDL_Window *window;
    SDL_GPUTexture *screen_target;
    SDL_GPUTexture *depth_texture;
    SDL_GPUTextureFormat depth_format;
    SDL_GPUTextureFormat color_format;
    Uint32 depth_width;
    Uint32 depth_height;
    SDL_GPUCommandBuffer *frame_command_buffer = nullptr;
    SDL_GPURenderPass *frame_render_pass = nullptr;
    SDL_GPUTexture *frame_swapchain_texture = nullptr;
    bool frame_active = false;

    SDL_GPUShader *color_vert_shader;
    SDL_GPUShader *color_frag_shader;
    SDL_GPUShader *textured_vert_shader;
    SDL_GPUShader *textured_frag_shader;

    SDL_GPUGraphicsPipeline *color_pipeline;
    SDL_GPUGraphicsPipeline *color_pipeline_transparent;
    SDL_GPUGraphicsPipeline *textured_pipeline;
    SDL_GPUGraphicsPipeline *textured_pipeline_transparent;
    SDL_GPUGraphicsPipeline *line_pipeline;

    SDL_GPUSampler *texture_sampler;

    SDL_GPUBuffer *color_vertex_buffer;
    size_t color_vertex_buffer_size;

    SDL_GPUBuffer *textured_vertex_buffer;
    size_t textured_vertex_buffer_size;
    SDL_GPUBuffer *color_index_buffer;
    size_t color_index_buffer_size;
    SDL_GPUBuffer *textured_index_buffer;
    size_t textured_index_buffer_size;

    // Renderer storages

    // Lights
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

    // Triangles
    std::vector<Triangle> triangle_buffer;
    std::vector<std::pair<TexturedTriangle, SDL_GPUTexture *>> textured_triangle_buffer;

    std::vector<Triangle> transparent_triangle_buffer;
    std::vector<std::pair<TexturedTriangle, SDL_GPUTexture *>> transparent_textured_triangle_buffer;

    // Indexed data
    std::vector<Vertex> indexed_color_vertices;
    std::vector<uint32_t> indexed_color_indices;
    std::vector<TexturedVertex> indexed_textured_vertices;
    std::vector<uint32_t> indexed_textured_indices;
    std::vector<Vertex> debug_line_vertices;

    struct IndexedCmd
    {
        bool transparent;
        SDL_GPUTexture *texture;
        uint32_t first_index;
        uint32_t index_count;
        bool has_model;
        glm::mat4 model;
    };
    std::vector<IndexedCmd> indexed_color_cmds;
    std::vector<IndexedCmd> indexed_textured_cmds;

    // Texture
    std::map<TextureID, SDL_GPUTexture *> texture_cache;

    struct StaticMesh
    {
        SDL_GPUBuffer *vertex_buffer = nullptr;
        SDL_GPUBuffer *index_buffer = nullptr;
        uint32_t index_count = 0;
        uint32_t vertex_stride = 0;
        bool textured = false;
        SDL_GPUTexture *texture = nullptr;
    };

    struct StaticMeshCmd
    {
        int handle = -1;
        bool transparent = false;
        glm::mat4 model{1.0f};
    };

    std::vector<StaticMesh> static_meshes;
    std::vector<StaticMeshCmd> static_mesh_cmds;
};
