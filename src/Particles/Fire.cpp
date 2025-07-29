#include <Particles/Fire.hpp>

static bool fire_particle_system_initialized = false;

void Fire_Particle_Init(void *user_data)
{
    if (!fire_particle_system_initialized)
    {
        fire_particle_system_initialized = true;
        auto *data = static_cast<FireParticleSystemData *>(user_data);
        data->particles.clear();
        data->particles.reserve(32768); // Réserve de particules

        PixL_CreateSSBO("FireParticlesSSBO", sizeof(FireParticleData) * 32768);
        PixL_CreatePipeline("FireParticle", &fire_particles_vertex, &fire_particles_fragment,
                            false, SDL_GPU_COMPAREOP_LESS, false, false);
    }
}

void Fire_Particle_Update(void *user_data)
{
    auto *data = static_cast<FireParticleSystemData *>(user_data);

    // Ajouter 5 particules par frame
    for (int i = 0; i < 40; ++i)
    {
        FireParticleData new_particle;
        new_particle.position = data->emiter_position;
        new_particle.step = 0;
        data->particles.push_back(new_particle);
    }

// Mise à jour parallèle
#pragma omp parallel for
    for (int i = 0; i < static_cast<int>(data->particles.size()); ++i)
    {
        FireParticleData &particle = data->particles[i];
        particle.step++;

        thread_local std::mt19937 rng(std::random_device{}());
        std::uniform_real_distribution<float> drift50(-0.005f, 0.005f);
        std::uniform_real_distribution<float> drift250(-0.0015f, 0.0015f);
        std::uniform_real_distribution<float> drift550(-0.0025f, 0.0025f);

        if (particle.step < 50)
        {
            particle.position.y += 0.005f;
            particle.position.x += drift50(rng);
        }
        else if (particle.step < 120)
        {
            particle.position.y += 0.004f;
            particle.position.x += drift250(rng);
        }
        else if (particle.step < 250)
        {
            particle.position.y += 0.003f;
            particle.position.x += drift550(rng);
        }
    }

    // Supprimer les particules expirées
    data->particles.erase(
        std::remove_if(data->particles.begin(), data->particles.end(),
                       [](const FireParticleData &p)
                       {
                           return p.step >= 250;
                       }),
        data->particles.end());

    // Trier (optionnel)
    std::sort(data->particles.begin(), data->particles.end(),
              [](const FireParticleData &a, const FireParticleData &b)
              {
                  return a.step > b.step;
              });

    // Mettre à jour le SSBO
    PixL_UpdateSSBO("FireParticlesSSBO", data->particles.data(), sizeof(FireParticleData) * data->particles.size());

    // Mettre à jour le nombre de particules
    PixL_Particle::UpdateParticlesNumber(data->particles.size());
}

void Fire_Particle_Draw(void *user_data)
{
    PixL_2D_AddDrawable(
        1, // Layer ID
        8, // Z-index
        "FireParticle",
        [](void *data)
        {
            auto *particle_data = static_cast<FireParticleSystemData *>(data);
            PixL_Draw("FireParticle", "", "", particle_data->particles.size(), 6, nullptr, {},
                      {"FireParticlesSSBO"},
                      {}, {}, nullptr);
        },
        user_data);
}
