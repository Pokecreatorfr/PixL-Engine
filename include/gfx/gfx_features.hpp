#pragma once
#include <cstdint>

namespace pixl::gfx::device
{

    enum class MeshShaderSupport : uint8_t
    {
        NONE,
        MESH_ONLY,
        TASK_AND_MESH
    };
    enum class RayTracingSupport : uint8_t
    {
        NONE,
        RAY_QUERY,
        RT_PIPELINE
    };
    enum class VRSTier : uint8_t
    {
        NONE,
        TIER1,
        TIER2
    };

    struct Features
    {
        bool descriptorIndexing{false};
        bool dynamicRendering{false};
        bool timelineSemaphore{false};
        bool bufferDeviceAddress{false};
        bool shaderInt64{false};
        bool shaderFloat16{false};
        bool subgroupBasic{false};
        bool subgroupVote{false};
        bool subgroupBallot{false};
        bool subgroupShuffle{false};

        MeshShaderSupport mesh{MeshShaderSupport::NONE};
        RayTracingSupport rayTracing{RayTracingSupport::NONE};
        VRSTier vrs{VRSTier::NONE};
        bool conservativeRaster{false};
        bool fragmentShaderInterlock{false};

        bool samplerAnisotropy{true};
        bool samplerFilterCubic{false};
        bool sparseBinding{false};
        bool residency2{false};
    };

}
