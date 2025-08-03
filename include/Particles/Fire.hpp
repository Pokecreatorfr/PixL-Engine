#pragma once

#include <PixL_2D.hpp>
#include <algorithm>
#include <const/generated_shaders.hpp>
#include <glm/glm.hpp>
#include <modules/PixL-Particle.hpp>
#include <random>
#include <vector>

struct FireParticleData
{
    glm::vec2 position; // Position of the fire particle
    uint32_t step;      // Step count for the particle
    float rotation;     // Rotation of the particle
};

struct FireParticleSystemData
{
    glm::vec2 emiter_position;
    std::vector<FireParticleData> particles;
};

void Fire_Particle_Init(void *user_data);

void Fire_Particle_Update(void *user_data);

void Fire_Particle_Draw(void *user_data);
