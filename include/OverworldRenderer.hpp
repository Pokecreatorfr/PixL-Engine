#pragma once

#include <MapRenderer.hpp>
#include <generated/map2cpp.hpp>


class OverworldRenderer
{
    public:
        OverworldRenderer(camera *cam);
        ~OverworldRenderer();
        void check_maps_visibility();
        void draw();
    private:
        bool check_map_visibility(const map_struct* map);
        void check_tilesets_usage();
        camera *Camera;
        std::vector<MapRenderer*> map_renderers;
        std::vector<Tileset*> tilesets;
};