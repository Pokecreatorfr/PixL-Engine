#pragma once

#include <string_view>
#include <span>
#include <gfx/gfx_desc.hpp>

namespace pixl::gfx
{
    class ShaderCache
    {
    public:
        static ShaderLayout loadOrCreate(std::span<const uint32_t> spirvWords, std::string_view debugName = {});

    private:
        static ShaderLayout loadFromDisk(uint64_t spirvHash);
        static void writeToDisk(const ShaderLayout &layout);
    };
}

