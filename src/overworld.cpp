#include <overworld.hpp>

Overworld::Overworld(SDL_Renderer* renderer, overworld_vars* overworld_struct, std::vector<SDL_Texture*> layers, Logger* logger)
{
    this->renderer = renderer;
    this->overworld_struct = overworld_struct;
    this->layers = layers;
    this->logger = logger;
    logger->log("Overworld created");
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
        coord_2d pos = {maps[i]->map_pos_x, maps[i]->map_pos_y};
        coord_2d size = {maps[i]->width, maps[i]->height};
        if(!check_visibility(pos, size , overworld_struct->camera))
        {
            logger->log("map " + std::to_string(i) + " unloaded");
            unload_map(i);
        }
    }
    for(int i = 0 ; i < overworld_maps.size() ; i++)
    {
        coord_2d pos = {overworld_maps[i]->map_pos_x, overworld_maps[i]->map_pos_y};
        coord_2d size = {overworld_maps[i]->width, overworld_maps[i]->height};
        if
        ( check_visibility(pos, size , overworld_struct->camera))
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
                logger->log("map " + std::to_string(i) + " loaded");
                load_map(overworld_maps[i]);
            }
        }

    }
}

void Overworld::draw()
{
    // calc tiles size 
    int sizew = TILE_SIZE * overworld_struct->camera.zoom;
    int sizeh = TILE_SIZE * overworld_struct->camera.zoom; 
    for(int i = 0 ; i < 3 ; i++)
    {
        SDL_Texture* drawed_layer = this->layers[i];
        SDL_SetRenderTarget(this->renderer, NULL);
        SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 0);
        SDL_RenderClear(this->renderer);
        for( int j = 0 ; j < maps.size() ; j++)
        {
            float map_pos_x = ( maps[j]->map_pos_x * TILE_SIZE - overworld_struct->camera.position.x) * overworld_struct->camera.zoom + overworld_struct->camera.size.x / 2;
            float map_pos_y = ( maps[j]->map_pos_y * TILE_SIZE - overworld_struct->camera.position.y) * overworld_struct->camera.zoom + overworld_struct->camera.size.y / 2;
            Tileset* actual_tileset = nullptr;
            for(int k = 0 ; k < tilesets.size() ; k++)
            {
                if(tilesets[k]->get_tileset_uid() == maps[j]->tileset->uid)
                {
                    actual_tileset = tilesets[k];
                }
            }
            for(int k = 0 ; k < 3 ; k++)
            {

                const std::vector<int>* actual_layer = nullptr;
                switch (k)
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
                            float posx = map_pos_x + x * sizew;
                            float posy = map_pos_y + y * sizeh;
                            //float posx = ((maps[j]->map_pos_x + x) * TILE_SIZE - overworld_struct->camera.position.x) * overworld_struct->camera.zoom + overworld_struct->camera.size.x / 2;
                            //float posy = ((maps[j]->map_pos_y + y) * TILE_SIZE - overworld_struct->camera.position.y) * overworld_struct->camera.zoom + overworld_struct->camera.size.y / 2;
                            if(x == 0 )
                            {
                                //logger->log("x : " + std::to_string(x) + " y : " + std::to_string(y));
                                //logger->log("posx : " + std::to_string(posx) + " posy : " + std::to_string(posy));
                                //logger->log("sizeh : " + std::to_string(sizeh) + " sizew : " + std::to_string(sizew));
                            }
                            actual_tileset->draw_tile(tile_index, posx, posy, sizew, sizeh);
                        }
                    }
                }
            }
        }
    }
}
