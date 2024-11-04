#pragma once

#include <SystemEvent.hpp>
#include <iostream>
#include <vector>

class InputsManager
{
protected:
    InputsManager(SystemEvent *event);
    static InputsManager *_instance;

public:
    static InputsManager *get_instance(SystemEvent *event);
    ~InputsManager();
    void update();
    void newKeyBind(std::vector<SDL_Scancode> keys);
    std::vector<bool> getKeysPressed();
    SystemEvent *getEvent();

private:
    SystemEvent *event;
    std::vector<SDL_Scancode> keys;
    std::vector<bool> keysPressed;
};