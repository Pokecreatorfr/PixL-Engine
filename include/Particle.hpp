#pragma once

#include <Graphics.hpp>
#include <Camera.hpp>

enum particle_type
{
    FIRE,
    SMOKE,
};

struct rgb
{
    int r;
    int g;
    int b;
};

struct particle
{
    particle_type type;
    SDL_Texture* texture;
    int width;
    int height;
    rgb color;
    int opacity;
    coord_2d pos;
    int rotation;
    int step;
};

class ParticleEmitter
{
    public:
        ParticleEmitter();
        ~ParticleEmitter();
        void add_particle(particle_type , coord_2d );
        void update();
        void render();
    private:
        vector<particle*> particles;
        SDL_Texture* texture;
        Camera* cam;
};

struct OverworldParticleEmitter
{
    int layer;
    ParticleEmitter* emitter = new ParticleEmitter();
};