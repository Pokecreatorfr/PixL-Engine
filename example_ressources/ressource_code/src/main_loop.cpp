#include <SDL3/SDL.h>
#include <cmath>
#include <debug_menu.hpp>
#include <game/camera.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <input_handler/input_handler.hpp>
#include <iostream>
#include <limits>
#include <main_loop.hpp>
#include <retro_renderer/retro_renderer.hpp>
#include <stb_image.h>
#include <vector>

int MainLoop_Init(bool *running)
{
    return MainLoop::Init(running);
}

int MainLoop_Run()
{
    return MainLoop::Run();
}

int MainLoop_Quit()
{
    return MainLoop::Quit();
}

MainLoop::MainLoop()
    : running(nullptr) {}

MainLoop *MainLoop::_instance = nullptr;
int MainLoop::Init(bool *running)
{
    if (_instance != nullptr)
        return -1;
    _instance = new MainLoop();
    _instance->running = running;

    if (Camera::Init() != 0)
    {
        std::cerr << "Failed to initialize Camera" << std::endl;
        delete _instance;
        _instance = nullptr;
        return -2;
    }

    Camera::SetPosition(glm::vec3(0.0f, 50.0f, 400.0f));
    Camera::SetYawPitchDeg(180.0f, -7.0f);
    Camera::SetAspect(800.0f / 600.0f);
    Camera::SetFovDeg(45.0f);
    Camera::SetNearFar(1.0f, 5000.0f);

    _instance->mesh.model_matrix = glm::mat4(1.0f);

    std::vector<LoadedPrimitive> loaded_primitives;
    int load_err = loadGltfFromFile("test.glb", loaded_primitives);
    if (load_err == 0 && !loaded_primitives.empty())
    {
        std::cout << "Loaded " << loaded_primitives.size() << " primitives from model" << std::endl;

        
        glm::vec3 global_min(std::numeric_limits<float>::max());
        glm::vec3 global_max(std::numeric_limits<float>::lowest());
        for (const auto &lp : loaded_primitives)
        {
            global_min = glm::min(global_min, lp.aabb_min);
            global_max = glm::max(global_max, lp.aabb_max);
        }

        glm::vec3 size = global_max - global_min;
        glm::vec3 center = (global_min + global_max) * 0.5f;
        float max_extent = glm::max(size.x, glm::max(size.y, size.z));

        const float target_size = 200.0f;
        float scale_factor = (max_extent > 0.0001f) ? (target_size / max_extent) : 1.0f;

        _instance->mesh.model_matrix = glm::mat4(1.0f);
        _instance->mesh.model_matrix = glm::scale(_instance->mesh.model_matrix, glm::vec3(scale_factor));
        _instance->mesh.model_matrix = glm::translate(_instance->mesh.model_matrix, -center);

        std::cout << "Model AABB: [" << global_min.x << "," << global_min.y << "," << global_min.z << "] - ["
                  << global_max.x << "," << global_max.y << "," << global_max.z << "]" << std::endl;
        std::cout << "Scale factor: " << scale_factor << ", Center: [" << center.x << "," << center.y << "," << center.z << "]" << std::endl;

        auto loadTexture = [](const std::vector<uint8_t> &pixels, int w, int h, AlphaMode alpha) -> TextureID
        {
            if (pixels.empty())
                return 0;
            TextureID id = 0;
            int err = RetroRenderer::LoadTexture(
                const_cast<char *>(reinterpret_cast<const char *>(pixels.data())),
                pixels.size(), w, h, id, alpha);
            return (err == 0) ? id : 0;
        };

        for (size_t i = 0; i < loaded_primitives.size(); ++i)
        {
            const auto &lp = loaded_primitives[i];

            AlphaMode alpha_mode = OPAQUE;
            switch (lp.alpha_mode)
            {
            case MaterialAlphaMode::OPAQUE:
                alpha_mode = OPAQUE;
                break;
            case MaterialAlphaMode::MASK:
                alpha_mode = MASK;
                break;
            case MaterialAlphaMode::BLEND:
                alpha_mode = BLEND;
                break;
            }

            PBRPrimitive prim;
            prim.vertices = lp.vertices;
            prim.indices = lp.indices;
            prim.transparent = (lp.alpha_mode == MaterialAlphaMode::BLEND);

            prim.material.albedo_texture = loadTexture(lp.texture_pixels, lp.tex_w, lp.tex_h, alpha_mode);
            prim.material.normal_texture = loadTexture(lp.normal_pixels, lp.normal_w, lp.normal_h, OPAQUE);
            prim.material.metallic_roughness_texture = loadTexture(lp.metallic_roughness_pixels, lp.mr_w, lp.mr_h, OPAQUE);
            prim.material.ao_texture = loadTexture(lp.occlusion_pixels, lp.ao_w, lp.ao_h, OPAQUE);
            prim.material.emissive_texture = loadTexture(lp.emissive_pixels, lp.emissive_w, lp.emissive_h, OPAQUE);

            prim.material.metallic_factor = lp.metallic_factor;
            prim.material.roughness_factor = lp.roughness_factor;
            float emissive_strength = glm::length(lp.emissive_factor);
            prim.material.emissive_strength = emissive_strength;
            prim.material.ApplyFallbacks();

            
            int handle = -1;
            int reg_err = RetroRenderer::RegisterIndexedTexturedTriangleMeshPBR(
                prim.vertices.data(),
                prim.vertices.size(),
                prim.indices.data(),
                prim.indices.size(),
                prim.material,
                handle);
            if (reg_err == 0)
            {
                prim.mesh_handle = handle;
                std::cout << "Primitive " << i << " registered as GPU mesh handle=" << handle << std::endl;
            }
            else
            {
                std::cerr << "Failed to register primitive " << i << " as GPU mesh, error=" << reg_err << std::endl;
            }

            _instance->mesh.pbr_primitives.push_back(std::move(prim));
            std::cout << "Primitive " << i << " loaded (albedo=" << prim.material.albedo_texture
                      << ", normal=" << prim.material.normal_texture
                      << ", mr=" << prim.material.metallic_roughness_texture << ")" << std::endl;
        }

        std::cout << "Total PBR primitives: " << _instance->mesh.pbr_primitives.size() << std::endl;
    }
    else
    {
        std::cerr << "Failed to load mesh from test.glb, error: " << load_err << std::endl;
    }

    RetroRenderer::SetDirectionalLight(DirectionalLightInfo{
        .r = 255, .g = 255, .b = 255, .intensity = 1.0f, .direction_x = -0.5f, .direction_y = -1.0f, .direction_z = -0.5f});

    InputHandler::Bind_KeyCode(SDLK_A, &_instance->a);
    InputHandler::Bind_KeyCode(SDLK_Z, &_instance->z);
    InputHandler::Bind_KeyCode(SDLK_E, &_instance->e);
    InputHandler::Bind_KeyCode(SDLK_R, &_instance->r);
    InputHandler::Bind_KeyCode(SDLK_T, &_instance->t);
    InputHandler::Bind_KeyCode(SDLK_Y, &_instance->y);
    InputHandler::Bind_KeyCode(SDLK_UP, &_instance->up);
    InputHandler::Bind_KeyCode(SDLK_DOWN, &_instance->down);
    InputHandler::Bind_KeyCode(SDLK_LEFT, &_instance->left);
    InputHandler::Bind_KeyCode(SDLK_RIGHT, &_instance->right);
    InputHandler::Bind_KeyCode(SDLK_Q, &_instance->q);
    InputHandler::Bind_KeyCode(SDLK_S, &_instance->s);
    InputHandler::Bind_KeyCode(SDLK_D, &_instance->d);
    InputHandler::Bind_UpdateScreenRatio();

    const char *sky_path = "sky.hdr";
    int err = RetroRenderer::LoadHDRTextureFromFile(sky_path, _instance->skybox_texture_id);
    if (err == 0)
    {
        std::cout << "Skysphere texture loaded successfully with ID: " << _instance->skybox_texture_id << std::endl;
    }
    else
    {
        std::cout << "Failed to load skysphere texture from " << sky_path << ", error " << err << std::endl;
    }

    
    DebugMenu::Init();

    
    uint8_t white_pixels[4] = {255, 255, 255, 255};
    int err2 = RetroRenderer::LoadTexture(reinterpret_cast<char *>(white_pixels), 4, 1, 1, _instance->white_texture_id, OPAQUE);
    if (err2 != 0)
    {
        std::cout << "Failed to create white texture for billboards" << std::endl;
    }

    return 0;
}

int MainLoop::Run()
{
    if (_instance == nullptr)
        return -1;

    if (_instance->a)
        std::cout << "A key is pressed" << std::endl;
    if (_instance->z)
        std::cout << "Z key is pressed" << std::endl;
    if (_instance->e)
        std::cout << "E key is pressed" << std::endl;
    if (_instance->r)
        std::cout << "R key is pressed" << std::endl;
    if (_instance->t)
        std::cout << "T key is pressed" << std::endl;
    if (_instance->y)
        _instance->flashlight_enabled = !_instance->flashlight_enabled;

    const float rotSpeedDeg = 1.0f;

    if (_instance->left)
        Camera::AddYawPitchDeg(-rotSpeedDeg, 0.0f);
    if (_instance->right)
        Camera::AddYawPitchDeg(rotSpeedDeg, 0.0f);
    if (_instance->up)
        Camera::AddYawPitchDeg(0.0f, rotSpeedDeg);
    if (_instance->down)
        Camera::AddYawPitchDeg(0.0f, -rotSpeedDeg);

    Camera::Update();

    const float moveSpeed = 10.0f;
    float moveForward = 0.0f;
    float moveRight = 0.0f;

    if (_instance->z)
        moveForward += moveSpeed;
    if (_instance->s)
        moveForward -= moveSpeed;
    if (_instance->q)
        moveRight -= moveSpeed;
    if (_instance->d)
        moveRight += moveSpeed;

    Camera::Move(moveForward, moveRight, 0.0f);

    _instance->time_elapsed += 0.016f;

    
    if (_instance->flashlight_enabled)
    {
        
        SpotLightInfo sl;
        sl.r = 255;
        sl.g = 255;
        sl.b = 200;
        sl.intensity = 1.0f;
        glm::vec3 cam_pos;
        Camera::GetPosition(cam_pos);
        sl.position_x = cam_pos.x;
        sl.position_y = cam_pos.y;
        sl.position_z = cam_pos.z;
        glm::vec3 cam_forward;
        Camera::Forward(cam_forward);
        sl.direction_x = cam_forward.x;
        sl.direction_y = cam_forward.y;
        sl.direction_z = cam_forward.z;
        sl.cutoff_angle = 20.0f; 
        sl.constant = 1.0f;
        sl.linear = 0.09f;
        sl.quadratic = 0.032f;
        RetroRenderer::SetSpotLight(sl, 0);
    }

    
    const float orbit_radius = 300.0f;
    const float orbit_speed = 0.5f;
    const float orbit_height = 50.0f;

    struct LightColor
    {
        uint8_t r, g, b;
    };
    LightColor colors[4] = {
        {255, 100, 100}, 
        {100, 255, 100}, 
        {100, 100, 255}, 
        {255, 255, 100}  
    };

    glm::vec3 light_positions[NUM_ORBITING_LIGHTS];

    for (int i = 0; i < NUM_ORBITING_LIGHTS; ++i)
    {
        float angle = _instance->time_elapsed * orbit_speed + (i * 2.0f * 3.14159f / NUM_ORBITING_LIGHTS);
        PointLightInfo pl;
        pl.r = colors[i].r;
        pl.g = colors[i].g;
        pl.b = colors[i].b;
        pl.intensity = 2.0f;
        pl.position_x = orbit_radius * std::cos(angle);
        pl.position_y = orbit_height + 30.0f * std::sin(angle * 2.0f);
        pl.position_z = orbit_radius * std::sin(angle);
        pl.constant = 1.0f;
        pl.linear = 0.07f;
        pl.quadratic = 0.0002f;
        RetroRenderer::SetPointLight(pl, i);
        light_positions[i] = glm::vec3(pl.position_x, pl.position_y, pl.position_z);
    }

    RetroRenderer::BeginFrame();

    glm::mat4 view_matrix, proj_matrix;
    Camera::GetViewMatrix(view_matrix);
    Camera::GetProjectionMatrix(proj_matrix);

    RetroRenderer::SetViewMatrix(view_matrix);
    RetroRenderer::SetProjectionMatrix(proj_matrix);

    for (const auto &prim : _instance->mesh.pbr_primitives)
    {
        
        if (prim.mesh_handle >= 0)
        {
            RetroRenderer::DrawRegisteredMeshPBR(
                prim.mesh_handle,
                &_instance->mesh.model_matrix,
                prim.transparent);
        }
        else
        {
            
            RetroRenderer::DrawIndexedTexturedTriangleArrayPBR(
                const_cast<TexturedVertex *>(prim.vertices.data()),
                prim.vertices.size(),
                const_cast<uint32_t *>(prim.indices.data()),
                prim.indices.size(),
                &_instance->mesh.model_matrix,
                prim.material,
                prim.transparent);
        }
    }

    
    if (_instance->white_texture_id != 0)
    {
        glm::vec3 cam_pos;
        Camera::GetPosition(cam_pos);

        const float billboard_size = 10.0f;

        for (int i = 0; i < NUM_ORBITING_LIGHTS; ++i)
        {
            glm::vec3 light_pos = light_positions[i];

            
            glm::vec3 to_camera = glm::normalize(cam_pos - light_pos);
            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
            glm::vec3 right = glm::normalize(glm::cross(up, to_camera));
            glm::vec3 billboard_up = glm::cross(to_camera, right);

            
            TexturedVertex verts[4];
            glm::vec3 offsets[4] = {
                -right * billboard_size - billboard_up * billboard_size,
                right * billboard_size - billboard_up * billboard_size,
                right * billboard_size + billboard_up * billboard_size,
                -right * billboard_size + billboard_up * billboard_size};
            glm::vec2 uvs[4] = {{0, 1}, {1, 1}, {1, 0}, {0, 0}};

            
            glm::vec4 color(colors[i].r / 255.0f, colors[i].g / 255.0f, colors[i].b / 255.0f, 1.0f);

            for (int v = 0; v < 4; ++v)
            {
                glm::vec3 pos = light_pos + offsets[v];
                verts[v].x = pos.x;
                verts[v].y = pos.y;
                verts[v].z = pos.z;
                verts[v].u = uvs[v].x;
                verts[v].v = uvs[v].y;
                verts[v].r = color.r;
                verts[v].g = color.g;
                verts[v].b = color.b;
                verts[v].a = color.a;
                verts[v].nx = to_camera.x;
                verts[v].ny = to_camera.y;
                verts[v].nz = to_camera.z;
            }

            uint32_t indices[6] = {0, 1, 2, 0, 2, 3};

            
            PBRMaterial mat;
            mat.albedo_texture = _instance->white_texture_id;
            mat.emissive_texture = _instance->white_texture_id;
            mat.emissive_strength = 5.0f;
            mat.metallic_factor = 0.0f;
            mat.roughness_factor = 1.0f;

            glm::mat4 identity(1.0f);
            RetroRenderer::DrawIndexedTexturedTriangleArrayPBR(
                verts, 4, indices, 6, &identity, mat, false);
        }
    }

    if (_instance->skybox_texture_id != 0)
    {
        RetroRenderer::DrawSkySphere(_instance->skybox_texture_id);
    }

    DebugMenu::Render();

    RetroRenderer::RenderFrame();
    return 0;
}

int MainLoop::Quit()
{
    if (_instance == nullptr)
        return -1;
    Camera::Quit();
    delete _instance;
    return 0;
}
