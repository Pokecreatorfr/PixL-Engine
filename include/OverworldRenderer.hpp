#pragma once

#include <MapRenderer.hpp>



class OverworldRenderer
{
    public:
        OverworldRenderer(camera *camera);
    private:
        camera *camera;
        std::vector<MapRenderer*> map_renderers;


};