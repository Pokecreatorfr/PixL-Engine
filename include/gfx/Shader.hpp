#pragma once
#include <gfx/GpuTypes.hpp>
#include <string>
#include <vector>

namespace pixl::gfx
{
    struct ShaderStageSource
    {
        ShaderStage stage = ShaderStage::Vertex;
        ShaderLanguage language = ShaderLanguage::SPV;
        std::string entryPoint = "main";
        std::string source;
    };

    struct ShaderDesc
    {
        std::vector<ShaderStageSource> stages;
    };
}
