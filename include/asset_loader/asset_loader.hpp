#pragma once
#include <filesystem>
#include <vector>

#include <fastgltf/core.hpp>
#include <glm/vec3.hpp>
#include <retro_renderer/retro_renderer.hpp>

int loadImageFromFile(const char *filename, char *data, size_t dataSize, int &width, int &height);
int loadHDRImageFromFile(const char *filename, std::vector<float> &outRGBA, int &width, int &height);

enum class MaterialAlphaMode
{
    OPAQUE,
    MASK,
    BLEND
};

struct LoadedPrimitive
{
    std::vector<TexturedVertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<uint8_t> texture_pixels;
    int tex_w = 0;
    int tex_h = 0;
    TextureID texture_id = 0;
    glm::vec3 aabb_min{0.0f};
    glm::vec3 aabb_max{0.0f};
    MaterialAlphaMode alpha_mode = MaterialAlphaMode::OPAQUE;
    float alpha_cutoff = 0.5f;

    std::vector<uint8_t> normal_pixels;
    int normal_w = 0, normal_h = 0;
    std::vector<uint8_t> metallic_roughness_pixels;
    int mr_w = 0, mr_h = 0;
    std::vector<uint8_t> occlusion_pixels;
    int ao_w = 0, ao_h = 0;
    std::vector<uint8_t> emissive_pixels;
    int emissive_w = 0, emissive_h = 0;

    float metallic_factor = 1.0f;
    float roughness_factor = 1.0f;
    glm::vec3 emissive_factor{0.0f};
};

int loadGltfFromFile(const std::filesystem::path &path, std::vector<LoadedPrimitive> &primitives);
