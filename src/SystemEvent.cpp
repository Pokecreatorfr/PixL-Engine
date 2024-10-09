#include <SystemEvent.hpp>

SystemEvent::SystemEvent(SDL_Event *event)
{
    this->_event = event;
    this->quit = false;
    for (int i = 0; i < SDL_NUM_SCANCODES; i++)
    {
        this->keyboardInputs[i] = false;
    }
    this->mousePosition = {0, 0};
    for (int i = 0; i < 3; i++)
    {
        this->mouseButtons[i] = false;
    }
    this->mouseWheel = 0;
}

SystemEvent *SystemEvent::_instance = nullptr;

SystemEvent *SystemEvent::getInstance(SDL_Event *event)
{
    if (SystemEvent::_instance == nullptr)
    {
        SystemEvent::_instance = new SystemEvent(event);
    }
    return SystemEvent::_instance;
}

void SystemEvent::update()
{
    while (SDL_PollEvent(this->_event))
    {
        switch (this->_event->type)
        {
        case SDL_QUIT:
            this->quit = true;
            break;
        case SDL_KEYDOWN:
            this->keyboardInputs[this->_event->key.keysym.scancode] = true;
            break;
        case SDL_KEYUP:
            this->keyboardInputs[this->_event->key.keysym.scancode] = false;
            break;
        case SDL_MOUSEMOTION:
            this->mousePosition = {this->_event->motion.x, this->_event->motion.y};
            break;
        case SDL_MOUSEBUTTONDOWN:
            this->mouseButtons[this->_event->button.button - 1] = true;
            break;
        case SDL_MOUSEBUTTONUP:
            this->mouseButtons[this->_event->button.button - 1] = false;
            break;
        case SDL_MOUSEWHEEL:
            this->mouseWheel = this->_event->wheel.y;
            break;
        default:
            break;
        }
    }
}
