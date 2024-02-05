#pragma once

#include <Renderer.hpp>
#include <overworld_vars.hpp>
#include <map_struct.hpp>
#include <const/const_renderer.hpp>
#include <tileset.hpp>
#include <generated/tileset2cpp.hpp>

class TilemapRenderer
{
public:
	TilemapRenderer(map_struct* map ,int layer);
	void Draw(Tileset* tileset,Camera camera,SDL_Renderer* renderer);
private:
	vector<Tile> tiles;
    Tileset* tileset;
};

class MapRenderer
{
    public:

    private:

}