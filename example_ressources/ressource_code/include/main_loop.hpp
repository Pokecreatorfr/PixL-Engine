#pragma once
#include <asset_loader/asset_loader.hpp>
#include <retro_renderer/retro_renderer.hpp>
#include <vector>

int MainLoop_Init(bool *running);
int MainLoop_Run();
int MainLoop_Quit();

struct PBRPrimitive
{
    std::vector<TexturedVertex> vertices;
    std::vector<uint32_t> indices;
    PBRMaterial material;
    bool transparent = false;
    int mesh_handle = -1;
};

struct Mesh
{
    std::vector<PBRPrimitive> pbr_primitives;
    glm::mat4 model_matrix;

    Mesh() : model_matrix(1.0f) {}
};

class MainLoop
{
    MainLoop();
    static MainLoop *_instance;

public:
    static int Init(bool *running);
    static int Run();
    static int Quit();

private:
    bool *running;
    Mesh mesh;
    float time_elapsed = 0.0f;
    bool flashlight_enabled = false;
    static constexpr int NUM_ORBITING_LIGHTS = 4;
    glm::mat4 view_matrix;
    glm::mat4 projection_matrix;
    bool a = false;
    bool e = false;
    bool r = false;
    bool t = false;
    bool y = false;
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
    bool z = false;
    bool q = false;
    bool s = false;
    bool d = false;
    TextureID skybox_texture_id = 0;
    TextureID white_texture_id = 0;
};