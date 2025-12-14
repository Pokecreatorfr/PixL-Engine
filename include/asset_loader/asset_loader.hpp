#pragma once
#include <filesystem>
#include <vector>

#include <fastgltf/core.hpp>
#include <retro_renderer/retro_renderer.hpp>
#include <glm/vec3.hpp>

int loadImageFromFile(const char *filename, char *data, size_t dataSize, int &width, int &height);

struct LoadedPrimitive
{
    std::vector<TexturedVertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<uint8_t> texture_pixels; // RGBA8
    int tex_w = 0;
    int tex_h = 0;
    TextureID texture_id = 0;
    glm::vec3 aabb_min{0.0f};
    glm::vec3 aabb_max{0.0f};
};

int loadGltfFromFile(const std::filesystem::path &path, std::vector<LoadedPrimitive> &primitives);
