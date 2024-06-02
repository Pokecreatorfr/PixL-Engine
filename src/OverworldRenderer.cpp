#include "OverworldRenderer.hpp"

OverworldRenderer::OverworldRenderer()
{
    camera = Camera::GetInstance();
    update();
    shader = new OpenGLShader(Shader_basic_shader);
}

OverworldRenderer::~OverworldRenderer()
{
}

void OverworldRenderer::draw()
{
    for(int i = 0; i < maps.size(); i++)
    {
        maps[i]->draw_layer0();
        maps[i]->draw_layer1();
        maps[i]->draw_layer2();
    }
}

void OverworldRenderer::update()
{
    // delete maps that are not visible
    for(int i = maps.size() - 1; i >= 0; i--)
    {
        if(!check_overworld_rect_visibility(maps[i]->get_map_pos_x(), maps[i]->get_map_pos_y(), maps[i]->get_width(), maps[i]->get_height()))
        {
            std::cout << "deleting map : " << maps[i]->get_uid() << std::endl;
            delete maps[i];
            maps.erase(maps.begin() + i);
        }
    }
    // load maps that are visible and not loaded
    for(int i = 0; i < overworld_maps.size(); i++)
    {
        if(!check_if_map_is_loaded(overworld_maps[i]->uid) && check_overworld_rect_visibility(overworld_maps[i]->map_pos_x, overworld_maps[i]->map_pos_y, overworld_maps[i]->width, overworld_maps[i]->height))
        {
            std::cout << "loading map : " << overworld_maps[i]->uid << std::endl;
            Tileset *ts0 = get_tileset(overworld_maps[i]->tile_layer_0.tileset);
            Tileset *ts1 = get_tileset(overworld_maps[i]->tile_layer_1.tileset);
            Tileset *ts2 = get_tileset(overworld_maps[i]->tile_layer_2.tileset);
            maps.push_back(new MapRenderer(overworld_maps[i], ts0, ts1, ts2));
        }
    }
    // check if some tilesets are not used anymore
    for(int i = tilesets.size() - 1; i >= 0; i--)
    {
        bool is_used = false;
        for(int j = 0; j < maps.size(); j++)
        {
            std::vector<int> tilesets_uid = maps[j]->get_tilesets_uid();
            for(int k = 0; k < tilesets_uid.size(); k++)
            {
                if(tilesets_uid[k] == tilesets[i]->get_uid())
                {
                    is_used = true;
                    break;
                }
            }
            if(is_used)
            {
                break;
            }
        }
        if(!is_used)
        {
            std::cout << "deleting tileset : " << tilesets[i]->get_uid() << std::endl;
            delete tilesets[i];
            tilesets.erase(tilesets.begin() + i);
        }
    }
}

bool OverworldRenderer::check_if_map_is_loaded(int uid)
{
    for(int i = 0; i < maps.size(); i++)
    {
        if(maps[i]->get_uid() == uid)
        {
            return true;
        }
    }
    return false;
}

Tileset *OverworldRenderer::get_tileset(const tileset* ts)
{
    for(int i = 0; i < tilesets.size(); i++)
    {
        if(tilesets[i]->get_uid() == ts->uid)
        {
            return tilesets[i];
        }
    }
    std::cout << "loading tileset : " << ts->uid << std::endl;
    Tileset *new_tileset = new Tileset(ts);
    tilesets.push_back(new_tileset);
    return new_tileset;
}