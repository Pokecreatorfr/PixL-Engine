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

bool check_visibility(coord_2d position, coord_2d size, camera cam)
{
	float camsx = cam.size.x / 2;
	float camsy = cam.size.y / 2;
	float camsxzoom = camsx * ( 1/cam.zoom );
	float camsyzoom = camsy * ( 1/cam.zoom );

	return (position.x * TILE_SIZE <= cam.position.x + camsxzoom) && ((position.x + size.x) * TILE_SIZE >= cam.position.x - camsxzoom) && (position.y * TILE_SIZE <= cam.position.y + camsyzoom) && ((position.y + size.y) * TILE_SIZE >= cam.position.y - camsyzoom);

}