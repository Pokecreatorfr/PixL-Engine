#pragma once
#include <SDL2/SDL.h>

using namespace std;

struct pointInt
{
    int x;
    int y;
};

class SystemEvent
{
protected:
    SystemEvent(SDL_Event *event, SDL_Window *window);
    static SystemEvent *_instance;

public:
    static SystemEvent *getInstance(SDL_Event *event, SDL_Window *window);
    void update();
    bool isKeyPressed(SDL_Scancode key) { return this->keyboardInputs[key]; }
    bool isMouseButtonPressed(int button) { return this->mouseButtons[button]; }
    bool *getKeyboardInputs() { return this->keyboardInputs; }
    pointInt *getMousePosition() { return &this->mousePosition; }
    int16_t getMouseWheel() { return this->mouseWheel; }
    bool isQuit() { return this->quit; }
    pointInt getWindowSize() { return this->windowSize; }

private:
    SDL_Event *_event;
    bool keyboardInputs[SDL_NUM_SCANCODES];
    pointInt mousePosition;
    bool mouseButtons[3];
    int16_t mouseWheel;
    bool quit;
    SDL_Window *window;
    pointInt windowSize;
};
