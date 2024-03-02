#pragma once
#include <TilemapRenderer.hpp>

class MapRenderer
{
    public:
        MapRenderer(const map_struct* map, Tileset* ts0, Tileset* ts1, Tileset* ts2);
        ~MapRenderer();
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
        Camera *Camera;
        std::vector<TilemapRenderer*> tilemap_renderers;
        const map_struct* map;
        
};