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
            overworld_maps[i]->map_pos_x + overworld_maps[i]->width * 2< overworld_struct->camera.x + (overworld_struct->camera.width * ( 1/ overworld_struct->camera.zoom) / 2 )
            &&
            overworld_maps[i]->map_pos_x + overworld_maps[i]->width > overworld_struct->camera.x - (overworld_struct->camera.width * ( 1/ overworld_struct->camera.zoom) / 2 )
            &&
            overworld_maps[i]->map_pos_y + overworld_maps[i]->height * 2< overworld_struct->camera.y + (overworld_struct->camera.height * ( 1/ overworld_struct->camera.zoom) / 2 )
            &&
            overworld_maps[i]->map_pos_y + overworld_maps[i]->height > overworld_struct->camera.y - (overworld_struct->camera.height * ( 1/ overworld_struct->camera.zoom) / 2 )
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

void Overworld::draw()
{
    for(int i = 0 ; i < 3 ; i++)
    {
        SDL_Texture* drawed_layer = this->layers[i];
        SDL_SetRenderTarget(this->renderer, drawed_layer);
        SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 0);
        SDL_RenderClear(this->renderer);
        for( int j = 0 ; j < maps.size() ; j++)
        {
            int relative_pos_x = maps[j]->map_pos_x - overworld_struct->camera.x;
            int relative_pos_y = maps[j]->map_pos_y - overworld_struct->camera.y;
            const std::vector<int>* actual_layer = nullptr;
            switch (j)
            {
            case 0:
                actual_layer = &maps[j]->tile_layer_0;
                break;
            case 1:
                actual_layer = &maps[j]->tile_layer_1;
                break;
            case 2:
                actual_layer = &maps[j]->tile_layer_2;
                break;
            }
            for(int x = 0 ; x < maps[j]->width ; x++)
            {
                for(int y = 0 ; y < maps[j]->height ; y++)
                {
                    int tile_index = (*actual_layer)[x + y * maps[j]->width];
                    if(tile_index != -1)
                    {
                        this->tilesets[j]->draw_tile(tile_index, relative_pos_x + x * TILE_SIZE, relative_pos_y + y * TILE_SIZE);
                    }
                }
            }
        }
    }
}
