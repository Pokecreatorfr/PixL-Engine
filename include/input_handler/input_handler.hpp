#pragma once
#include <SDL3/SDL.h>
#include <game/camera.hpp>
#include <map>
#include <vector>

class InputHandler
{
    InputHandler();
    static InputHandler *_instance;

public:
    static int Init();
    static int Quit();
    static int Update();
    static int Bind_KeyCode(SDL_Keycode keycode, bool *state);
    static int Bind_QuitRequest(bool *state);

    static int Bind_UpdateScreenRatio();
    static int Unbind_KeyCode(SDL_Keycode keycode);
    static int Unbind_QuitRequest();
    static int Unbind_UpdateScreenRatio();

private:
    static void ClearBindings();
    static std::map<SDL_Keycode, bool *> keycode_bindings;
    static bool *quit_requested;
    static bool update_screen_ratio;
};