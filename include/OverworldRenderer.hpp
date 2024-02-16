#pragma once

#include <MapRenderer.hpp>
#include <generated/map2cpp.hpp>


class OverworldRenderer
{
    public:
        OverworldRenderer(camera *cam);
        ~OverworldRenderer();
        void check_maps_visibility();
    private:
        bool check_map_visibility(const map_struct* map);
        camera *Camera;
        std::vector<MapRenderer*> map_renderers;


};