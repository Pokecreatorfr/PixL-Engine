#include <maths.hpp>

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

bool check_visibility(coord_2d position, coord_2d size, Camera* cam)
{
	float camsx = cam->GetSize()->x / 2;
	float camsy = cam->GetSize()->y / 2;
	float camsxzoom = camsx * ( 1/ *cam->GetZoom() );
	float camsyzoom = camsy * ( 1/ *cam->GetZoom());

	return (position.x * TILE_SIZE <= cam->GetPosition()->x + camsxzoom) && ((position.x + size.x) * TILE_SIZE >= cam->GetPosition()->x - camsxzoom) && (position.y * TILE_SIZE <= cam->GetPosition()->y + camsyzoom) && ((position.y + size.y) * TILE_SIZE >= cam->GetPosition()->y - camsyzoom);

}