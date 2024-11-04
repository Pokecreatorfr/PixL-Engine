#include <Maths/Geometry.hpp>

bool checkRectVisible(camera *camInfo, SDL_Rect *rect)
{
    // camInfo x and y are the center of the camera
    // rect x and y are the top left corner of the rect
    // include zoom in the calculation
    float zoom = camInfo->zoom;
    int camX = camInfo->x - (camInfo->w / 2) * zoom;
    int camY = camInfo->y - (camInfo->h / 2) * zoom;

    int camW = camInfo->w * zoom;
    int camH = camInfo->h * zoom;

    // check if the rect is inside the camera
    if (rect->x + rect->w < camX || rect->x > camX + camW || rect->y + rect->h < camY || rect->y > camY + camH)
    {
        return false;
    }

    return true;
}

bool checkRectVisibleWithOffset(camera *camInfo, SDL_Rect *rect, int offsetX, int offsetY)
{
    // add security offset by adding the offset to the camera size
    camera camInfoOffset = {camInfo->x, camInfo->y, camInfo->h + offsetY, camInfo->w + offsetX};

    return checkRectVisible(&camInfoOffset, rect);
}