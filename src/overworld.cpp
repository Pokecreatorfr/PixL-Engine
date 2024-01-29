#include <overworld.hpp>

Overworld::Overworld(SDL_Renderer* renderer, overworld_vars* overworld_struct, std::vector<SDL_Texture*> layers)
{
    this->renderer = renderer;
    this->overworld_struct = overworld_struct;
    this->layers = layers;
}

Overworld::~Overworld()
{
    for(int i = 0 ; i < this->tilesets.size() ; i++)
    {
        delete this->tilesets[i];
    }
}

void Overworld::load_tileset(const tileset *tileset)
{
    this->tilesets.push_back(new Tileset(this->renderer, tileset, this->overworld_struct));
}

void Overworld::load_map(const map_struct * map)
{
    this->maps.push_back(map);
}

void Overworld::update()
{
    for(int i = 0 ; i <maps.size() ; i++)
    {
        if( 

            maps[i]->map_pos_x + maps[i]->width < overworld_struct->camera.x - (overworld_struct->camera.width * ( 1/ overworld_struct->camera.zoom) / 2 )
            ||
            maps[i]->map_pos_x > overworld_struct->camera.x + (overworld_struct->camera.width * ( 1/ overworld_struct->camera.zoom) / 2 )
            ||
            maps[i]->map_pos_y + maps[i]->height < overworld_struct->camera.y - (overworld_struct->camera.height * ( 1/ overworld_struct->camera.zoom) / 2 )
            ||
            maps[i]->map_pos_y > overworld_struct->camera.y + (overworld_struct->camera.height * ( 1/ overworld_struct->camera.zoom) / 2 )

        )
        {
            maps.erase(maps.begin() + i);
        }
    }
    for(int i = 0 ; i < overworld_maps.size() ; i++)
    {
        if
        ( 
            maps[i]->map_pos_x + maps[i]->width * 2< overworld_struct->camera.x + (overworld_struct->camera.width * ( 1/ overworld_struct->camera.zoom) / 2 )
            &&
            maps[i]->map_pos_x + maps[i]->width > overworld_struct->camera.x - (overworld_struct->camera.width * ( 1/ overworld_struct->camera.zoom) / 2 )
            &&
            maps[i]->map_pos_y + maps[i]->height * 2< overworld_struct->camera.y + (overworld_struct->camera.height * ( 1/ overworld_struct->camera.zoom) / 2 )
            &&
            maps[i]->map_pos_y + maps[i]->height > overworld_struct->camera.y - (overworld_struct->camera.height * ( 1/ overworld_struct->camera.zoom) / 2 )
        )
        {
            bool map_present = false;
            for(int j = 0 ; j < maps.size() ; j++)
            {
                if(maps[j] == overworld_maps[i])
                {
                    map_present = true;
                }
            }
            if(!map_present)
            {
                maps.push_back(overworld_maps[i]);
            }
        }

    }
}