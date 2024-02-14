#pragma once
#include <TilemapRenderer.hpp>

class MapRenderer
{
    public:
        MapRenderer(camera *camera);
    private:
        camera *camera;
        std::vector<TilemapRenderer*> tilemap_renderers;
};