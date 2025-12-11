#pragma once
#include <cstddef>
#include <gfx/GpuTypes.hpp>

namespace pixl::gfx
{
    struct BufferDesc
    {
        size_t size = 0;
        BufferUsage usage = BufferUsage::Vertex;
        uint32_t stride = 0;
        bool cpuWritable = true;
        bool dynamic = false;
    };
}
