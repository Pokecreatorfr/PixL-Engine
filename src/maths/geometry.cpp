#include <maths/geometry.hpp>

bool isPointInRect(SDL_Point point, SDL_Rect rect)
{
    return point.x >= rect.x && point.x <= rect.x + rect.w && point.y >= rect.y && point.y <= rect.y + rect.h;
}

bool isRectInRect(SDL_Rect rect1, SDL_Rect rect2)
{
    return rect1.x <= rect2.x + rect2.w && rect1.x + rect1.w >= rect2.x && rect1.y <= rect2.y + rect2.h && rect1.y + rect1.h >= rect2.y;
}

bool isRectInRectStrict(SDL_Rect rect1, SDL_Rect rect2)
{
    return rect1.x < rect2.x + rect2.w && rect1.x + rect1.w > rect2.x && rect1.y < rect2.y + rect2.h && rect1.y + rect1.h > rect2.y;
}
bool isRectsIntersect(SDL_Rect rect1, SDL_Rect rect2)
{
    return isRectInRect(rect1, rect2) || isRectInRect(rect2, rect1);
}