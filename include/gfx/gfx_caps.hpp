#pragma once
#include <cstdint>
#include <gfx/gfx_types.hpp>

namespace pixl::gfx::device
{

    struct Limits
    {
        // Textures
        uint32_t maxTextureDim1D, maxTextureDim2D, maxTextureDim3D;
        uint32_t maxArrayLayers;
        uint32_t maxSamplerAnisotropy;
        // Buffers
        uint64_t maxUniformBufferRange;
        uint64_t maxStorageBufferRange;
        uint32_t maxPushConstantsSize;
        // Compute
        uint32_t maxComputeWorkGroupInvocations;
        uint32_t maxComputeWorkGroupSize[3];
        // Subgroup
        uint32_t subgroupSizeMin, subgroupSizeMax;
        // Draw indirect
        uint32_t maxDrawIndirectCount;
        // Mesh shaders
        uint32_t maxMeshWorkGroupInvocations{0};
        uint32_t maxMeshOutputVertices{0};
        uint32_t maxMeshOutputPrimitives{0};
        // Ray tracing
        uint64_t maxRayRecursionDepth{0};
    };

    struct FormatCaps
    {
        bool sampleable{false};
        bool colorAttachment{false};
        bool depthAttachment{false};
        bool storage{false};
        bool linearFilter{false};
        bool blitSrc{false}, blitDst{false};
        bool msaa[6]{}; // MSAA x1,x2,x4,x8,x16,x32
    };

    struct PresentCaps
    {
        bool supported{false};
        bool mailbox{false}, fifo{true}, immediate{false};
        bool srgbFormats{true};
    };

    struct QueueCaps
    {
        uint32_t graphicsQueues{1};
        uint32_t computeQueues{1};
        uint32_t transferQueues{1};
        bool dedicatedCompute{false};
        bool dedicatedTransfer{false};
    };

}
