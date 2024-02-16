#include <math.h>

int adjustedSize(int originalSize, float zoom)
{
    return static_cast<int>(ceil(originalSize * zoom));
}

int calculateExtraPixels(int originalSize, float zoom)
{
    // Calcule la taille ajustée
    int adjusted = adjustedSize(originalSize, zoom);

    return adjusted - originalSize;
}

bool check_visibility(int camx, int camy, int camw, int camh, int rectx, int recty, int rectw, int recth)
{
    // Vérifie si le rectangle est visible
    return (rectx + rectw > camx && rectx < camx + camw && recty + recth > camy && recty < camy + camh);
}