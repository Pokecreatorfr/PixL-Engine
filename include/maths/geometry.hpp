#pragma once
#include <SDL2/SDL.h>

bool isPointInRect(SDL_Point point, SDL_Rect rect);

bool isRectInRect(SDL_Rect rect1, SDL_Rect rect2);

bool isRectInRectStrict(SDL_Rect rect1, SDL_Rect rect2);

bool isRectsIntersect(SDL_Rect rect1, SDL_Rect rect2);