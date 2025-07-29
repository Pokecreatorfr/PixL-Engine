#pragma once
#include <cstdint>

static uint64_t PIXL_LOADED_MODULES = 0;

#define PIXL_MODULE_TILEMAP 1 << 0
#define PIXL_MODULE_SPRITE 1 << 1
#define PIXL_MODULE_IMGUI 1 << 2
#define PIXL_MODULE_PARTICLE 1 << 3
