#pragma once
#include <cstdint>

inline uint64_t PIXL_LOADED_MODULES = 0;

#define PIXL_MODULE_TILEMAP (1u << 0)
#define PIXL_MODULE_SPRITE (1u << 1)
#define PIXL_MODULE_IMGUI (1u << 2)
#define PIXL_MODULE_PARTICLE (1u << 3)
