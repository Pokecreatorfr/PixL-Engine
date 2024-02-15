#pragma once
#include <TilemapRenderer.hpp>
#include <MapStruct.hpp>

class MapRenderer
{
    public:
        MapRenderer(camera *cam , map_struct map);
        int get_uid();
        int get_width();
        int get_height();
        int get_map_pos_x();
        int get_map_pos_y();
        void draw_layer0();
        void draw_layer1();
        void draw_layer2();
        std::vector<int> get_tilesets_uid();
    private:
        camera *Camera;
        map_struct map;
        std::vector<Tileset*> tilesets;
};