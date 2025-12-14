#include <array>
#include <cmath>
#include <cstdint>
#include <functional>
#include <numeric>
#include <vector>

#include <SDL3/SDL.h>
#include <algorithm>
#include <asset_loader/asset_loader.hpp>
#include <filesystem>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <limits>
#include <memory>
#include <retro_renderer/retro_renderer.hpp>
#include <string>

int main(int argc, char *argv[])
{
    if (RetroRenderer::Init(800, 600, "Retro Renderer Example") != 0)
    {
        return -1;
    }
    std::vector<LoadedPrimitive> primitives;

    if (loadGltfFromFile("test.glb", primitives) != 0)
    {
        SDL_Log("Failed to load GLTF file\n");
        RetroRenderer::Quit();
        return -1;
    }

    std::vector<int> mesh_handles;
    mesh_handles.reserve(primitives.size());

    for (auto &prim : primitives)
    {
        if (!prim.texture_pixels.empty())
        {
            TextureID tex_id = 0;
            if (RetroRenderer::LoadTexture(reinterpret_cast<char *>(prim.texture_pixels.data()), prim.texture_pixels.size(), prim.tex_w, prim.tex_h, tex_id) == 0)
            {
                prim.texture_id = tex_id;
            }
        }

        int handle = -1;
        int reg_err = -1;
        if (prim.texture_id != 0)
        {
            reg_err = RetroRenderer::RegisterIndexedTexturedTriangleMesh(prim.vertices.data(), prim.vertices.size(), prim.indices.data(), prim.indices.size(), prim.texture_id, handle);
        }
        else
        {
            reg_err = RetroRenderer::RegisterIndexedTriangleMesh(reinterpret_cast<Vertex *>(prim.vertices.data()), prim.vertices.size(), prim.indices.data(), prim.indices.size(), handle);
        }
        if (reg_err != 0)
        {
            SDL_Log("Failed to register primitive mesh (err=%d)", reg_err);
            handle = -1;
        }
        mesh_handles.push_back(handle);
    }

    glm::vec3 scene_min(std::numeric_limits<float>::max());
    glm::vec3 scene_max(std::numeric_limits<float>::lowest());
    for (const auto &p : primitives)
    {
        scene_min = glm::min(scene_min, p.aabb_min);
        scene_max = glm::max(scene_max, p.aabb_max);
    }
    glm::vec3 scene_center = (scene_min + scene_max) * 0.5f;
    glm::vec3 scene_size = scene_max - scene_min;
    float scene_extent = std::max(std::max(scene_size.x, scene_size.y), std::max(scene_size.z, 0.001f));
    float model_scale = (scene_extent > 0.0f) ? (2.0f / scene_extent) : 1.0f;

    int win_w = 800, win_h = 600;
    SDL_GetWindowSizeInPixels(RetroRenderer::GetWindow(), &win_w, &win_h);

    auto update_projection = [&](int w, int h)
    {
        float aspect = (h != 0) ? static_cast<float>(w) / static_cast<float>(h) : 1.0f;
        RetroRenderer::SetProjectionMatrix(glm::perspective(glm::radians(60.0f), aspect, 0.01f, 2000.0f));
    };
    update_projection(win_w, win_h);

    float yaw = 0.0f;
    float pitch = 0.0f;
    float radius = (scene_extent * 0.5f) * model_scale;
    float distance = std::max(2.5f * radius, 2.0f);
    glm::vec3 target = glm::vec3(0.0f);
    glm::vec3 pan(0.0f);
    bool dragging = false;

    auto update_view = [&]()
    {
        glm::vec3 forward;
        forward.x = std::cos(pitch) * std::sin(yaw);
        forward.y = std::sin(pitch);
        forward.z = std::cos(pitch) * std::cos(yaw);
        glm::vec3 eye = target + pan - forward * distance;
        RetroRenderer::SetViewMatrix(glm::lookAt(eye, target + pan, glm::vec3(0.0f, 1.0f, 0.0f)));
    };
    update_view();

    AmbiantLightInfo amb{255, 255, 255, 0.4f};
    RetroRenderer::SetAmbiantLight(amb);
    DirectionalLightInfo dir{};
    dir.r = dir.g = dir.b = 255;
    dir.intensity = 0.8f;
    dir.direction_x = -0.4f;
    dir.direction_y = -1.0f;
    dir.direction_z = -0.3f;
    RetroRenderer::SetDirectionalLight(dir);

    bool running = true;
    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
            else if (event.type == SDL_EVENT_WINDOW_RESIZED)
            {
                SDL_GetWindowSizeInPixels(RetroRenderer::GetWindow(), &win_w, &win_h);
                update_projection(win_w, win_h);
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT)
            {
                dragging = true;
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP && event.button.button == SDL_BUTTON_LEFT)
            {
                dragging = false;
            }
            else if (event.type == SDL_EVENT_MOUSE_MOTION && dragging)
            {
                const float sens = 0.005f;
                yaw += event.motion.xrel * sens;
                pitch += event.motion.yrel * sens;
                pitch = std::clamp(pitch, -1.5f, 1.5f);
                update_view();
            }
            else if (event.type == SDL_EVENT_MOUSE_WHEEL)
            {
                float zoom_factor = (event.wheel.y > 0) ? 0.9f : 1.1f;
                distance *= zoom_factor;
                distance = std::clamp(distance, 0.5f, 500.0f);
                update_view();
            }
            else if (event.type == SDL_EVENT_KEY_DOWN)
            {
                glm::vec3 forward;
                forward.x = std::cos(pitch) * std::sin(yaw);
                forward.y = 0.0f;
                forward.z = std::cos(pitch) * std::cos(yaw);
                forward = glm::normalize(forward);
                glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
                const float step = 0.2f;
                switch (event.key.key)
                {
                case SDLK_W:
                case SDLK_UP:
                    pan += forward * step;
                    break;
                case SDLK_S:
                case SDLK_DOWN:
                    pan -= forward * step;
                    break;
                case SDLK_A:
                case SDLK_LEFT:
                    pan -= right * step;
                    break;
                case SDLK_D:
                case SDLK_RIGHT:
                    pan += right * step;
                    break;
                default:
                    break;
                }
                update_view();
            }
        }

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(model_scale));
        model = glm::translate(model, -scene_center);
        RetroRenderer::SetModelMatrix(model);

        RetroRenderer::BeginFrame();
        for (size_t i = 0; i < mesh_handles.size(); ++i)
        {
            if (mesh_handles[i] < 0)
                continue;
            RetroRenderer::DrawRegisteredMesh(mesh_handles[i], &model, false);
        }
        RetroRenderer::RenderFrame();
    }

    RetroRenderer::Quit();

    return 0;
}
