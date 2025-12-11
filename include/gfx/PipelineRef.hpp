#pragma once

#include <gfx/GpuTypes.hpp>
#include <memory>

namespace pixl::gfx
{
    struct PipelineRef
    {
        explicit PipelineRef(PipelineHandle p, ShaderHandle vs = InvalidHandle, ShaderHandle fs = InvalidHandle)
            : pipeline(p), vertexShader(vs), fragmentShader(fs) {}

        PipelineHandle pipeline{InvalidHandle};
        ShaderHandle vertexShader{InvalidHandle};
        ShaderHandle fragmentShader{InvalidHandle};

        operator PipelineHandle() const { return pipeline; }
    };

    using PipelinePtr = std::shared_ptr<PipelineRef>;
}
