#pragma once

#include <span>
#include <cstdint>
#include <gfx/gfx_desc.hpp>

namespace pixl::gfx
{
    ShaderLayout reflectShaderLayout(std::span<const uint32_t> spirvWords);
}

