#pragma once
#include <TilemapRenderer.hpp>

class MapRenderer
{
    public:
        MapRenderer(camera *cam ,const map_struct* map);
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
        camera *Camera;
        std::vector<Tileset*> tilesets;
        std::vector<TilemapRenderer*> tilemap_renderers;
        void draw(tiles_layer tl , Tileset* ts);
        const map_struct* map;
        
};