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
    // check if map tileset is already loaded
    bool tileset_loaded = false;
    for (int i = 0 ; i < this->tilesets.size(); i++)
    {
        if(this->tilesets[i]->get_tileset_uid() == map->tileset->uid)
        {
            tileset_loaded = true;
        }
    }
    if(!tileset_loaded)
    {
        this->load_tileset(map->tileset);
    }
}

void Overworld::unload_map(int index)
{
    int t_uid = maps[index]->tileset->uid;
    this->maps.erase(this->maps.begin() + index);
    bool tileset_used = false;
    for(int i = 0 ; i < this->maps.size() ; i++)
    {
        if(this->maps[i]->tileset->uid == t_uid)
        {
            tileset_used = true;
        }
    }
    if(!tileset_used)
    {
        for(int i = 0 ; i < this->tilesets.size() ; i++)
        {
            if(this->tilesets[i]->get_tileset_uid() == t_uid)
            {
                delete this->tilesets[i];
                this->tilesets.erase(this->tilesets.begin() + i);
            }
        }
    }
}

void Overworld::update()
{
    for(int i = 0 ; i <maps.size() ; i++)
    {
        if( 

            maps[i]->map_pos_x + maps[i]->width < overworld_struct->camera.x - (overworld_struct->camera.width * ( 1/ overworld_struct->camera.zoom) / 2 ) ||
            maps[i]->map_pos_x > overworld_struct->camera.x + (overworld_struct->camera.width * ( 1/ overworld_struct->camera.zoom) / 2 ) ||
            maps[i]->map_pos_y + maps[i]->height < overworld_struct->camera.y - (overworld_struct->camera.height * ( 1/ overworld_struct->camera.zoom) / 2 ) ||
            maps[i]->map_pos_y > overworld_struct->camera.y + (overworld_struct->camera.height * ( 1/ overworld_struct->camera.zoom) / 2 )

        )
        {
            unload_map(i);
        }
    }
    for(int i = 0 ; i < overworld_maps.size() ; i++)
    {
        if
        ( 
            maps[i]->map_pos_x + maps[i]->width * 2< overworld_struct->camera.x + (overworld_struct->camera.width * ( 1/ overworld_struct->camera.zoom) / 2 ) &&
            maps[i]->map_pos_x + maps[i]->width > overworld_struct->camera.x - (overworld_struct->camera.width * ( 1/ overworld_struct->camera.zoom) / 2 ) &&
            maps[i]->map_pos_y + maps[i]->height * 2< overworld_struct->camera.y + (overworld_struct->camera.height * ( 1/ overworld_struct->camera.zoom) / 2 ) &&
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
                load_map(overworld_maps[i]);
            }
        }

    }
}