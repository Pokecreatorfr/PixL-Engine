#pragma once 
#include <SDL2/SDL.h>
#include <string>

enum ButtonState
{
    BUTTON_STATE_NORMAL,
    BUTTON_STATE_HOVER,
    BUTTON_STATE_CLICK,
};

class Button
{
public:
    Button();
    int x, y = 0;
    int w, h = 0;
    Uint32 color = 0;
    SDL_Texture* texture = nullptr;
    ButtonState state = BUTTON_STATE_NORMAL;
    ButtonState prevState = BUTTON_STATE_NORMAL;
    std::string text = "";
    void (*onClick)(void) = nullptr;
    void (*onHover)(void) = nullptr;
    bool visible = true;
    bool prevVisible = false;
private:
    int index = 0;
};

struct ButtonInfo
{
    int x, y = 0;
    int w, h = 0;
    Uint32 color = 0;
    Uint32 hoverColor = 0;
    Uint32 clickColor = 0;
    SDL_Texture* texture = nullptr;
    SDL_Texture* hoverTexture = nullptr;
    SDL_Texture* clickTexture = nullptr;
    std::string text = "";
    void (*onClick)(void);
    void (*onHover)(void);
};

struct BuildedButton
{
    Button button;
    ButtonInfo buttonInfo;
};