#pragma once

#include <MapRenderer.hpp>
#include <generated/map2cpp.hpp>
#include <Particle.hpp>


class OverworldRenderer
{
    public:
        OverworldRenderer();
        ~OverworldRenderer();
        void check_maps_visibility();
        void draw();
    private:
        bool check_map_visibility(const map_struct* map);
        void check_tilesets_usage();
        Camera *Camera;
        std::vector<MapRenderer*> map_renderers;
        std::vector<Tileset*> tilesets;
        vector<OverworldParticleEmitter*> particle_emitters;
        Logger* logger;
};