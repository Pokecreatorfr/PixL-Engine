#include <input_handler/input_handler.hpp>

InputHandler::InputHandler() {};

InputHandler *InputHandler::_instance = nullptr;

std::map<SDL_Keycode, bool *> InputHandler::keycode_bindings;
bool *InputHandler::quit_requested = nullptr;

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
        return 1; // Keycode was already bound
    return 0;
}

int InputHandler::Bind_QuitRequest(bool *state)
{
    if (_instance == nullptr)
        return -1;
    quit_requested = state;
    return 0;
}