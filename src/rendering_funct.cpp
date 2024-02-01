#include <rendering_funct.hpp>


bool check_visibility(coord_2d position, coord_2d size, camera camera)
{
	float camsx = camera.size.x / 2;
	float camsy = camera.size.y / 2;
	float camsxzoom = - camsx * ( 1 - 1/camera.zoom );
	float camsyzoom = - camsy * ( 1 - 1/camera.zoom );
	//cout << "camsx : " << camsx << " camsxzoom : " << camsxzoom << " addition : " << camsx + camsxzoom << endl;

	if ( (position.x - 1) * TILE_SIZE <= (camera.position.x + camsx + camsxzoom) && (position.x + size.x + 1) * TILE_SIZE >= (camera.position.x - camsx - camsxzoom) && (position.y - 1) * TILE_SIZE <= (camera.position.y + camsy + camsyzoom) && (position.y + size.y + 1) * TILE_SIZE >= (camera.position.y - camsy - camsyzoom))
	{
		return true;
	}
	return false;
}