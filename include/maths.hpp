#pragma once

#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
SDL_FPoint screen_square_size(int w, int h);
#include <Camera.hpp>
bool check_overworld_rect_visibility(float x, float y, float w, float h);