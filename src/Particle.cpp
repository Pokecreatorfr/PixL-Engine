#include <Particle.hpp>

ParticleEmitter::ParticleEmitter()
{
    this->cam = Camera::GetInstance();
    particles = vector<particle*>();
    SDL_Surface* surface = SDL_CreateRGBSurface(0, 1, 1, 32, 0xFF000000,  0x00FF0000,  0x0000FF00,  0x000000FF);
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 255, 255, 255));
    this->texture1 = SDL_CreateTextureFromSurface(cam->GetRenderer(), surface);
    surface = SDL_CreateRGBSurface(0, 32, 32, 32, 0xFF000000,  0x00FF0000,  0x0000FF00,  0x000000FF);
    SDL_FillRect(surface, NULL, SDL_MapRGBA(surface->format, 0, 0, 0, 0));
    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 32; j++)
        {
            if (sqrt(pow(i - 16, 2) + pow(j - 16, 2)) < 16)
            {
                ((Uint32*)surface->pixels)[i + j * 32] = SDL_MapRGBA(surface->format, 255, 255, 255, 255);
            }
        }
    }
    this->texture2 = SDL_CreateTextureFromSurface(cam->GetRenderer(), surface);
    SDL_FreeSurface(surface);

}


ParticleEmitter::~ParticleEmitter()
{
    for (int i = 0; i < particles.size(); i++)
    {
        delete particles[i];
    }
    particles.clear();
    SDL_DestroyTexture(texture1);
    SDL_DestroyTexture(texture2);
}


void ParticleEmitter::add_particle(particle_type type , coord_2d pos , int number , coord_2d size)
{
    Logger::GetInstance()->log("particle added");
    for (int i = 0 ; i < number ; i++)
    {
        if (type == FIRE)
        {
            particle* part = new particle();
            part->type = FIRE;
            part->texture = this->texture1;
            part->width = size.x;
            part->height = size.y;
            part->color.r = 255;
            part->color.g = 255;
            part->color.b = 255;
            part->opacity = 100;
            part->pos = {pos.x + (rand() % 11 - 5) , pos.y + (rand() % 11 - 5), };
            part->rotation = rand() % 360;
            part->step = 0;
            particles.push_back(part);
        }
        else if (type == SMOKE)
        {
            particle* part = new particle();
            part->type = SMOKE;
            part->texture = this->texture2;
            part->width = size.x;
            part->height = size.y;
            part->color.r = 0;
            part->color.g = 0;
            part->color.b = 0;
            part->opacity = 200;
            part->pos = {pos.x + (rand() % 11 - 5) , pos.y + (rand() % 11 - 5), };
            part->rotation = rand() % 360;
            part->step = 0;
            particles.push_back(part);
        }
    }
}


void ParticleEmitter::update()
{
    for (int i = 0; i < particles.size(); i++)
    {
        if (particles[i]->type == FIRE)
        {
            particles[i]->step++;
            particles[i]->pos.y -= 2;
            particles[i]->rotation += (rand() % 20);
            if (particles[i]->step % 8 == 0)
            {
                particles[i]->width +=  1;
                particles[i]->height += 1 ;
                particles[i]->pos.x += (rand() % 3 - 1);
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
                particles[i]->pos.x += (rand() % 3 - 1);
            }
            else if (particles[i]->step < 80)
            {
                if (particles[i]->step == 41)
                {
                    particles[i]->texture = texture2;
                }
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
                particles[i]->pos.x += (rand() % 5 - 2);
                particles[i]->height++;
                particles[i]->width++;
            }
            // if step is 60 or more, delete particle
            else
            {
                delete particles[i];
                particles.erase(particles.begin() + i);
            }
        }
        else if(particles[i]->type == SMOKE)
        {
            particles[i]->step++;
            particles[i]->pos.y -= 2;
            particles[i]->rotation += (rand() % 20);
            if (particles[i]->step % 8 == 0)
            {
                particles[i]->width +=  1;
                particles[i]->height += 1 ;
                particles[i]->pos.x += (rand() % 3 - 1);
            }
            if (particles[i]->rotation > 360)
            {
                particles[i]->rotation = particles[i]->rotation - 360;
            }
            if (particles[i]->step < 20)
            {
                particles[i]->color.r = 0;
                particles[i]->color.g = 0;
                particles[i]->color.b = 0;
            }
            else if (particles[i]->step < 60)
            {
                particles[i]->color.r = (particles[i]->step - 20) * 100 / 40;
                particles[i]->color.g = (particles[i]->step - 20) * 100 / 40;
                particles[i]->color.b = (particles[i]->step - 20) * 100 / 40;
                particles[i]->opacity = 255 - (particles[i]->step - 20) * 255 / 40;
            }
            else
            {
                delete particles[i];
                particles.erase(particles.begin() + i);
            }
        }
    }
    //Logger::GetInstance()->log("particles size: " + to_string(particles.size()));
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

OverworldParticleEmitter::OverworldParticleEmitter()
{
    this->cam = Camera::GetInstance();
    particles = vector<particle*>();
    SDL_Surface* surface = SDL_CreateRGBSurface(0, 1, 1, 32, 0xFF000000,  0x00FF0000,  0x0000FF00,  0x000000FF);
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 255, 255, 255));
    this->texture1 = SDL_CreateTextureFromSurface(cam->GetRenderer(), surface);
    surface = SDL_CreateRGBSurface(0, 32, 32, 32, 0xFF000000,  0x00FF0000,  0x0000FF00,  0x000000FF);
    SDL_FillRect(surface, NULL, SDL_MapRGBA(surface->format, 0, 0, 0, 0));
    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 32; j++)
        {
            if (sqrt(pow(i - 16, 2) + pow(j - 16, 2)) < 16)
            {
                ((Uint32*)surface->pixels)[i + j * 32] = SDL_MapRGBA(surface->format, 255, 255, 255, 255);
            }
        }
    }
    this->texture2 = SDL_CreateTextureFromSurface(cam->GetRenderer(), surface);
    SDL_FreeSurface(surface);
}

OverworldParticleEmitter::~OverworldParticleEmitter()
{
    for (int i = 0; i < particles.size(); i++)
    {
        delete particles[i];
    }
    particles.clear();
    SDL_DestroyTexture(texture1);
    SDL_DestroyTexture(texture2);
}

void OverworldParticleEmitter::render()
{
    int x = cam->GetPosition()->x;
    int y = cam->GetPosition()->y;
    for (int i = particles.size()-1; i >= 0; i--)
    {
        //Logger::GetInstance()->log("pososition de la particule:" + to_string(particles[i]->pos.x) + " " + to_string(particles[i]->pos.y));
        SDL_Rect dest;
        dest.x = static_cast<int>((particles[i]->pos.x - x) * *cam->GetZoom() + cam->GetSize()->x / 2);
        dest.y = static_cast<int>((particles[i]->pos.y - y) * *cam->GetZoom() + cam->GetSize()->y / 2);
        dest.w = static_cast<int>(particles[i]->width * *cam->GetZoom());
        dest.h = static_cast<int>(particles[i]->height * *cam->GetZoom());
        SDL_SetTextureColorMod(particles[i]->texture, particles[i]->color.r, particles[i]->color.g, particles[i]->color.b);
        SDL_SetTextureAlphaMod(particles[i]->texture, particles[i]->opacity);
        SDL_RenderCopyEx(cam->GetRenderer(), particles[i]->texture, NULL, &dest, particles[i]->rotation, NULL, SDL_FLIP_NONE);
    }
    //Logger::GetInstance()->log("particles size: " + to_string(particles.size()));
}

void OverworldParticleEmitter::add_particle(particle_type type , coord_2d pos , int number , coord_2d size)
{
    Logger::GetInstance()->log("particle added");
    for (int i = 0 ; i < number ; i++)
    {
        if (type == FIRE)
        {
            particle* part = new particle();
            part->type = FIRE;
            part->texture = this->texture1;
            part->width = size.x;
            part->height = size.y;
            part->color.r = 255;
            part->color.g = 255;
            part->color.b = 255;
            part->opacity = 100;
            part->pos = {pos.x + (rand() % 11 - 5) , pos.y + (rand() % 11 - 5), };
            part->rotation = rand() % 360;
            part->step = 0;
            particles.push_back(part);
        }
        else if (type == SMOKE)
        {
            particle* part = new particle();
            part->type = SMOKE;
            part->texture = this->texture2;
            part->width = size.x;
            part->height = size.y;
            part->color.r = 0;
            part->color.g = 0;
            part->color.b = 0;
            part->opacity = 200;
            part->pos = {pos.x + (rand() % 11 - 5) , pos.y + (rand() % 11 - 5), };
            part->rotation = rand() % 360;
            part->step = 0;
            particles.push_back(part);
        }
    }
}

void OverworldParticleEmitter::update()
{
    for (int i = 0; i < particles.size(); i++)
    {
        if (particles[i]->type == FIRE)
        {
            particles[i]->step++;
            particles[i]->pos.y -= 2;
            particles[i]->rotation += (rand() % 20);
            if (particles[i]->step % 8 == 0)
            {
                particles[i]->width +=  1;
                particles[i]->height += 1 ;
                particles[i]->pos.x += (rand() % 3 - 1);
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
                particles[i]->pos.x += (rand() % 3 - 1);
            }
            else if (particles[i]->step < 80)
            {
                if (particles[i]->step == 41)
                {
                    particles[i]->texture = texture2;
                }
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
                particles[i]->pos.x += (rand() % 5 - 2);
                particles[i]->height++;
                particles[i]->width++;
            }
            // if step is 60 or more, delete particle
            else
            {
                delete particles[i];
                particles.erase(particles.begin() + i);
            }
        }
        else if(particles[i]->type == SMOKE)
        {
            particles[i]->step++;
            particles[i]->pos.y -= 2;
            particles[i]->rotation += (rand() % 20);
            if (particles[i]->step % 8 == 0)
            {
                particles[i]->width +=  1;
                particles[i]->height += 1 ;
                particles[i]->pos.x += (rand() % 3 - 1);
            }
            if (particles[i]->rotation > 360)
            {
                particles[i]->rotation = particles[i]->rotation - 360;
            }
            if (particles[i]->step < 20)
            {
                particles[i]->color.r = 0;
                particles[i]->color.g = 0;
                particles[i]->color.b = 0;
            }
            else if (particles[i]->step < 60)
            {
                particles[i]->color.r = (particles[i]->step - 20) * 100 / 40;
                particles[i]->color.g = (particles[i]->step - 20) * 100 / 40;
                particles[i]->color.b = (particles[i]->step - 20) * 100 / 40;
                particles[i]->opacity = 255 - (particles[i]->step - 20) * 255 / 40;
            }
            else
            {
                delete particles[i];
                particles.erase(particles.begin() + i);
            }
        }
    }
    //Logger::GetInstance()->log("particles size: " + to_string(particles.size()));
}

MovingParticleEmitter::MovingParticleEmitter(int* x, int* y, bool affected_by_zoom)
{
    this->cam = Camera::GetInstance();
    particles = vector<particle*>();
    SDL_Surface* surface = SDL_CreateRGBSurface(0, 1, 1, 32, 0xFF000000,  0x00FF0000,  0x0000FF00,  0x000000FF);
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 255, 255, 255));
    this->texture1 = SDL_CreateTextureFromSurface(cam->GetRenderer(), surface);
    surface = SDL_CreateRGBSurface(0, 32, 32, 32, 0xFF000000,  0x00FF0000,  0x0000FF00,  0x000000FF);
    SDL_FillRect(surface, NULL, SDL_MapRGBA(surface->format, 0, 0, 0, 0));
    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 32; j++)
        {
            if (sqrt(pow(i - 16, 2) + pow(j - 16, 2)) < 16)
            {
                ((Uint32*)surface->pixels)[i + j * 32] = SDL_MapRGBA(surface->format, 255, 255, 255, 255);
            }
        }
    }
    this->texture2 = SDL_CreateTextureFromSurface(cam->GetRenderer(), surface);
    SDL_FreeSurface(surface);
    this->posx = x;
    this->posy = y;
    this->affected_by_zoom = affected_by_zoom;
}

MovingParticleEmitter::~MovingParticleEmitter()
{
    for (int i = 0; i < particles.size(); i++)
    {
        delete particles[i];
    }
    particles.clear();
    SDL_DestroyTexture(texture1);
    SDL_DestroyTexture(texture2);
}

void MovingParticleEmitter::add_particle(particle_type type, coord_2d pos, int number, coord_2d size)
{
    Logger::GetInstance()->log("particle added");
    for (int i = 0 ; i < number ; i++)
    {
        if (type == FIRE)
        {
            particle* part = new particle();
            part->type = FIRE;
            part->texture = this->texture1;
            part->width = size.x;
            part->height = size.y;
            part->color.r = 255;
            part->color.g = 255;
            part->color.b = 255;
            part->opacity = 100;
            part->pos = {pos.x + (rand() % 11 - 5) , pos.y + (rand() % 11 - 5), };
            part->rotation = rand() % 360;
            part->step = 0;
            particles.push_back(part);
        }
        else if (type == SMOKE)
        {
            particle* part = new particle();
            part->type = SMOKE;
            part->texture = this->texture2;
            part->width = size.x;
            part->height = size.y;
            part->color.r = 0;
            part->color.g = 0;
            part->color.b = 0;
            part->opacity = 200;
            part->pos = {pos.x + (rand() % 11 - 5) , pos.y + (rand() % 11 - 5), };
            part->rotation = rand() % 360;
            part->step = 0;
            particles.push_back(part);
        }
    }
}

void MovingParticleEmitter::render()
{
    for (int i = particles.size()-1; i >= 0; i--)
    {
        //Logger::GetInstance()->log("pososition de la particule:" + to_string(particles[i]->pos.x) + " " + to_string(particles[i]->pos.y));
        SDL_Rect dest;
        if (this->affected_by_zoom)
        {
            
            dest.x = static_cast<int>(((particles[i]->pos.x ) * *cam->GetZoom())+ *this->posx);
            dest.y = static_cast<int>(((particles[i]->pos.y ) * *cam->GetZoom()) + *this->posy);
            dest.w = static_cast<int>(particles[i]->width * *cam->GetZoom());
            dest.h = static_cast<int>(particles[i]->height * *cam->GetZoom());
        }
        else
        {
            dest = {particles[i]->pos.x + *this->posx  , particles[i]->pos.y + *this->posy, particles[i]->width, particles[i]->height};
        }
        SDL_SetTextureColorMod(particles[i]->texture, particles[i]->color.r, particles[i]->color.g, particles[i]->color.b);
        SDL_SetTextureAlphaMod(particles[i]->texture, particles[i]->opacity);
        SDL_RenderCopyEx(cam->GetRenderer(), particles[i]->texture, NULL, &dest, particles[i]->rotation, NULL, SDL_FLIP_NONE);
    }
    //Logger::GetInstance()->log("particles size: " + to_string(particles.size()));
}

void MovingParticleEmitter::update()
{
        for (int i = 0; i < particles.size(); i++)
    {
        if (particles[i]->type == FIRE)
        {
            particles[i]->step++;
            particles[i]->pos.y -= 2;
            particles[i]->rotation += (rand() % 20);
            if (particles[i]->step % 8 == 0)
            {
                particles[i]->width +=  1;
                particles[i]->height += 1 ;
                particles[i]->pos.x += (rand() % 3 - 1);
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
                particles[i]->pos.x += (rand() % 3 - 1);
            }
            else if (particles[i]->step < 80)
            {
                if (particles[i]->step == 41)
                {
                    particles[i]->texture = texture2;
                }
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
                particles[i]->pos.x += (rand() % 5 - 2);
                particles[i]->height++;
                particles[i]->width++;
            }
            // if step is 60 or more, delete particle
            else
            {
                delete particles[i];
                particles.erase(particles.begin() + i);
            }
        }
        else if(particles[i]->type == SMOKE)
        {
            particles[i]->step++;
            particles[i]->pos.y -= 2;
            particles[i]->rotation += (rand() % 20);
            if (particles[i]->step % 8 == 0)
            {
                particles[i]->width +=  1;
                particles[i]->height += 1 ;
                particles[i]->pos.x += (rand() % 3 - 1);
            }
            if (particles[i]->rotation > 360)
            {
                particles[i]->rotation = particles[i]->rotation - 360;
            }
            if (particles[i]->step < 20)
            {
                particles[i]->color.r = 0;
                particles[i]->color.g = 0;
                particles[i]->color.b = 0;
            }
            else if (particles[i]->step < 60)
            {
                particles[i]->color.r = (particles[i]->step - 20) * 100 / 40;
                particles[i]->color.g = (particles[i]->step - 20) * 100 / 40;
                particles[i]->color.b = (particles[i]->step - 20) * 100 / 40;
                particles[i]->opacity = 255 - (particles[i]->step - 20) * 255 / 40;
            }
            else
            {
                delete particles[i];
                particles.erase(particles.begin() + i);
            }
        }
    }
    //Logger::GetInstance()->log("particles size: " + to_string(particles.size()));
}

void MovingParticleEmitter::set_affected_by_zoom(bool affected_by_zoom)
{
    this->affected_by_zoom = affected_by_zoom;
}
