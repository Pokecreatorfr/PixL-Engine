#include <OverworldRenderer.hpp>


OverworldRenderer::OverworldRenderer()
{
    this->Camera = Camera::GetInstance();
    this->logger = Logger::GetInstance();
}

OverworldRenderer::~OverworldRenderer()
{
    for (auto &mr : this->map_renderers)
    {
        delete mr;
    }
}

void OverworldRenderer::check_maps_visibility()
{
    for (int i = 0; i < overworld_maps.size(); i++)
    {
        if (check_map_visibility(overworld_maps[i]))
        {
            bool present = false;
            for(int j = 0; j < this->map_renderers.size(); j++)
            {
                if(this->map_renderers[j]->get_uid() == overworld_maps[i]->uid)
                {
                    present = true;
                    break;
                }
            }
            if(!present)
            {
                bool tileset_present = false;
                std::vector<const tileset*> tilesets_vector;
                tilesets_vector.push_back(overworld_maps[i]->tile_layer_0.tileset);
                tilesets_vector.push_back(overworld_maps[i]->tile_layer_1.tileset);
                tilesets_vector.push_back(overworld_maps[i]->tile_layer_2.tileset);

                for (int k = 0; k < tilesets_vector.size(); k++)
                {
                    for (int l = 0; l < this->tilesets.size(); l++)
                    {
                        if (tilesets_vector[k]->uid == this->tilesets[l]->get_uid())
                        {
                            tileset_present = true;
                            break;
                        }
                    }
                    if (!tileset_present)
                    {
                        this->tilesets.push_back(new Tileset(tilesets_vector[k]));
                        this->logger->log("Tileset " + std::to_string(tilesets_vector[k]->uid) + " loaded");
                    }
                }

                std::vector<Tileset*> tilesets_vector2;
                for (int k = 0; k < tilesets_vector.size(); k++)
                {
                    for (int l = 0; l < this->tilesets.size(); l++)
                    {
                        if (tilesets_vector[k]->uid == this->tilesets[l]->get_uid())
                        {
                            tilesets_vector2.push_back(this->tilesets[l]);
                            break;
                        }
                    }
                }



                this->map_renderers.push_back(new MapRenderer(overworld_maps[i] , tilesets_vector2[0], tilesets_vector2[1], tilesets_vector2[2]));
                this->logger->log("Map " + std::to_string(overworld_maps[i]->uid) + " loaded");
            }
        }
        else
        {
            for(int j = 0; j < this->map_renderers.size(); j++)
            {
                if(this->map_renderers[j]->get_uid() == overworld_maps[i]->uid)
                {
                    delete this->map_renderers[j];
                    this->map_renderers.erase(this->map_renderers.begin() + j);
                    this->logger->log("Map " + std::to_string(overworld_maps[i]->uid) + " unloaded");
                    break;
                }
            }
        }
    }
    this->check_tilesets_usage();
}

void OverworldRenderer::check_tilesets_usage()
{
    for (int i = 0; i < this->tilesets.size(); i++)
    {
        bool present = false;
        for (int j = 0; j < this->map_renderers.size(); j++)
        {
            std::vector<int> tilesets_uid = this->map_renderers[j]->get_tilesets_uid();
            for (int k = 0; k < tilesets_uid.size(); k++)
            {
                if (tilesets_uid[k] == this->tilesets[i]->get_uid())
                {
                    present = true;
                    break;
                }
            }
        }
        if (!present)
        {
            this->logger->log("Tileset " + std::to_string(this->tilesets[i]->get_uid()) + " unloaded");
            delete this->tilesets[i];
            this->tilesets.erase(this->tilesets.begin() + i);
        }
    }
}

bool OverworldRenderer::check_map_visibility(const map_struct* map)
{
    coord_2d map_position = {map->map_pos_x, map->map_pos_y};
    coord_2d map_size = {map->width, map->height};
    return check_tile_visibility(map_position, map_size, this->Camera);
}

void OverworldRenderer::draw()
{
    for(int i = 0; i < this->map_renderers.size(); i++)
    {
        this->map_renderers[i]->draw_layer0();
        this->map_renderers[i]->draw_layer1();
        this->map_renderers[i]->draw_layer2();
    }
}