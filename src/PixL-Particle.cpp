#include <modules/PixL-Particle.hpp>

PixL_Particle::PixL_Particle(ParticleData particleData)
{
    if ((PIXL_LOADED_MODULES & PIXL_MODULE_PARTICLE) == 0)
    {
        PIXL_LOADED_MODULES |= PIXL_MODULE_PARTICLE;
        std::cout << "PixL Particle module loaded." << std::endl;
    }

    this->particleData = particleData;

    id = 0;
    bool found = false;
    while (!found)
    {
        id++;
        if (_instances.find(id) == _instances.end())
        {
            found = true; // Found an unused ID
        }
    }
    _instances[id] = this; // Store the instance in the map

    if (particleData.init_func)
    {
        particleData.init_func(particleData.user_data.get());
    }
}

PixL_Particle::~PixL_Particle()
{
    auto it = _instances.find(id);
    if (it != _instances.end() && it->second == this)
    {
        _instances.erase(it);
    }
}

void PixL_Particle::deleteInstance(uint16_t id)
{
    auto it = _instances.find(id);
    if (it != _instances.end())
    {
        PixL_Particle *instance = it->second;
        _instances.erase(it);
        delete instance;
    }
}

void PixL_Particle::deleteAllInstances()
{
    while (!_instances.empty())
    {
        auto it = _instances.begin();
        PixL_Particle *instance = it->second;
        _instances.erase(it);
        delete instance;
    }
}

std::map<uint16_t, PixL_Particle *> PixL_Particle::_instances = {};

uint32_t PixL_Particle::particle_number = 0;
