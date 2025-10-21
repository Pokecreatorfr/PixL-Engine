#pragma once

#include <PixL_2D.hpp>
#include <functional>
#include <map>
#include <memory>
#include <modules/Modules-config.hpp>

struct ParticleData
{
    std::function<void(void *)> init_func = nullptr;   // Function to initialize the particle
    std::function<void(void *)> update_func = nullptr; // Function to update the particle
    std::function<void(void *)> draw_func = nullptr;   // Function to draw the particle
    std::shared_ptr<void> user_data = nullptr;         // User data for the particle
};

class PixL_Particle
{
public:
    virtual ~PixL_Particle();
    static PixL_Particle *getInstance(uint16_t id)
    {
        if (_instances.find(id) == _instances.end())
        {
            return nullptr; // Return nullptr if the instance does not exist
        }
        return _instances[id];
    }

    static PixL_Particle *getInstance(ParticleData particleData)
    {
        return new PixL_Particle(particleData);
    }

    static void deleteInstance(uint16_t id);

    static void deleteAllInstances();

    void update()
    {
        particle_number = 0;
        if (particleData.update_func)
        {
            particleData.update_func(particleData.user_data.get());
        }
    }

    void draw()
    {
        if (particleData.draw_func)
        {
            particleData.draw_func(particleData.user_data.get());
        }
    }

    static void UpdateParticlesNumber(uint32_t nb)
    {
        particle_number += nb;
    }

    static uint32_t GetParticlesNumber()
    {
        return particle_number;
    }

protected:
    explicit PixL_Particle(ParticleData particleData);

private:
    static std::map<uint16_t, PixL_Particle *> _instances;
    static uint32_t particle_number;
    uint16_t id;
    ParticleData particleData;
};
