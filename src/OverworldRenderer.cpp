#include "OverworldRenderer.hpp"

OverworldRenderer::OverworldRenderer(camera *cam)
{
    this->Camera = cam;
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
                this->map_renderers.push_back(new MapRenderer(this->Camera, overworld_maps[i]));
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
                    break;
                }
            }
        }
    }
}

bool OverworldRenderer::check_map_visibility(const map_struct* map)
{
    return check_visibility(Camera->position.x, Camera->position.y,Camera->size.x * Camera->zoom , Camera->position.y*Camera->zoom, map->map_pos_x, map->map_pos_y, map->width, map->height);
}