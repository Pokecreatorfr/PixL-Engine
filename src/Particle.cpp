#include <Particle.hpp>

ParticleEmitter::ParticleEmitter()
{
    this->cam = Camera::GetInstance();
    particles = vector<particle*>();
    SDL_Surface* surface = SDL_CreateRGBSurface(0, 2, 2, 32, 0xFF000000,  0x00FF0000,  0x0000FF00,  0x000000FF);
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 255, 255, 255));
    this->texture = SDL_CreateTextureFromSurface(cam->GetRenderer(), surface);
    SDL_FreeSurface(surface);
}


ParticleEmitter::~ParticleEmitter()
{
    for (int i = 0; i < particles.size(); i++)
    {
        delete particles[i];
    }
    particles.clear();
    SDL_DestroyTexture(texture);
}


void ParticleEmitter::add_particle(particle_type type , coord_2d pos)
{
    switch (type)
    {
    case FIRE:
        particle* part = new particle();
        part->type = FIRE;
        part->texture = this->texture;
        part->width = 15;
        part->height = 15;
        part->color.r = 255;
        part->color.g = 255;
        part->color.b = 255;
        part->opacity = 255;
        part->pos = {pos.x + (rand() % 11 - 5) , pos.y + (rand() % 11 - 5), };
        part->step = 0;
        particles.push_back(part);
        break;
    }
}


void ParticleEmitter::update()
{
    for (int i = 0; i < particles.size(); i++)
    {
        particles[i]->step++;
        particles[i]->pos.y -= 1 + (rand() % 2);
        particles[i]->pos.x += (rand() % 3 - 1);
        particles[i]->rotation += (rand() % 20);
        if (particles[i]->step % 4 == 0)
        {
            particles[i]->width += rand() % 2 ;
            particles[i]->height += rand() % 2 ;
        }
        if (particles[i]->rotation > 360)
        {
            particles[i]->rotation = particles[i]->rotation - 360;
        }
        if (particles[i]->step < 20)
        {
            particles[i]->color.r = 255;
            particles[i]->color.g = 255;
            particles[i]->color.b = 255 - particles[i]->step * 255 / 20;
        }
        else if (particles[i]->step < 40)
        {
            particles[i]->color.r = 255;
            particles[i]->color.g = 255 - (particles[i]->step - 20) * 255 / 20;
            particles[i]->color.b = 0;
            particles[i]->pos.x += (rand() % 7 - 3);
        }
        else if (particles[i]->step < 80)
        {
            particles[i]->color.r = 255 - (particles[i]->step - 40) * 255 / 40;
            particles[i]->color.g = 0;
            particles[i]->color.b = 0;
            particles[i]->opacity = 255 - (particles[i]->step - 40) * 255 / 60;
        }
        else if (particles[i]->step < 120)
        {
            particles[i]->color.r = (particles[i]->step - 80) * 100 / 40;
            particles[i]->color.g = (particles[i]->step - 80) * 100 / 40;
            particles[i]->color.b = (particles[i]->step - 80) * 100 / 40;
            particles[i]->opacity = 255 - (particles[i]->step - 80) * 255 / 40;
            particles[i]->pos.y -= 1;
        }
        // if step is 60 or more, delete particle
        else
        {
            delete particles[i];
            particles.erase(particles.begin() + i);
        }
    }
    Logger::GetInstance()->log("particles size: " + to_string(particles.size()));
}


void ParticleEmitter::render()
{
    for (int i = 0; i < particles.size(); i++)
    {
        SDL_Rect dest = {particles[i]->pos.x, particles[i]->pos.y, particles[i]->width, particles[i]->height};
        SDL_SetTextureColorMod(particles[i]->texture, particles[i]->color.r, particles[i]->color.g, particles[i]->color.b);
        SDL_SetTextureAlphaMod(particles[i]->texture, particles[i]->opacity);
        SDL_RenderCopyEx(cam->GetRenderer(), particles[i]->texture, NULL, &dest, particles[i]->rotation, NULL, SDL_FLIP_NONE);
    }
}