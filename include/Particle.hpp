#pragma once

#include <Graphics.hpp>
#include <Camera.hpp>
#include <math.h>

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
        void add_particle(particle_type type ,coord_2d pos ,int number ,coord_2d size );
        void update();
        void render();
    private:
        vector<particle*> particles;
        SDL_Texture* texture1;
        SDL_Texture* texture2;
        Camera* cam;
};

class OverworldParticleEmitter
{
    public:
        OverworldParticleEmitter();
        ~OverworldParticleEmitter();
        void add_particle(particle_type type ,coord_2d pos ,int number ,coord_2d size );
        void render();
        void update();
        int get_layer();
    private:
        vector<particle*> particles;
        SDL_Texture* texture1;
        SDL_Texture* texture2;
        Camera* cam;
};

class MovingParticleEmitter
{
    public:
        MovingParticleEmitter(int* x ,int* y, bool affected_by_zoom);
        ~MovingParticleEmitter();
        void add_particle(particle_type type ,coord_2d pos ,int number ,coord_2d size );
        void render();
        void update();
        void set_affected_by_zoom(bool affected_by_zoom);
    private:
        vector<particle*> particles;
        SDL_Texture* texture1;
        SDL_Texture* texture2;
        Camera* cam;
        int* posx;
        int* posy;
        bool affected_by_zoom;
};