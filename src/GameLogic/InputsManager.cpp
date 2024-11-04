#include <GameLogic/InputsManager.hpp>

InputsManager::InputsManager(SystemEvent *event)
{
    this->event = event;
}

InputsManager *InputsManager::_instance = nullptr;

InputsManager *InputsManager::get_instance(SystemEvent *event)
{
    if (InputsManager::_instance == nullptr)
    {
        InputsManager::_instance = new InputsManager(event);
    }
    return InputsManager::_instance;
}

InputsManager::~InputsManager()
{
}

void InputsManager::update()
{
    for (int i = 0; i < this->keys.size(); i++)
    {
        if (this->event->isKeyPressed(this->keys[i]))
        {
            this->keysPressed[i] = true;
        }
        else
        {
            this->keysPressed[i] = false;
        }
    }
}

void InputsManager::newKeyBind(std::vector<SDL_Scancode> keys)
{
    this->keys = keys;
    this->keysPressed = std::vector<bool>(keys.size(), false);
}

std::vector<bool> InputsManager::getKeysPressed()
{
    return this->keysPressed;
}

SystemEvent *InputsManager::getEvent()
{
    return this->event;
}
