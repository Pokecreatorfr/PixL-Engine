#pragma once
#include <cstdint>

namespace pixl::gfx
{
    struct RenderPassDesc
    {
        bool clearColor = true;
        float clearColorValue[4] = {0.0f, 0.0f, 0.0f, 1.0f};
        bool useDepth = false;
        bool clearDepth = false;
        float clearDepthValue = 1.0f;
        bool clearStencil = false;
        uint8_t clearStencilValue = 0;
        bool useGBuffer = false;
        bool lightingBlit = false; // if true and useGBuffer, blit albedo to backbuffer after geometry pass
    };
}
