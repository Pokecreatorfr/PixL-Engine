#include "Tileset.hpp"

Tileset::Tileset(const tileset* tileset)
{
    texture = new OpenGLTexture(tileset->ressource);
    tile_width = tileset->tileset_width;
    tile_height = tileset->tileset_height;
    uid = tileset->uid;
}

Tileset::~Tileset()
{
    delete texture;
}

int Tileset::get_tile_width()
{
    return tile_width;
}

int Tileset::get_tile_height()
{
    return tile_height;
}

int Tileset::get_uid()
{
    return uid;
}

OpenGLTexture *Tileset::get_texture()
{
    return texture;
}
