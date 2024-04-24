#pragma once
#include <vector>
#include <SDL2/SDL.h>

enum ActionKey
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    START,
    SELECT,
};

class Save
{
    protected:
        Save();
        static Save* instance_;
    public:
        static Save* GetInstance();
        std::vector<int> GetKeys();
    private:
        // vector with size of enum
        std::vector<int> keys = {SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT, SDL_SCANCODE_RETURN, SDL_SCANCODE_SPACE};
};