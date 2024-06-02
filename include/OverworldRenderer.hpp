#pragma once

#include <MapRenderer.hpp>

class OverworldRenderer
{
    public:
        OverworldRenderer();
        ~OverworldRenderer();
        void draw();
        void update();
    private:
        bool check_if_map_is_loaded(int uid);
        Tileset* get_tileset(const tileset* ts);
        std::vector<MapRenderer*> maps;
        std::vector<Tileset*> tilesets;
        Camera* camera;
        OpenGLShader* shader;
};