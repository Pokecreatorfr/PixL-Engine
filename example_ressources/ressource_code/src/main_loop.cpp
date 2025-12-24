#include <SDL3/SDL.h>
#include <game/camera.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <input_handler/input_handler.hpp>
#include <iostream>
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

    Camera::SetPosition(glm::vec3(0.0f, 100.0f, 800.0f));
    Camera::SetYawPitchDeg(-90.0f, -5.0f);
    Camera::SetAspect(800.0f / 600.0f);
    Camera::SetFovDeg(30.0f);
    Camera::SetNearFar(1.0f, 5000.0f);

    _instance->mesh.model_matrix = glm::mat4(1.0f);
    _instance->mesh.model_matrix = glm::translate(_instance->mesh.model_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
    _instance->mesh.model_matrix = glm::scale(_instance->mesh.model_matrix, glm::vec3(1.0f, 1.0f, 1.0f));

    std::vector<LoadedPrimitive> loaded_primitives;
    int load_err = loadGltfFromFile("test.glb", loaded_primitives);
    if (load_err == 0 && !loaded_primitives.empty())
    {
        std::cout << "Loaded " << loaded_primitives.size() << " primitives from model" << std::endl;

        for (size_t i = 0; i < loaded_primitives.size(); ++i)
        {
            TextureID texture_id = 0;
            if (!loaded_primitives[i].texture_pixels.empty())
            {
                // Convert MaterialAlphaMode to AlphaMode
                AlphaMode alpha_mode = OPAQUE;
                switch (loaded_primitives[i].alpha_mode)
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

                int tex_err = RetroRenderer::LoadTexture(
                    reinterpret_cast<char *>(loaded_primitives[i].texture_pixels.data()),
                    loaded_primitives[i].texture_pixels.size(),
                    loaded_primitives[i].tex_w,
                    loaded_primitives[i].tex_h,
                    texture_id,
                    alpha_mode);
                if (tex_err != 0)
                {
                    std::cerr << "Failed to load texture for primitive " << i << ", error: " << tex_err << std::endl;
                    texture_id = 0;
                }
                else
                {
                    std::cout << "Texture loaded for primitive " << i << " with ID: " << texture_id << std::endl;
                }
            }

            int primitive_handle = -1;
            int reg_err = RetroRenderer::RegisterIndexedTexturedTriangleMesh(
                loaded_primitives[i].vertices.data(),
                loaded_primitives[i].vertices.size(),
                loaded_primitives[i].indices.data(),
                loaded_primitives[i].indices.size(),
                texture_id != 0 ? texture_id : 1, primitive_handle);
            if (reg_err == 0)
            {
                _instance->mesh.primitive_handles.push_back(primitive_handle);
                std::cout << "Primitive " << i << " registered with handle: " << primitive_handle << std::endl;
            }
            else
            {
                std::cerr << "Failed to register primitive " << i << ", error: " << reg_err << std::endl;
            }
        }

        std::cout << "Total primitives registered: " << _instance->mesh.primitive_handles.size() << std::endl;
    }
    else
    {
        std::cerr << "Failed to load mesh from test.glb, error: " << load_err << std::endl;
    }

    RetroRenderer::SetAmbiantLight(AmbiantLightInfo{255, 255, 255, 1.0f});

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
        std::cout << "Y key is pressed" << std::endl;

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

    RetroRenderer::BeginFrame();

    glm::mat4 view_matrix, proj_matrix;
    Camera::GetViewMatrix(view_matrix);
    Camera::GetProjectionMatrix(proj_matrix);

    RetroRenderer::SetViewMatrix(view_matrix);
    RetroRenderer::SetProjectionMatrix(proj_matrix);

    for (int handle : _instance->mesh.primitive_handles)
    {
        int draw_err = RetroRenderer::DrawRegisteredMesh(handle, &_instance->mesh.model_matrix);
        if (draw_err != 0)
        {
            std::cerr << "Failed to draw registered mesh handle " << handle << ", error: " << draw_err << std::endl;
        }
    }

    if (_instance->skybox_texture_id != 0)
    {
        RetroRenderer::DrawSkySphere(_instance->skybox_texture_id);
    }

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
