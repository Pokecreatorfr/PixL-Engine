#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <input_handler/input_handler.hpp>
#include <iostream>

InputHandler::InputHandler() {};

InputHandler *InputHandler::_instance = nullptr;

std::map<SDL_Keycode, bool *> InputHandler::keycode_bindings;
bool *InputHandler::quit_requested = nullptr;
bool InputHandler::update_screen_ratio = false;

int InputHandler::Init()
{
    if (_instance != nullptr)
        return -1;
    _instance = new InputHandler();
    return 0;
}

int InputHandler::Quit()
{
    if (_instance == nullptr)
        return -1;
    delete _instance;
    return 0;
}

int InputHandler::Update()
{
    if (_instance == nullptr)
        return -1;
    ClearBindings();
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL3_ProcessEvent(&event);

        if (event.type == SDL_EVENT_KEY_DOWN)
        {
            SDL_Keycode keycode = event.key.key;
            bool is_pressed = (event.type == SDL_EVENT_KEY_DOWN);
            auto it = keycode_bindings.find(keycode);
            if (it != keycode_bindings.end())
            {
                *(it->second) = is_pressed;
            }
        }
        else if (quit_requested != nullptr && event.type == SDL_EVENT_QUIT)
        {
            *quit_requested = true;
        }
        else if (update_screen_ratio && event.type == SDL_EVENT_WINDOW_RESIZED)
        {
            int w = event.display.data1;
            int h = event.display.data2;
            Camera::SetAspect(static_cast<float>(w) / static_cast<float>(h));
            std::cout << "Updated camera aspect ratio to " << static_cast<float>(w) / static_cast<float>(h) << " due to window resize." << std::endl;
        }
    }
    return 0;
}

void InputHandler::ClearBindings()
{
    if (_instance == nullptr)
        return;
    for (auto &binding : keycode_bindings)
    {
        *(binding.second) = false;
    }
    if (quit_requested != nullptr)
        *quit_requested = false;
}

int InputHandler::Bind_KeyCode(SDL_Keycode keycode, bool *state)
{
    if (_instance == nullptr)
        return -1;
    bool already_bound = (keycode_bindings.find(keycode) != keycode_bindings.end());
    keycode_bindings[keycode] = state;
    if (already_bound)
        return 1; 
    return 0;
}

int InputHandler::Bind_QuitRequest(bool *state)
{
    if (_instance == nullptr)
        return -1;
    quit_requested = state;
    return 0;
}

int InputHandler::Bind_UpdateScreenRatio()
{
    if (_instance == nullptr)
        return -1;
    int errc = Camera::Init();
    if (errc != 0 && errc != -1)
        return -2;
    update_screen_ratio = true;
    return 0;
}

int InputHandler::Unbind_KeyCode(SDL_Keycode keycode)
{
    if (_instance == nullptr)
        return -1;
    auto it = keycode_bindings.find(keycode);
    if (it != keycode_bindings.end())
    {
        keycode_bindings.erase(it);
        return 0;
    }
    return 1; 
}

int InputHandler::Unbind_QuitRequest()
{
    if (_instance == nullptr)
        return -1;
    quit_requested = nullptr;
    return 0;
}

int InputHandler::Unbind_UpdateScreenRatio()
{
    if (_instance == nullptr)
        return -1;
    update_screen_ratio = false;
    return 0;
}