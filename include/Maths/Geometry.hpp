#pragma once

#include <GameLogic/GameLogicHandler.hpp>
#include <SDL2/SDL.h>
#include <vector>

struct camera;

bool checkRectVisible(camera *camInfo, SDL_Rect *rect);
bool checkRectVisibleWithOffset(camera *camInfo, SDL_Rect *rect, int offsetX, int offsetY);