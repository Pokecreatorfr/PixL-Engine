#pragma once
#include <gfx/gfx_types.hpp>

namespace pixl::gfx::device
{
    enum class EXTENSION_SUPPORT : uint32_t
    {
        MESH_SHADER = 1 << 0,
        RAY_TRACING = 1 << 1,
        VARIABLE_RATE_SHADING = 1 << 2,
        SAMPLER_FEEDBACK = 1 << 3
    };

    enum class API : uint8_t
    {
        VULKAN,
        DIRECTX12,
        METAL,
        OPENGL,
        OPENGL_ES
    };

    enum class DeviceType : uint8_t
    {
        UNKNOWN,
        INTEGRATED_GPU,
        DISCRETE_GPU,
        VIRTUAL_GPU,
        CPU
    };

    struct DeviceInfo
    {
        API api;
        DeviceType type;
        uint32_t vendor_id;
        uint32_t device_id;
        char name[256];
        uint64_t total_memory; // bytes
        uint32_t supported_extensions;
    };

}