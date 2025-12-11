#pragma once

#include <gfx/GpuTypes.hpp>
#include <memory>

namespace pixl::gfx
{
    struct TextureRef
    {
        explicit TextureRef(TextureHandle h) : handle(h) {}
        TextureHandle handle{InvalidHandle};
        operator TextureHandle() const { return handle; }
    };

    using TexturePtr = std::shared_ptr<TextureRef>;
}
