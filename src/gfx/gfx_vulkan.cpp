#define VMA_IMPLEMENTATION
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1

#include <gfx/gfx_vulkan.hpp>
#include <gfx/gfx_shader_cache.hpp>

namespace pixl::gfx
{
    namespace
    {
        std::atomic<uint64_t> g_bufferHandleCounter{1};
        std::atomic<uint64_t> g_imageHandleCounter{1};
        std::atomic<uint64_t> g_imageViewHandleCounter{1};
        std::atomic<uint64_t> g_samplerHandleCounter{1};
        std::atomic<uint64_t> g_shaderHandleCounter{1};
        std::atomic<uint64_t> g_pipelineLayoutHandleCounter{1};
        std::atomic<uint64_t> g_pipelineHandleCounter{1};
        std::atomic<uint64_t> g_descriptorSetLayoutHandleCounter{1};
        std::atomic<uint64_t> g_descriptorSetHandleCounter{1};

        constexpr uint32_t kDefaultDescriptorPoolMaxSets = 512;
        constexpr uint32_t kDefaultBindlessDescriptorCount = 4096;

        vk::Format toVkFormat(TextureFormat fmt)
        {
            switch (fmt)
            {
            case TextureFormat::R8_UNORM:
                return vk::Format::eR8Unorm;
            case TextureFormat::R8_SNORM:
                return vk::Format::eR8Snorm;
            case TextureFormat::R8_UINT:
                return vk::Format::eR8Uint;
            case TextureFormat::R8_SINT:
                return vk::Format::eR8Sint;
            case TextureFormat::R8G8_UNORM:
                return vk::Format::eR8G8Unorm;
            case TextureFormat::R8G8_SNORM:
                return vk::Format::eR8G8Snorm;
            case TextureFormat::R8G8_UINT:
                return vk::Format::eR8G8Uint;
            case TextureFormat::R8G8_SINT:
                return vk::Format::eR8G8Sint;
            case TextureFormat::R8G8B8_UNORM:
                return vk::Format::eR8G8B8Unorm;
            case TextureFormat::B8G8R8_UNORM:
                return vk::Format::eB8G8R8Unorm;
            case TextureFormat::R8G8B8A8_UNORM:
                return vk::Format::eR8G8B8A8Unorm;
            case TextureFormat::R8G8B8A8_SNORM:
                return vk::Format::eR8G8B8A8Snorm;
            case TextureFormat::R8G8B8A8_UINT:
                return vk::Format::eR8G8B8A8Uint;
            case TextureFormat::R8G8B8A8_SINT:
                return vk::Format::eR8G8B8A8Sint;
            case TextureFormat::R8G8B8A8_UNORM_SRGB:
                return vk::Format::eR8G8B8A8Srgb;
            case TextureFormat::B8G8R8A8_UNORM:
                return vk::Format::eB8G8R8A8Unorm;
            case TextureFormat::B8G8R8A8_UNORM_SRGB:
                return vk::Format::eB8G8R8A8Srgb;
            case TextureFormat::R10G10B10A2_UNORM:
                return vk::Format::eA2B10G10R10UnormPack32;
            case TextureFormat::R11G11B10_FLOAT:
                return vk::Format::eB10G11R11UfloatPack32;
            case TextureFormat::R16_UNORM:
                return vk::Format::eR16Unorm;
            case TextureFormat::R16_SNORM:
                return vk::Format::eR16Snorm;
            case TextureFormat::R16_UINT:
                return vk::Format::eR16Uint;
            case TextureFormat::R16_SINT:
                return vk::Format::eR16Sint;
            case TextureFormat::R16_FLOAT:
                return vk::Format::eR16Sfloat;
            case TextureFormat::R16G16_UNORM:
                return vk::Format::eR16G16Unorm;
            case TextureFormat::R16G16_SNORM:
                return vk::Format::eR16G16Snorm;
            case TextureFormat::R16G16_UINT:
                return vk::Format::eR16G16Uint;
            case TextureFormat::R16G16_SINT:
                return vk::Format::eR16G16Sint;
            case TextureFormat::R16G16_FLOAT:
                return vk::Format::eR16G16Sfloat;
            case TextureFormat::R16G16B16A16_UNORM:
                return vk::Format::eR16G16B16A16Unorm;
            case TextureFormat::R16G16B16A16_SNORM:
                return vk::Format::eR16G16B16A16Snorm;
            case TextureFormat::R16G16B16A16_UINT:
                return vk::Format::eR16G16B16A16Uint;
            case TextureFormat::R16G16B16A16_SINT:
                return vk::Format::eR16G16B16A16Sint;
            case TextureFormat::R16G16B16A16_FLOAT:
                return vk::Format::eR16G16B16A16Sfloat;
            case TextureFormat::R32_UINT:
                return vk::Format::eR32Uint;
            case TextureFormat::R32_SINT:
                return vk::Format::eR32Sint;
            case TextureFormat::R32_FLOAT:
                return vk::Format::eR32Sfloat;
            case TextureFormat::R32G32_UINT:
                return vk::Format::eR32G32Uint;
            case TextureFormat::R32G32_SINT:
                return vk::Format::eR32G32Sint;
            case TextureFormat::R32G32_FLOAT:
                return vk::Format::eR32G32Sfloat;
            case TextureFormat::R32G32B32_UINT:
                return vk::Format::eR32G32B32Uint;
            case TextureFormat::R32G32B32_SINT:
                return vk::Format::eR32G32B32Sint;
            case TextureFormat::R32G32B32_FLOAT:
                return vk::Format::eR32G32B32Sfloat;
            case TextureFormat::R32G32B32A32_UINT:
                return vk::Format::eR32G32B32A32Uint;
            case TextureFormat::R32G32B32A32_SINT:
                return vk::Format::eR32G32B32A32Sint;
            case TextureFormat::R32G32B32A32_FLOAT:
                return vk::Format::eR32G32B32A32Sfloat;
            case TextureFormat::D16_UNORM:
                return vk::Format::eD16Unorm;
            case TextureFormat::D24_UNORM:
                return vk::Format::eX8D24UnormPack32;
            case TextureFormat::S8_UINT:
                return vk::Format::eS8Uint;
            case TextureFormat::D24_UNORM_S8_UINT:
                return vk::Format::eD24UnormS8Uint;
            case TextureFormat::D32_FLOAT:
                return vk::Format::eD32Sfloat;
            case TextureFormat::D32_FLOAT_S8X24_UINT:
                return vk::Format::eD32SfloatS8Uint;
            case TextureFormat::BC1_UNORM:
                return vk::Format::eBc1RgbaUnormBlock;
            case TextureFormat::BC1_UNORM_SRGB:
                return vk::Format::eBc1RgbaSrgbBlock;
            case TextureFormat::BC3_UNORM:
                return vk::Format::eBc3UnormBlock;
            case TextureFormat::BC3_UNORM_SRGB:
                return vk::Format::eBc3SrgbBlock;
            case TextureFormat::BC4_UNORM:
                return vk::Format::eBc4UnormBlock;
            case TextureFormat::BC5_UNORM:
                return vk::Format::eBc5UnormBlock;
            case TextureFormat::BC6H_UFLOAT:
                return vk::Format::eBc6HUfloatBlock;
            case TextureFormat::BC6H_SFLOAT:
                return vk::Format::eBc6HSfloatBlock;
            case TextureFormat::BC7_UNORM:
                return vk::Format::eBc7UnormBlock;
            case TextureFormat::BC7_UNORM_SRGB:
                return vk::Format::eBc7SrgbBlock;
            case TextureFormat::ETC2_R8G8B8_UNORM:
                return vk::Format::eEtc2R8G8B8UnormBlock;
            case TextureFormat::ETC2_R8G8B8A1_UNORM:
                return vk::Format::eEtc2R8G8B8A1UnormBlock;
            case TextureFormat::ETC2_R8G8B8A8_UNORM:
                return vk::Format::eEtc2R8G8B8A8UnormBlock;
            case TextureFormat::ASTC_4x4_UNORM:
                return vk::Format::eAstc4x4UnormBlock;
            case TextureFormat::ASTC_4x4_UNORM_SRGB:
                return vk::Format::eAstc4x4SrgbBlock;
            default:
                return vk::Format::eUndefined;
            }
        }

        TextureFormat fromVkFormat(vk::Format format)
        {
            switch (format)
            {
            case vk::Format::eB8G8R8A8Srgb:
                return TextureFormat::B8G8R8A8_UNORM_SRGB;
            case vk::Format::eB8G8R8A8Unorm:
                return TextureFormat::B8G8R8A8_UNORM;
            case vk::Format::eR8G8B8A8Srgb:
                return TextureFormat::R8G8B8A8_UNORM_SRGB;
            case vk::Format::eR8G8B8A8Unorm:
                return TextureFormat::R8G8B8A8_UNORM;
            case vk::Format::eR16G16B16A16Sfloat:
                return TextureFormat::R16G16B16A16_FLOAT;
            case vk::Format::eR16G16B16A16Unorm:
                return TextureFormat::R16G16B16A16_UNORM;
            case vk::Format::eA2B10G10R10UnormPack32:
                return TextureFormat::R10G10B10A2_UNORM;
            case vk::Format::eD32Sfloat:
                return TextureFormat::D32_FLOAT;
            case vk::Format::eD24UnormS8Uint:
                return TextureFormat::D24_UNORM_S8_UINT;
            default:
                return TextureFormat::UNKNOWN;
            }
        }

        vk::BufferUsageFlags toVkBufferUsage(BufferUsage usage)
        {
            vk::BufferUsageFlags flags{};
            if (any(usage & BufferUsage::VERTEX))
            {
                flags |= vk::BufferUsageFlagBits::eVertexBuffer;
            }
            if (any(usage & BufferUsage::INDEX))
            {
                flags |= vk::BufferUsageFlagBits::eIndexBuffer;
            }
            if (any(usage & BufferUsage::UNIFORM))
            {
                flags |= vk::BufferUsageFlagBits::eUniformBuffer;
            }
            if (any(usage & BufferUsage::STORAGE))
            {
                flags |= vk::BufferUsageFlagBits::eStorageBuffer;
            }
            if (any(usage & BufferUsage::INDIRECT))
            {
                flags |= vk::BufferUsageFlagBits::eIndirectBuffer;
            }
            if (any(usage & BufferUsage::TRANSFER_SRC))
            {
                flags |= vk::BufferUsageFlagBits::eTransferSrc;
            }
            if (any(usage & BufferUsage::TRANSFER_DST))
            {
                flags |= vk::BufferUsageFlagBits::eTransferDst;
            }
            if (any(usage & BufferUsage::SHADER_DEVICE_ADDRESS))
            {
                flags |= vk::BufferUsageFlagBits::eShaderDeviceAddress;
            }
#ifdef VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME
            if (any(usage & BufferUsage::ACCEL_STRUCT_BUILD_INPUT))
            {
                flags |= vk::BufferUsageFlagBits::eAccelerationStructureBuildInputReadOnlyKHR;
            }
#else
            if (any(usage & BufferUsage::ACCEL_STRUCT_BUILD_INPUT))
            {
                flags |= vk::BufferUsageFlagBits::eStorageBuffer;
            }
#endif

            if (!flags)
            {
                flags |= vk::BufferUsageFlagBits::eTransferSrc;
            }

            return flags;
        }

        vk::ImageUsageFlags toVkImageUsage(TextureUsage usage)
        {
            vk::ImageUsageFlags flags{};
            if (any(usage & TextureUsage::SAMPLED))
            {
                flags |= vk::ImageUsageFlagBits::eSampled;
            }
            if (any(usage & TextureUsage::STORAGE))
            {
                flags |= vk::ImageUsageFlagBits::eStorage;
            }
            if (any(usage & TextureUsage::COLOR_ATTACHMENT))
            {
                flags |= vk::ImageUsageFlagBits::eColorAttachment;
            }
            if (any(usage & TextureUsage::DEPTH_STENCIL_ATTACHMENT))
            {
                flags |= vk::ImageUsageFlagBits::eDepthStencilAttachment;
            }
            if (any(usage & TextureUsage::TRANSFER_SRC))
            {
                flags |= vk::ImageUsageFlagBits::eTransferSrc;
            }
            if (any(usage & TextureUsage::TRANSFER_DST))
            {
                flags |= vk::ImageUsageFlagBits::eTransferDst;
            }
            if (any(usage & TextureUsage::INPUT_ATTACHMENT))
            {
                flags |= vk::ImageUsageFlagBits::eInputAttachment;
            }

            if (any(usage & TextureUsage::RESOLVE_SRC))
            {
                flags |= vk::ImageUsageFlagBits::eTransferSrc;
            }
            if (any(usage & TextureUsage::RESOLVE_DST))
            {
                flags |= vk::ImageUsageFlagBits::eTransferDst;
            }

            if (!flags)
            {
                flags |= vk::ImageUsageFlagBits::eSampled;
            }

            return flags;
        }

        vk::SampleCountFlagBits toVkSampleCount(SampleCount samples)
        {
            switch (samples)
            {
            case SampleCount::_2:
                return vk::SampleCountFlagBits::e2;
            case SampleCount::_4:
                return vk::SampleCountFlagBits::e4;
            case SampleCount::_8:
                return vk::SampleCountFlagBits::e8;
            case SampleCount::_16:
                return vk::SampleCountFlagBits::e16;
            default:
                return vk::SampleCountFlagBits::e1;
            }
        }

        vk::ImageType toVkImageType(const ImageDesc &desc)
        {
            if (desc.depth > 1)
            {
                return vk::ImageType::e3D;
            }
            return vk::ImageType::e2D;
        }

        vk::Extent3D toVkExtent(const ImageDesc &desc)
        {
            return vk::Extent3D{
                std::max(desc.width, 1u),
                std::max(desc.height, 1u),
                std::max(desc.depth, 1u)};
        }

        vk::ImageViewType toVkImageViewType(ImageViewType type)
        {
            switch (type)
            {
            case ImageViewType::TYPE_1D:
                return vk::ImageViewType::e1D;
            case ImageViewType::TYPE_2D:
                return vk::ImageViewType::e2D;
            case ImageViewType::TYPE_2D_ARRAY:
                return vk::ImageViewType::e2DArray;
            case ImageViewType::TYPE_3D:
                return vk::ImageViewType::e3D;
            case ImageViewType::TYPE_CUBE:
                return vk::ImageViewType::eCube;
            case ImageViewType::TYPE_CUBE_ARRAY:
                return vk::ImageViewType::eCubeArray;
            default:
                return vk::ImageViewType::e2D;
            }
        }

        vk::ImageAspectFlags toVkAspectFlags(AspectMask mask)
        {
            vk::ImageAspectFlags flags{};
            if (any(mask & AspectMask::COLOR))
            {
                flags |= vk::ImageAspectFlagBits::eColor;
            }
            if (any(mask & AspectMask::DEPTH))
            {
                flags |= vk::ImageAspectFlagBits::eDepth;
            }
            if (any(mask & AspectMask::STENCIL))
            {
                flags |= vk::ImageAspectFlagBits::eStencil;
            }
            return flags;
        }

        bool isDepthFormat(TextureFormat fmt)
        {
            switch (fmt)
            {
            case TextureFormat::D16_UNORM:
            case TextureFormat::D24_UNORM:
            case TextureFormat::D24_UNORM_S8_UINT:
            case TextureFormat::D32_FLOAT:
            case TextureFormat::D32_FLOAT_S8X24_UINT:
                return true;
            default:
                return false;
            }
        }

        bool hasStencil(TextureFormat fmt)
        {
            switch (fmt)
            {
            case TextureFormat::D24_UNORM_S8_UINT:
            case TextureFormat::D32_FLOAT_S8X24_UINT:
            case TextureFormat::S8_UINT:
                return true;
            default:
                return false;
            }
        }

        vk::ImageAspectFlags defaultAspectFlags(TextureFormat fmt)
        {
            if (fmt == TextureFormat::S8_UINT)
            {
                return vk::ImageAspectFlagBits::eStencil;
            }

            if (isDepthFormat(fmt))
            {
                vk::ImageAspectFlags flags = vk::ImageAspectFlagBits::eDepth;
                if (hasStencil(fmt))
                {
                    flags |= vk::ImageAspectFlagBits::eStencil;
                }
                return flags;
            }

            return vk::ImageAspectFlagBits::eColor;
        }

        AspectMask fromVkAspectFlags(vk::ImageAspectFlags flags)
        {
            AspectMask mask = AspectMask::NONE;
            if (flags & vk::ImageAspectFlagBits::eColor)
            {
                mask |= AspectMask::COLOR;
            }
            if (flags & vk::ImageAspectFlagBits::eDepth)
            {
                mask |= AspectMask::DEPTH;
            }
            if (flags & vk::ImageAspectFlagBits::eStencil)
            {
                mask |= AspectMask::STENCIL;
            }
            return mask;
        }

        vk::Filter toVkFilter(Filter filter)
        {
            switch (filter)
            {
            case Filter::NEAREST:
                return vk::Filter::eNearest;
            case Filter::LINEAR:
            default:
                return vk::Filter::eLinear;
            }
        }

        vk::SamplerMipmapMode toVkMipmapMode(MipmapMode mode)
        {
            switch (mode)
            {
            case MipmapMode::NEAREST:
                return vk::SamplerMipmapMode::eNearest;
            case MipmapMode::LINEAR:
            default:
                return vk::SamplerMipmapMode::eLinear;
            }
        }

        vk::SamplerAddressMode toVkAddressMode(AddressMode mode)
        {
            switch (mode)
            {
            case AddressMode::REPEAT:
                return vk::SamplerAddressMode::eRepeat;
            case AddressMode::MIRRORED_REPEAT:
                return vk::SamplerAddressMode::eMirroredRepeat;
            case AddressMode::CLAMP_TO_EDGE:
                return vk::SamplerAddressMode::eClampToEdge;
            case AddressMode::CLAMP_TO_BORDER:
            default:
                return vk::SamplerAddressMode::eClampToBorder;
            }
        }

        vk::BorderColor toVkBorderColor(BorderColor color)
        {
            switch (color)
            {
            case BorderColor::TRANSPARENT_BLACK:
                return vk::BorderColor::eFloatTransparentBlack;
            case BorderColor::OPAQUE_WHITE:
                return vk::BorderColor::eFloatOpaqueWhite;
            case BorderColor::OPAQUE_BLACK:
            default:
                return vk::BorderColor::eFloatOpaqueBlack;
            }
        }

        vk::CompareOp toVkCompareOp(CompareOp op)
        {
            switch (op)
            {
            case CompareOp::NEVER:
                return vk::CompareOp::eNever;
            case CompareOp::LESS:
                return vk::CompareOp::eLess;
            case CompareOp::EQUAL:
                return vk::CompareOp::eEqual;
            case CompareOp::LESS_EQUAL:
                return vk::CompareOp::eLessOrEqual;
            case CompareOp::GREATER:
                return vk::CompareOp::eGreater;
            case CompareOp::NOT_EQUAL:
                return vk::CompareOp::eNotEqual;
            case CompareOp::GREATER_EQUAL:
                return vk::CompareOp::eGreaterOrEqual;
            case CompareOp::ALWAYS:
            default:
                return vk::CompareOp::eAlways;
            }
        }

        device::DeviceType mapDeviceType(vk::PhysicalDeviceType type)
        {
            using device::DeviceType;
            switch (type)
            {
            case vk::PhysicalDeviceType::eIntegratedGpu:
                return DeviceType::INTEGRATED_GPU;
            case vk::PhysicalDeviceType::eDiscreteGpu:
                return DeviceType::DISCRETE_GPU;
            case vk::PhysicalDeviceType::eVirtualGpu:
                return DeviceType::VIRTUAL_GPU;
            case vk::PhysicalDeviceType::eCpu:
                return DeviceType::CPU;
            default:
                return DeviceType::UNKNOWN;
            }
        }

        bool extensionAvailable(const std::vector<vk::ExtensionProperties> &extensions, const char *name)
        {
            return std::any_of(extensions.begin(), extensions.end(),
                               [&](const vk::ExtensionProperties &ext)
                               { return std::strcmp(ext.extensionName, name) == 0; });
        }

        bool requestExtension(const std::vector<vk::ExtensionProperties> &extensions,
                              std::vector<const char *> &requested,
                              const char *name)
        {
            if (!extensionAvailable(extensions, name))
            {
                return false;
            }
            const bool alreadyRequested = std::any_of(
                requested.begin(), requested.end(),
                [&](const char *existing)
                { return std::strcmp(existing, name) == 0; });
            if (!alreadyRequested)
            {
                requested.push_back(name);
            }
            return true;
        }

        vk::ShaderStageFlagBits toVkShaderStage(ShaderStage stage)
        {
            switch (stage)
            {
            case ShaderStage::VERTEX:
                return vk::ShaderStageFlagBits::eVertex;
            case ShaderStage::FRAGMENT:
                return vk::ShaderStageFlagBits::eFragment;
            case ShaderStage::COMPUTE:
                return vk::ShaderStageFlagBits::eCompute;
            case ShaderStage::GEOMETRY:
                return vk::ShaderStageFlagBits::eGeometry;
            case ShaderStage::TESS_CONTROL:
                return vk::ShaderStageFlagBits::eTessellationControl;
            case ShaderStage::TESS_EVAL:
                return vk::ShaderStageFlagBits::eTessellationEvaluation;
            case ShaderStage::TASK:
                return vk::ShaderStageFlagBits::eTaskEXT;
            case ShaderStage::MESH:
                return vk::ShaderStageFlagBits::eMeshEXT;
            case ShaderStage::RAYGEN:
                return vk::ShaderStageFlagBits::eRaygenKHR;
            case ShaderStage::MISS:
                return vk::ShaderStageFlagBits::eMissKHR;
            case ShaderStage::CLOSEST_HIT:
                return vk::ShaderStageFlagBits::eClosestHitKHR;
            case ShaderStage::ANY_HIT:
                return vk::ShaderStageFlagBits::eAnyHitKHR;
            case ShaderStage::INTERSECTION:
                return vk::ShaderStageFlagBits::eIntersectionKHR;
            case ShaderStage::CALLABLE:
                return vk::ShaderStageFlagBits::eCallableKHR;
            case ShaderStage::ALL_GRAPHICS:
                return vk::ShaderStageFlagBits::eAllGraphics;
            default:
                throw std::runtime_error("Unsupported shader stage");
            }
        }

        vk::ShaderStageFlags toVkShaderStageFlags(ShaderStage stages)
        {
            if (stages == ShaderStage::NONE)
            {
                return {};
            }

            if (stages == ShaderStage::ALL_GRAPHICS)
            {
                return vk::ShaderStageFlagBits::eAllGraphics;
            }

            vk::ShaderStageFlags flags{};
            const uint32_t value = to_u(stages);
            if (value & to_u(ShaderStage::VERTEX))
            {
                flags |= vk::ShaderStageFlagBits::eVertex;
            }
            if (value & to_u(ShaderStage::FRAGMENT))
            {
                flags |= vk::ShaderStageFlagBits::eFragment;
            }
            if (value & to_u(ShaderStage::COMPUTE))
            {
                flags |= vk::ShaderStageFlagBits::eCompute;
            }
            if (value & to_u(ShaderStage::GEOMETRY))
            {
                flags |= vk::ShaderStageFlagBits::eGeometry;
            }
            if (value & to_u(ShaderStage::TESS_CONTROL))
            {
                flags |= vk::ShaderStageFlagBits::eTessellationControl;
            }
            if (value & to_u(ShaderStage::TESS_EVAL))
            {
                flags |= vk::ShaderStageFlagBits::eTessellationEvaluation;
            }
            if (value & to_u(ShaderStage::TASK))
            {
                flags |= vk::ShaderStageFlagBits::eTaskEXT;
            }
            if (value & to_u(ShaderStage::MESH))
            {
                flags |= vk::ShaderStageFlagBits::eMeshEXT;
            }
            if (value & to_u(ShaderStage::RAYGEN))
            {
                flags |= vk::ShaderStageFlagBits::eRaygenKHR;
            }
            if (value & to_u(ShaderStage::MISS))
            {
                flags |= vk::ShaderStageFlagBits::eMissKHR;
            }
            if (value & to_u(ShaderStage::CLOSEST_HIT))
            {
                flags |= vk::ShaderStageFlagBits::eClosestHitKHR;
            }
            if (value & to_u(ShaderStage::ANY_HIT))
            {
                flags |= vk::ShaderStageFlagBits::eAnyHitKHR;
            }
            if (value & to_u(ShaderStage::INTERSECTION))
            {
                flags |= vk::ShaderStageFlagBits::eIntersectionKHR;
            }
            if (value & to_u(ShaderStage::CALLABLE))
            {
                flags |= vk::ShaderStageFlagBits::eCallableKHR;
            }

            return flags;
        }

        vk::DescriptorType toVkDescriptorTypeInternal(DescType type)
        {
            switch (type)
            {
            case DescType::UniformBuffer:
                return vk::DescriptorType::eUniformBuffer;
            case DescType::StorageBuffer:
                return vk::DescriptorType::eStorageBuffer;
            case DescType::CombinedImageSampler:
                return vk::DescriptorType::eCombinedImageSampler;
            case DescType::SampledImage:
                return vk::DescriptorType::eSampledImage;
            case DescType::Sampler:
                return vk::DescriptorType::eSampler;
            case DescType::StorageImage:
                return vk::DescriptorType::eStorageImage;
            case DescType::TexelBuffer:
                return vk::DescriptorType::eUniformTexelBuffer;
            case DescType::StorageTexelBuffer:
                return vk::DescriptorType::eStorageTexelBuffer;
            case DescType::AccelStructureKHR:
                return vk::DescriptorType::eAccelerationStructureKHR;
            default:
                throw std::runtime_error("Unsupported descriptor type");
            }
        }

        vk::DescriptorSetLayoutBinding makeDescriptorBinding(const BindingInfo &info)
        {
            vk::DescriptorSetLayoutBinding binding{};
            binding.binding = info.binding;
            binding.descriptorType = toVkDescriptorTypeInternal(info.type);
            uint32_t count = info.count == 0 ? 1u : info.count;
            if (info.bindless && info.count == 0)
            {
                count = kDefaultBindlessDescriptorCount;
            }
            binding.descriptorCount = count;
            ShaderStage stageMask = info.stages == 0 ? ShaderStage::ALL_GRAPHICS : static_cast<ShaderStage>(info.stages);
            binding.stageFlags = toVkShaderStageFlags(stageMask);
            return binding;
        }

        vk::PrimitiveTopology toVkPrimitiveTopology(PrimitiveTopology topology)
        {
            switch (topology)
            {
            case PrimitiveTopology::POINT_LIST:
                return vk::PrimitiveTopology::ePointList;
            case PrimitiveTopology::LINE_LIST:
                return vk::PrimitiveTopology::eLineList;
            case PrimitiveTopology::LINE_STRIP:
                return vk::PrimitiveTopology::eLineStrip;
            case PrimitiveTopology::TRIANGLE_LIST:
                return vk::PrimitiveTopology::eTriangleList;
            case PrimitiveTopology::TRIANGLE_STRIP:
                return vk::PrimitiveTopology::eTriangleStrip;
            case PrimitiveTopology::PATCH_LIST:
                return vk::PrimitiveTopology::ePatchList;
            default:
                return vk::PrimitiveTopology::eTriangleList;
            }
        }

        vk::PolygonMode toVkPolygonMode(PolygonMode mode)
        {
            switch (mode)
            {
            case PolygonMode::LINE:
                return vk::PolygonMode::eLine;
            case PolygonMode::POINT:
                return vk::PolygonMode::ePoint;
            case PolygonMode::FILL:
            default:
                return vk::PolygonMode::eFill;
            }
        }

        vk::CullModeFlags toVkCullMode(CullMode mode)
        {
            switch (mode)
            {
            case CullMode::NONE:
                return vk::CullModeFlagBits::eNone;
            case CullMode::FRONT:
                return vk::CullModeFlagBits::eFront;
            case CullMode::FRONT_AND_BACK:
                return vk::CullModeFlagBits::eFrontAndBack;
            case CullMode::BACK:
            default:
                return vk::CullModeFlagBits::eBack;
            }
        }

        vk::FrontFace toVkFrontFace(FrontFace face)
        {
            return face == FrontFace::CLOCKWISE ? vk::FrontFace::eClockwise : vk::FrontFace::eCounterClockwise;
        }

        vk::IndexType toVkIndexType(IndexType type)
        {
            return type == IndexType::UINT16 ? vk::IndexType::eUint16 : vk::IndexType::eUint32;
        }

        vk::BlendFactor toVkBlendFactor(BlendFactor factor)
        {
            switch (factor)
            {
            case BlendFactor::ZERO:
                return vk::BlendFactor::eZero;
            case BlendFactor::ONE:
                return vk::BlendFactor::eOne;
            case BlendFactor::SRC_COLOR:
                return vk::BlendFactor::eSrcColor;
            case BlendFactor::ONE_MINUS_SRC_COLOR:
                return vk::BlendFactor::eOneMinusSrcColor;
            case BlendFactor::DST_COLOR:
                return vk::BlendFactor::eDstColor;
            case BlendFactor::ONE_MINUS_DST_COLOR:
                return vk::BlendFactor::eOneMinusDstColor;
            case BlendFactor::SRC_ALPHA:
                return vk::BlendFactor::eSrcAlpha;
            case BlendFactor::ONE_MINUS_SRC_ALPHA:
                return vk::BlendFactor::eOneMinusSrcAlpha;
            case BlendFactor::DST_ALPHA:
                return vk::BlendFactor::eDstAlpha;
            case BlendFactor::ONE_MINUS_DST_ALPHA:
                return vk::BlendFactor::eOneMinusDstAlpha;
            default:
                return vk::BlendFactor::eOne;
            }
        }

        vk::BlendOp toVkBlendOp(BlendOp op)
        {
            switch (op)
            {
            case BlendOp::ADD:
                return vk::BlendOp::eAdd;
            case BlendOp::SUBTRACT:
                return vk::BlendOp::eSubtract;
            case BlendOp::REVERSE_SUBTRACT:
                return vk::BlendOp::eReverseSubtract;
            case BlendOp::MIN:
                return vk::BlendOp::eMin;
            case BlendOp::MAX:
                return vk::BlendOp::eMax;
            default:
                return vk::BlendOp::eAdd;
            }
        }

        vk::ColorComponentFlags toVkColorWriteMask(uint8_t mask)
        {
            vk::ColorComponentFlags flags{};
            if (mask & 0x1)
                flags |= vk::ColorComponentFlagBits::eR;
            if (mask & 0x2)
                flags |= vk::ColorComponentFlagBits::eG;
            if (mask & 0x4)
                flags |= vk::ColorComponentFlagBits::eB;
            if (mask & 0x8)
                flags |= vk::ColorComponentFlagBits::eA;
            return flags;
        }

        vk::ImageLayout toVkImageLayout(ImageLayout layout)
        {
            switch (layout)
            {
            case ImageLayout::UNDEFINED:
                return vk::ImageLayout::eUndefined;
            case ImageLayout::GENERAL:
                return vk::ImageLayout::eGeneral;
            case ImageLayout::SHADER_READ_ONLY:
                return vk::ImageLayout::eShaderReadOnlyOptimal;
            case ImageLayout::COLOR_ATTACHMENT:
                return vk::ImageLayout::eColorAttachmentOptimal;
            case ImageLayout::DEPTH_STENCIL_ATTACHMENT:
                return vk::ImageLayout::eDepthStencilAttachmentOptimal;
            case ImageLayout::TRANSFER_SRC:
                return vk::ImageLayout::eTransferSrcOptimal;
            case ImageLayout::TRANSFER_DST:
                return vk::ImageLayout::eTransferDstOptimal;
            case ImageLayout::PRESENT:
                return vk::ImageLayout::ePresentSrcKHR;
            default:
                return vk::ImageLayout::eUndefined;
            }
        }

        vk::PipelineStageFlags2 toVkPipelineStage2(PipelineStage stage)
        {
            switch (stage)
            {
            case PipelineStage::TOP:
                return vk::PipelineStageFlagBits2::eTopOfPipe;
            case PipelineStage::DRAW:
                return vk::PipelineStageFlagBits2::eAllGraphics;
            case PipelineStage::COMPUTE:
                return vk::PipelineStageFlagBits2::eComputeShader;
            case PipelineStage::COPY:
                return vk::PipelineStageFlagBits2::eTransfer;
            case PipelineStage::BOTTOM:
                return vk::PipelineStageFlagBits2::eBottomOfPipe;
            case PipelineStage::NONE:
            default:
                return vk::PipelineStageFlagBits2::eNone;
            }
        }

        vk::AccessFlags2 toVkAccess2(Access access)
        {
            vk::AccessFlags2 flags{};
            if (any(access & Access::INDIRECT_READ))
                flags |= vk::AccessFlagBits2::eIndirectCommandRead;
            if (any(access & Access::UNIFORM_READ))
                flags |= vk::AccessFlagBits2::eUniformRead;
            if (any(access & Access::SHADER_SAMPLED_READ))
                flags |= vk::AccessFlagBits2::eShaderSampledRead;
            if (any(access & Access::SHADER_STORAGE_READ))
                flags |= vk::AccessFlagBits2::eShaderStorageRead;
            if (any(access & Access::SHADER_STORAGE_WRITE))
                flags |= vk::AccessFlagBits2::eShaderStorageWrite;
            if (any(access & Access::COLOR_ATTACHMENT_READ))
                flags |= vk::AccessFlagBits2::eColorAttachmentRead;
            if (any(access & Access::COLOR_ATTACHMENT_WRITE))
                flags |= vk::AccessFlagBits2::eColorAttachmentWrite;
            if (any(access & Access::DEPTH_STENCIL_READ))
                flags |= vk::AccessFlagBits2::eDepthStencilAttachmentRead;
            if (any(access & Access::DEPTH_STENCIL_WRITE))
                flags |= vk::AccessFlagBits2::eDepthStencilAttachmentWrite;
            if (any(access & Access::TRANSFER_READ))
                flags |= vk::AccessFlagBits2::eTransferRead;
            if (any(access & Access::TRANSFER_WRITE))
                flags |= vk::AccessFlagBits2::eTransferWrite;
            return flags;
        }

        vk::Extent2D deriveExtent(uint32_t width, uint32_t height, const ImageDesc &imageDesc)
        {
            vk::Extent2D extent{};
            extent.width = width ? width : std::max(imageDesc.width, 1u);
            extent.height = height ? height : std::max(imageDesc.height, 1u);
            return extent;
        }

        vk::MemoryPropertyFlags memoryPropertiesFromUsage(MemoryUsage usage)
        {
            switch (usage)
            {
            case MemoryUsage::CPU_ONLY:
            case MemoryUsage::CPU_TO_GPU:
            {
                vk::MemoryPropertyFlags flags = vk::MemoryPropertyFlagBits::eHostVisible;
                flags |= vk::MemoryPropertyFlagBits::eHostCoherent;
                return flags;
            }
            case MemoryUsage::GPU_TO_CPU:
            {
                vk::MemoryPropertyFlags flags = vk::MemoryPropertyFlagBits::eHostVisible;
                flags |= vk::MemoryPropertyFlagBits::eHostCoherent;
                flags |= vk::MemoryPropertyFlagBits::eHostCached;
                return flags;
            }
            case MemoryUsage::GPU_ONLY:
            default:
            {
                vk::MemoryPropertyFlags flags = vk::MemoryPropertyFlagBits::eDeviceLocal;
                return flags;
            }
            }
        }

        uint32_t findMemoryType(uint32_t typeBits, vk::MemoryPropertyFlags required,
                                const vk::PhysicalDeviceMemoryProperties &props,
                                vk::MemoryPropertyFlags fallback = {})
        {
            for (uint32_t i = 0; i < props.memoryTypeCount; ++i)
            {
                const bool supported = (typeBits & (1u << i)) != 0;
                const bool matches = (props.memoryTypes[i].propertyFlags & required) == required;
                if (supported && matches)
                {
                    return i;
                }
            }

            if (fallback)
            {
                for (uint32_t i = 0; i < props.memoryTypeCount; ++i)
                {
                    const bool supported = (typeBits & (1u << i)) != 0;
                    const bool matches = (props.memoryTypes[i].propertyFlags & fallback) == fallback;
                    if (supported && matches)
                    {
                        return i;
                    }
                }
            }

            throw std::runtime_error("No suitable Vulkan memory type found");
        }

        void ensureSuccess(vk::Result result, const char *message)
        {
            if (result != vk::Result::eSuccess)
            {
                throw std::runtime_error(message);
            }
        }
    }

    vk::UniqueInstance VulkanDevice::m_instance;
    const char *VulkanDevice::kValidationLayer = "VK_LAYER_KHRONOS_validation";

    VulkanDevice::VulkanDevice(const DeviceCreateInfo &info)
        : m_window(static_cast<SDL_Window *>(info.windowHandle)),
          m_headless(info.headless || info.windowHandle == nullptr),
          m_preferredColorFormat(info.preferredColorFormat)
    {
        if (info.api != device::API::VULKAN)
        {
            throw std::runtime_error("Vulkan only");
        }

        VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

        if (!m_instance.get())
        {
            const char *validationLayers[] = {kValidationLayer};
            bool enableValidation = info.enableValidation;

            std::vector<const char *> instanceExtensions;
            if (!m_headless)
            {
                if (!SDL_Vulkan_LoadLibrary(nullptr))
                {
                    throw std::runtime_error(SDL_GetError());
                }

                Uint32 extensionCount = 0;
                const char *const *extensions = SDL_Vulkan_GetInstanceExtensions(&extensionCount);
                if (!extensions || extensionCount == 0)
                {
                    throw std::runtime_error("SDL_Vulkan_GetInstanceExtensions returned no extensions");
                }
                instanceExtensions.assign(extensions, extensions + extensionCount);
            }

            if (enableValidation)
            {
                vk::ResultValue<std::vector<vk::LayerProperties>> layerResult = vk::enumerateInstanceLayerProperties();
                ensureSuccess(layerResult.result, "Failed to enumerate Vulkan instance layers");
                const std::vector<vk::LayerProperties> &availableLayers = layerResult.value;
                const bool hasValidationLayer = std::any_of(
                    availableLayers.begin(), availableLayers.end(),
                    [&](const vk::LayerProperties &layer)
                    { return std::strcmp(layer.layerName, kValidationLayer) == 0; });

                if (!hasValidationLayer)
                {
                    enableValidation = false;
                    std::fprintf(stderr,
                                 "Vulkan validation layer '%s' requested but not available; continuing without validation.\n",
                                 kValidationLayer);
                }
            }

            vk::ApplicationInfo appInfo{};
            appInfo.pApplicationName = "Pixl Engine";
            appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
            appInfo.pEngineName = "Pixl";
            appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
            appInfo.apiVersion = VK_API_VERSION_1_4;

            vk::InstanceCreateInfo instanceCreateInfo{};
            instanceCreateInfo.pApplicationInfo = &appInfo;
            instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
            instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.empty() ? nullptr : instanceExtensions.data();

            if (enableValidation)
            {
                instanceCreateInfo.enabledLayerCount = 1;
                instanceCreateInfo.ppEnabledLayerNames = validationLayers;
            }

            vk::ResultValue<vk::UniqueInstance> instanceResult = vk::createInstanceUnique(instanceCreateInfo);
            if (instanceResult.result != vk::Result::eSuccess)
            {
                throw std::runtime_error("Failed to create Vulkan instance");
            }
            m_instance = std::move(instanceResult.value);
            VULKAN_HPP_DEFAULT_DISPATCHER.init(m_instance.get());
        }

        if (!m_headless)
        {
            createSurface(m_window);
        }

        const std::vector<vk::PhysicalDevice> physicalDevices = m_instance->enumeratePhysicalDevices().value;
        if (physicalDevices.empty())
        {
            throw std::runtime_error("No Vulkan physical devices available");
        }

        m_physicalDevice = physicalDevices.front();
        vk::PhysicalDeviceType selectedType = m_physicalDevice.getProperties().deviceType;

        for (const vk::PhysicalDevice &physicalDevice : physicalDevices)
        {
            const vk::PhysicalDeviceProperties props = physicalDevice.getProperties();
            if (props.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
            {
                m_physicalDevice = physicalDevice;
                selectedType = props.deviceType;
                break;
            }

            if (props.deviceType == vk::PhysicalDeviceType::eIntegratedGpu &&
                selectedType != vk::PhysicalDeviceType::eDiscreteGpu)
            {
                m_physicalDevice = physicalDevice;
                selectedType = props.deviceType;
            }
        }

        vk::PhysicalDeviceDriverProperties driverProps{};
        vk::PhysicalDeviceSubgroupProperties subgroupProps{};
        vk::PhysicalDeviceSubgroupSizeControlProperties subgroupSizeProps{};
        vk::PhysicalDeviceMeshShaderPropertiesEXT meshProps{};
        vk::PhysicalDeviceRayTracingPipelinePropertiesKHR rayTracingProps{};
        vk::PhysicalDeviceFragmentShadingRatePropertiesKHR shadingRateProps{};
        driverProps.pNext = &subgroupProps;
        subgroupProps.pNext = &subgroupSizeProps;
        subgroupSizeProps.pNext = &meshProps;
        meshProps.pNext = &rayTracingProps;
        rayTracingProps.pNext = &shadingRateProps;
        vk::PhysicalDeviceProperties2 props2{};
        props2.pNext = &driverProps;
        m_physicalDevice.getProperties2(&props2);
        const vk::PhysicalDeviceProperties &props = props2.properties;

        const vk::PhysicalDeviceMemoryProperties memoryProps = m_physicalDevice.getMemoryProperties();
        m_memoryProperties = memoryProps;
        uint64_t vramBytes = 0;
        for (uint32_t i = 0; i < memoryProps.memoryHeapCount; ++i)
        {
            const vk::MemoryHeap &heap = memoryProps.memoryHeaps[i];
            if (heap.flags & vk::MemoryHeapFlagBits::eDeviceLocal)
            {
                vramBytes += heap.size;
            }
        }

        const std::vector<vk::ExtensionProperties> extensions = m_physicalDevice.enumerateDeviceExtensionProperties().value;

        vk::PhysicalDeviceFeatures2 features2{};
        vk::PhysicalDeviceVulkan12Features features12{};
        vk::PhysicalDeviceVulkan13Features features13{};
        vk::PhysicalDeviceMultiviewFeatures multiviewFeatures{};
        vk::PhysicalDeviceMeshShaderFeaturesEXT meshFeatures{};
        vk::PhysicalDeviceRayTracingPipelineFeaturesKHR rayTracingFeatures{};
        vk::PhysicalDeviceAccelerationStructureFeaturesKHR accelFeatures{};
        vk::PhysicalDeviceRayQueryFeaturesKHR rayQueryFeatures{};
        vk::PhysicalDeviceFragmentShadingRateFeaturesKHR shadingRateFeatures{};
        vk::PhysicalDeviceFragmentShaderInterlockFeaturesEXT fragmentInterlockFeatures{};
        features2.pNext = &features12;
        features12.pNext = &features13;
        features13.pNext = &multiviewFeatures;
        multiviewFeatures.pNext = &meshFeatures;
        meshFeatures.pNext = &rayTracingFeatures;
        rayTracingFeatures.pNext = &accelFeatures;
        accelFeatures.pNext = &rayQueryFeatures;
        rayQueryFeatures.pNext = &shadingRateFeatures;
        shadingRateFeatures.pNext = &fragmentInterlockFeatures;
        m_physicalDevice.getFeatures2(&features2);

        m_info.api = device::API::VULKAN;
        m_info.type = mapDeviceType(props.deviceType);
        m_info.apiVersionMajor = VK_VERSION_MAJOR(props.apiVersion);
        m_info.apiVersionMinor = VK_VERSION_MINOR(props.apiVersion);
        std::strncpy(m_info.name, props.deviceName, sizeof(m_info.name) - 1);
        m_info.name[sizeof(m_info.name) - 1] = '\0';
        m_info.vendorId = props.vendorID;
        m_info.deviceId = props.deviceID;
        m_info.vramBytes = vramBytes;

        if (driverProps.driverName[0] != '\0')
        {
            std::strncpy(m_info.driver, driverProps.driverName, sizeof(m_info.driver) - 1);
            m_info.driver[sizeof(m_info.driver) - 1] = '\0';
        }
        else
        {
            std::snprintf(m_info.driver, sizeof(m_info.driver), "0x%08x", props.driverVersion);
        }

        device::Features features{};
        const vk::PhysicalDeviceFeatures &baseFeatures = features2.features;

        features.descriptorIndexing = features12.descriptorIndexing;
        features.dynamicRendering = features13.dynamicRendering;
        features.timelineSemaphore = features12.timelineSemaphore;
        features.bufferDeviceAddress = features12.bufferDeviceAddress;
        features.shaderInt64 = baseFeatures.shaderInt64;
        features.shaderFloat16 = features12.shaderFloat16;
        features.subgroupBasic = static_cast<bool>(subgroupProps.supportedOperations & vk::SubgroupFeatureFlagBits::eBasic);
        features.subgroupVote = static_cast<bool>(subgroupProps.supportedOperations & vk::SubgroupFeatureFlagBits::eVote);
        features.subgroupBallot = static_cast<bool>(subgroupProps.supportedOperations & vk::SubgroupFeatureFlagBits::eBallot);
        const bool subgroupShuffle = static_cast<bool>(subgroupProps.supportedOperations & vk::SubgroupFeatureFlagBits::eShuffle);
        const bool subgroupShuffleRelative = static_cast<bool>(subgroupProps.supportedOperations & vk::SubgroupFeatureFlagBits::eShuffleRelative);
        features.subgroupShuffle = subgroupShuffle || subgroupShuffleRelative;

        if (meshFeatures.meshShader)
        {
            features.mesh = meshFeatures.taskShader ? device::MeshShaderSupport::TASK_AND_MESH : device::MeshShaderSupport::MESH_ONLY;
        }

        if (rayTracingFeatures.rayTracingPipeline && accelFeatures.accelerationStructure)
        {
            features.rayTracing = device::RayTracingSupport::RT_PIPELINE;
        }
        else if (rayQueryFeatures.rayQuery)
        {
            features.rayTracing = device::RayTracingSupport::RAY_QUERY;
        }

        if (shadingRateFeatures.pipelineFragmentShadingRate)
        {
            features.vrs = shadingRateFeatures.attachmentFragmentShadingRate ? device::VRSTier::TIER2 : device::VRSTier::TIER1;
        }

        bool conservativeRaster = false;
#ifdef VK_EXT_CONSERVATIVE_RASTERIZATION_EXTENSION_NAME
        conservativeRaster = extensionAvailable(extensions, VK_EXT_CONSERVATIVE_RASTERIZATION_EXTENSION_NAME);
#endif
        features.conservativeRaster = conservativeRaster;
        features.fragmentShaderInterlock = fragmentInterlockFeatures.fragmentShaderSampleInterlock ||
                                           fragmentInterlockFeatures.fragmentShaderPixelInterlock ||
                                           fragmentInterlockFeatures.fragmentShaderShadingRateInterlock;

        features.samplerAnisotropy = baseFeatures.samplerAnisotropy;
        bool samplerFilterCubic = false;
#ifdef VK_EXT_FILTER_CUBIC_EXTENSION_NAME
        samplerFilterCubic = samplerFilterCubic || extensionAvailable(extensions, VK_EXT_FILTER_CUBIC_EXTENSION_NAME);
#endif
#ifdef VK_IMG_FILTER_CUBIC_EXTENSION_NAME
        samplerFilterCubic = samplerFilterCubic || extensionAvailable(extensions, VK_IMG_FILTER_CUBIC_EXTENSION_NAME);
#endif
        features.samplerFilterCubic = samplerFilterCubic;
        features.sparseBinding = baseFeatures.sparseBinding;
        features.residency2 = baseFeatures.sparseResidency2Samples;

        device::Limits limits{};
        limits.maxTextureDim1D = props.limits.maxImageDimension1D;
        limits.maxTextureDim2D = props.limits.maxImageDimension2D;
        limits.maxTextureDim3D = props.limits.maxImageDimension3D;
        limits.maxArrayLayers = props.limits.maxImageArrayLayers;
        limits.maxSamplerAnisotropy = static_cast<uint32_t>(props.limits.maxSamplerAnisotropy);
        limits.maxUniformBufferRange = props.limits.maxUniformBufferRange;
        limits.maxStorageBufferRange = props.limits.maxStorageBufferRange;
        limits.maxPushConstantsSize = props.limits.maxPushConstantsSize;
        limits.maxComputeWorkGroupInvocations = props.limits.maxComputeWorkGroupInvocations;
        limits.maxComputeWorkGroupSize[0] = props.limits.maxComputeWorkGroupSize[0];
        limits.maxComputeWorkGroupSize[1] = props.limits.maxComputeWorkGroupSize[1];
        limits.maxComputeWorkGroupSize[2] = props.limits.maxComputeWorkGroupSize[2];
        limits.subgroupSizeMin = subgroupSizeProps.minSubgroupSize ? subgroupSizeProps.minSubgroupSize : subgroupProps.subgroupSize;
        limits.subgroupSizeMax = subgroupSizeProps.maxSubgroupSize ? subgroupSizeProps.maxSubgroupSize : subgroupProps.subgroupSize;
        limits.maxDrawIndirectCount = props.limits.maxDrawIndirectCount;
        limits.maxMeshWorkGroupInvocations = meshProps.maxMeshWorkGroupInvocations;
        limits.maxMeshOutputVertices = meshProps.maxMeshOutputVertices;
        limits.maxMeshOutputPrimitives = meshProps.maxMeshOutputPrimitives;
        limits.maxRayRecursionDepth = static_cast<uint64_t>(rayTracingProps.maxRayRecursionDepth);

        m_info.supportedFeatures = features;
        m_info.limits = limits;

        const std::vector<vk::QueueFamilyProperties> queueFamilies = m_physicalDevice.getQueueFamilyProperties();
        if (queueFamilies.empty())
        {
            throw std::runtime_error("No queue families reported by the physical device");
        }

        const uint32_t invalidIndex = std::numeric_limits<uint32_t>::max();
        uint32_t graphicsFamily = invalidIndex;
        for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilies.size()); ++i)
        {
            if (queueFamilies[i].queueCount == 0)
            {
                continue;
            }

            const bool graphicsCapable = static_cast<bool>(queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics);
            if (!graphicsCapable)
            {
                continue;
            }

            bool presentCapable = true;
            if (!m_headless && m_surface)
            {
                presentCapable = m_physicalDevice.getSurfaceSupportKHR(i, m_surface.get()).value != 0;
            }

            if (graphicsCapable && presentCapable)
            {
                graphicsFamily = i;
                break;
            }
        }

        if (graphicsFamily == invalidIndex)
        {
            throw std::runtime_error("Failed to find a graphics capable queue family with presentation support");
        }

        m_graphicsQueueFamily = graphicsFamily;

        if (meshFeatures.multiviewMeshShader && !multiviewFeatures.multiview)
        {
            throw std::runtime_error("Mesh shader multiview requires multiview feature support");
        }

        createLogicalDevice(info, features2, features12, features13, multiviewFeatures, props.apiVersion, extensions);
        setupQueues(info.queues);
        createSyncObjects();
        if (!m_headless)
        {
            createSwapchain(info.preferredColorFormat);
        }
    }

    VulkanDevice::~VulkanDevice()
    {
        if (m_device)
        {
            ensureSuccess(m_device->waitIdle(), "Failed to wait for device idle");
            collectGarbage(true);
            resetAllTransientPools();
            destroySwapchain();
            m_graphicsTimelineSemaphore.reset();
            m_renderFinishedSemaphores.clear();
            m_imageAvailableSemaphores.clear();
            m_pendingAcquireSemaphore = VK_NULL_HANDLE;
            m_pendingRenderFinishedSemaphore = VK_NULL_HANDLE;
            m_activeRenderFinishedSemaphore = VK_NULL_HANDLE;
            m_graphicsQueueWrapper.reset();
            {
                std::lock_guard<std::mutex> lock(m_resourceMutex);
                for (auto &entry : m_descriptorSets)
                {
                    if (entry.second.pool && entry.second.set)
                    {
                        m_device->freeDescriptorSets(entry.second.pool, entry.second.set);
                    }
                }
                m_descriptorSets.clear();
                m_descriptorSetLayouts.clear();
                m_descriptorPools.clear();
                m_imageViews.clear();
                m_samplers.clear();
                m_shaders.clear();
                m_buffers.clear();
                m_images.clear();
                m_pipelineLayouts.clear();
                m_pipelines.clear();
            }
            if (m_allocator)
            {
                m_allocator.destroy();
                m_allocator = nullptr;
            }
            m_device.reset();
        }

        if (m_surface)
        {
            m_surface.reset();
        }

        if (m_instance.get())
        {
            m_instance.reset();
            if (!m_headless)
            {
                SDL_Vulkan_UnloadLibrary();
            }
        }
    }

    void VulkanDevice::createSurface(SDL_Window *window)
    {
        if (!window)
        {
            return;
        }

        VkSurfaceKHR rawSurface = VK_NULL_HANDLE;
        if (!SDL_Vulkan_CreateSurface(window, static_cast<VkInstance>(m_instance.get()), nullptr, &rawSurface))
        {
            throw std::runtime_error(SDL_GetError());
        }

        m_surface = vk::UniqueSurfaceKHR(
            vk::SurfaceKHR(rawSurface),
            vk::detail::ObjectDestroy<vk::Instance, VULKAN_HPP_DEFAULT_DISPATCHER_TYPE>(m_instance.get()));
    }

    void VulkanDevice::destroySwapchain()
    {
        releaseSwapchainResources();
        m_swapchainImages.clear();
        if (m_swapchain)
        {
            m_swapchain.reset();
        }
        m_hasSwapchainImage = false;
        m_acquireWaitPending = false;
        m_pendingRenderFinishedSemaphore = VK_NULL_HANDLE;
        m_activeRenderFinishedSemaphore = VK_NULL_HANDLE;
    }

    void VulkanDevice::createLogicalDevice(const DeviceCreateInfo &,
                                           const vk::PhysicalDeviceFeatures2 &features2,
                                           const vk::PhysicalDeviceVulkan12Features &features12,
                                           const vk::PhysicalDeviceVulkan13Features &features13,
                                           const vk::PhysicalDeviceMultiviewFeatures &multiviewFeatures,
                                           uint32_t apiVersion,
                                           const std::vector<vk::ExtensionProperties> &extensions)
    {
        if (!features13.dynamicRendering || !features13.synchronization2)
        {
            throw std::runtime_error("Selected device does not support required Vulkan 1.3 features");
        }
        if (!features12.timelineSemaphore)
        {
            throw std::runtime_error("Selected device does not support timeline semaphores");
        }

        std::vector<vk::DeviceQueueCreateInfo> queueInfos;
        float queuePriority = 1.0f;
        vk::DeviceQueueCreateInfo graphicsQueueInfo{};
        graphicsQueueInfo.queueFamilyIndex = m_graphicsQueueFamily;
        graphicsQueueInfo.queueCount = 1;
        graphicsQueueInfo.pQueuePriorities = &queuePriority;
        queueInfos.push_back(graphicsQueueInfo);

        std::vector<const char *> deviceExtensions;

        if (!m_headless)
        {
            if (!requestExtension(extensions, deviceExtensions, VK_KHR_SWAPCHAIN_EXTENSION_NAME))
            {
                throw std::runtime_error("Required extension VK_KHR_swapchain is not supported");
            }
        }

        const uint32_t apiMajor = VK_VERSION_MAJOR(apiVersion);
        const uint32_t apiMinor = VK_VERSION_MINOR(apiVersion);
        const bool timelineCore = apiMajor > 1 || (apiMajor == 1 && apiMinor >= 2);
        const bool sync2Core = apiMajor > 1 || (apiMajor == 1 && apiMinor >= 3);
        const bool dynamicRenderingCore = apiMajor > 1 || (apiMajor == 1 && apiMinor >= 3);

        vk::PhysicalDeviceFeatures enabledFeatures = features2.features;

        vk::PhysicalDeviceVulkan12Features enabled12{};
        enabled12.timelineSemaphore = features12.timelineSemaphore;
        enabled12.bufferDeviceAddress = features12.bufferDeviceAddress;
        enabled12.descriptorIndexing = features12.descriptorIndexing;
        enabled12.runtimeDescriptorArray = features12.runtimeDescriptorArray;
        enabled12.descriptorBindingPartiallyBound = features12.descriptorBindingPartiallyBound;
        enabled12.descriptorBindingUpdateUnusedWhilePending = features12.descriptorBindingUpdateUnusedWhilePending;
        enabled12.descriptorBindingSampledImageUpdateAfterBind = features12.descriptorBindingSampledImageUpdateAfterBind;
        enabled12.descriptorBindingStorageBufferUpdateAfterBind = features12.descriptorBindingStorageBufferUpdateAfterBind;
        enabled12.shaderSampledImageArrayNonUniformIndexing = features12.shaderSampledImageArrayNonUniformIndexing;
        enabled12.shaderStorageBufferArrayNonUniformIndexing = features12.shaderStorageBufferArrayNonUniformIndexing;

        vk::PhysicalDeviceVulkan13Features enabled13{};
        enabled13.dynamicRendering = features13.dynamicRendering;
        enabled13.synchronization2 = features13.synchronization2;

        vk::PhysicalDeviceMultiviewFeatures enabledMultiview{};
        enabledMultiview.multiview = multiviewFeatures.multiview;
        enabledMultiview.multiviewGeometryShader = multiviewFeatures.multiviewGeometryShader;
        enabledMultiview.multiviewTessellationShader = multiviewFeatures.multiviewTessellationShader;

        void *pNextChain = nullptr;
        if (enabledMultiview.multiview || enabledMultiview.multiviewGeometryShader || enabledMultiview.multiviewTessellationShader)
        {
            enabledMultiview.pNext = pNextChain;
            pNextChain = &enabledMultiview;
        }
        if (enabled13.dynamicRendering || enabled13.synchronization2)
        {
            enabled13.pNext = pNextChain;
            pNextChain = &enabled13;
        }
        if (enabled12.timelineSemaphore || enabled12.bufferDeviceAddress || enabled12.descriptorIndexing)
        {
            enabled12.pNext = pNextChain;
            pNextChain = &enabled12;
        }

        bool timelineExtRequested = false;
        bool sync2ExtRequested = false;
        bool dynamicRenderingExtRequested = false;

        if (enabled12.timelineSemaphore)
        {
            timelineExtRequested = requestExtension(extensions, deviceExtensions, VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME);
        }
        if (enabled13.synchronization2)
        {
            sync2ExtRequested = requestExtension(extensions, deviceExtensions, VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME);
        }
        if (enabled13.dynamicRendering)
        {
            dynamicRenderingExtRequested = requestExtension(extensions, deviceExtensions, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
        }

        if (enabled12.timelineSemaphore && !timelineCore && !timelineExtRequested)
        {
            throw std::runtime_error("Timeline semaphore support requires VK_KHR_timeline_semaphore");
        }
        if (enabled13.synchronization2 && !sync2Core && !sync2ExtRequested)
        {
            throw std::runtime_error("Synchronization2 support requires VK_KHR_synchronization2");
        }
        if (enabled13.dynamicRendering && !dynamicRenderingCore && !dynamicRenderingExtRequested)
        {
            throw std::runtime_error("Dynamic rendering support requires VK_KHR_dynamic_rendering");
        }

        vk::DeviceCreateInfo deviceInfo{};
        deviceInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());
        deviceInfo.pQueueCreateInfos = queueInfos.data();
        deviceInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        deviceInfo.ppEnabledExtensionNames = deviceExtensions.data();
        deviceInfo.pEnabledFeatures = &enabledFeatures;
        deviceInfo.pNext = pNextChain;

        vk::ResultValue<vk::UniqueDevice> deviceResult = m_physicalDevice.createDeviceUnique(deviceInfo);
        if (deviceResult.result != vk::Result::eSuccess)
        {
            throw std::runtime_error("Failed to create logical device");
        }
        m_device = std::move(deviceResult.value);
        VULKAN_HPP_DEFAULT_DISPATCHER.init(m_device.get());
        initializeAllocator();
        initializeDescriptorPools();
    }

    void VulkanDevice::initializeAllocator()
    {
        if (!m_device)
        {
            return;
        }

        if (m_allocator)
        {
            m_allocator.destroy();
            m_allocator = nullptr;
        }

        m_vmaFunctions = vma::VulkanFunctions{};
        m_vmaFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
        m_vmaFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

        vma::AllocatorCreateInfo allocatorInfo{};
        allocatorInfo.instance = m_instance.get();
        allocatorInfo.physicalDevice = m_physicalDevice;
        allocatorInfo.device = m_device.get();
        allocatorInfo.pVulkanFunctions = &m_vmaFunctions;
        allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_4;
        allocatorInfo.flags = vma::AllocatorCreateFlagBits::eBufferDeviceAddress;

        vk::ResultValue<vma::Allocator> allocatorResult = vma::createAllocator(allocatorInfo);
        if (allocatorResult.result != vk::Result::eSuccess)
        {
            throw std::runtime_error("Failed to create Vulkan memory allocator");
        }
        m_allocator = std::move(allocatorResult.value);
    }

    void VulkanDevice::initializeDescriptorPools()
    {
        m_basePoolSizes.clear();
        m_basePoolSizes.push_back(vk::DescriptorPoolSize{vk::DescriptorType::eSampledImage, 100000});
        m_basePoolSizes.push_back(vk::DescriptorPoolSize{vk::DescriptorType::eCombinedImageSampler, 8192});
        m_basePoolSizes.push_back(vk::DescriptorPoolSize{vk::DescriptorType::eSampler, 4096});
        m_basePoolSizes.push_back(vk::DescriptorPoolSize{vk::DescriptorType::eUniformBuffer, 16384});
        m_basePoolSizes.push_back(vk::DescriptorPoolSize{vk::DescriptorType::eStorageBuffer, 8192});
        m_basePoolSizes.push_back(vk::DescriptorPoolSize{vk::DescriptorType::eStorageImage, 4096});
        m_basePoolSizes.push_back(vk::DescriptorPoolSize{vk::DescriptorType::eUniformTexelBuffer, 2048});
        m_basePoolSizes.push_back(vk::DescriptorPoolSize{vk::DescriptorType::eStorageTexelBuffer, 1024});
    }

    void VulkanDevice::createSwapchain(TextureFormat preferredFormat)
    {
        if (m_headless || !m_surface || !m_device)
        {
            return;
        }

        const vk::SurfaceCapabilitiesKHR caps = m_physicalDevice.getSurfaceCapabilitiesKHR(m_surface.get()).value;
        std::vector<vk::SurfaceFormatKHR> availableFormats = m_physicalDevice.getSurfaceFormatsKHR(m_surface.get()).value;
        if (availableFormats.empty())
        {
            throw std::runtime_error("No swapchain formats available");
        }

        vk::SurfaceFormatKHR chosenFormat = availableFormats.front();
        const vk::Format desiredFormat = toVkFormat(preferredFormat);
        for (const vk::SurfaceFormatKHR &format : availableFormats)
        {
            if (desiredFormat != vk::Format::eUndefined && format.format == desiredFormat)
            {
                chosenFormat = format;
                break;
            }

            if (format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
            {
                chosenFormat = format;
            }
        }

        vk::Extent2D extent{};
        if (caps.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            extent = caps.currentExtent;
        }
        else
        {
            int w = 0;
            int h = 0;
            SDL_GetWindowSize(m_window, &w, &h);
            extent.width = std::clamp(static_cast<uint32_t>(w), caps.minImageExtent.width, caps.maxImageExtent.width);
            extent.height = std::clamp(static_cast<uint32_t>(h), caps.minImageExtent.height, caps.maxImageExtent.height);
        }

        uint32_t imageCount = caps.minImageCount + 1;
        if (caps.maxImageCount > 0 && imageCount > caps.maxImageCount)
        {
            imageCount = caps.maxImageCount;
        }

        vk::SwapchainCreateInfoKHR swapchainInfo{};
        swapchainInfo.surface = m_surface.get();
        swapchainInfo.minImageCount = imageCount;
        swapchainInfo.imageFormat = chosenFormat.format;
        swapchainInfo.imageColorSpace = chosenFormat.colorSpace;
        swapchainInfo.imageExtent = extent;
        swapchainInfo.imageArrayLayers = 1;
        swapchainInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
        swapchainInfo.imageUsage |= vk::ImageUsageFlagBits::eTransferDst;
        swapchainInfo.imageSharingMode = vk::SharingMode::eExclusive;
        swapchainInfo.preTransform = caps.currentTransform;
        swapchainInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        swapchainInfo.presentMode = vk::PresentModeKHR::eFifo;
        swapchainInfo.clipped = VK_TRUE;
        swapchainInfo.oldSwapchain = m_swapchain.get();

        vk::ResultValue<vk::UniqueSwapchainKHR> swapchainResult = m_device->createSwapchainKHRUnique(swapchainInfo);
        if (swapchainResult.result != vk::Result::eSuccess)
        {
            throw std::runtime_error("Failed to create swapchain");
        }
        vk::UniqueSwapchainKHR newSwapchain = std::move(swapchainResult.value);
        if (swapchainInfo.oldSwapchain)
        {
            m_device->destroySwapchainKHR(swapchainInfo.oldSwapchain);
        }

        m_swapchain = std::move(newSwapchain);
        m_swapchainFormat = chosenFormat.format;
        m_swapchainTextureFormat = fromVkFormat(m_swapchainFormat);
        m_info.preferredColorFormat = m_swapchainTextureFormat;
        m_swapchainExtent = extent;

        m_swapchainImages = m_device->getSwapchainImagesKHR(m_swapchain.get()).value;
        ensureSwapchainSyncObjects(static_cast<uint32_t>(m_swapchainImages.size()));
        releaseSwapchainResources();
        m_swapchainImageLayouts.resize(m_swapchainImages.size(), ImageLayout::UNDEFINED);
        for (size_t i = 0; i < m_swapchainImages.size(); ++i)
        {
            const vk::Image &image = m_swapchainImages[i];
            vk::ImageViewCreateInfo viewInfo{};
            viewInfo.image = image;
            viewInfo.viewType = vk::ImageViewType::e2D;
            viewInfo.format = m_swapchainFormat;
            viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.layerCount = 1;
            vk::ResultValue<vk::UniqueImageView> viewResult = m_device->createImageViewUnique(viewInfo);
            if (viewResult.result != vk::Result::eSuccess)
            {
                throw std::runtime_error("Failed to create swapchain image view");
            }

            Image imageHandle{};
            imageHandle.h = g_imageHandleCounter.fetch_add(1, std::memory_order_relaxed);

            ImageResource imageResource{};
            imageResource.desc.width = extent.width;
            imageResource.desc.height = extent.height;
            imageResource.desc.depth = 1;
            imageResource.desc.mipLevels = 1;
            imageResource.desc.arrayLayers = 1;
            imageResource.desc.format = m_swapchainTextureFormat;
            imageResource.desc.usage = TextureUsage::COLOR_ATTACHMENT;
            imageResource.image = image;
            imageResource.allocation = nullptr;
            imageResource.swapchain = true;
            m_images.emplace(imageHandle.h, imageResource);
            m_swapchainImageHandles.push_back(imageHandle);

            ImageView viewHandle{};
            viewHandle.h = g_imageViewHandleCounter.fetch_add(1, std::memory_order_relaxed);

            ImageViewDesc viewDesc{};
            viewDesc.type = ImageViewType::TYPE_2D;
            viewDesc.format = m_swapchainTextureFormat;
            viewDesc.range.aspect = AspectMask::COLOR;
            viewDesc.range.levelCount = 1;
            viewDesc.range.layerCount = 1;

            ImageViewResource viewResource{};
            viewResource.desc = viewDesc;
            viewResource.image = imageHandle;
            viewResource.view = std::move(viewResult.value);
            viewResource.swapchain = true;
            m_imageViews.emplace(viewHandle.h, std::move(viewResource));
            m_swapchainImageViewHandles.push_back(viewHandle);
        }
    }

    void VulkanDevice::createSyncObjects()
    {
        if (!m_device)
        {
            return;
        }

        m_imageAvailableSemaphores.clear();
        m_renderFinishedSemaphores.clear();
        m_nextAcquireSemaphore = 0;
        m_pendingAcquireSemaphore = VK_NULL_HANDLE;
        m_pendingRenderFinishedSemaphore = VK_NULL_HANDLE;
        m_activeRenderFinishedSemaphore = VK_NULL_HANDLE;

        ensureAcquireSemaphorePool(2);

        vk::SemaphoreTypeCreateInfo timelineInfo{};
        timelineInfo.semaphoreType = vk::SemaphoreType::eTimeline;
        timelineInfo.initialValue = 0;

        vk::SemaphoreCreateInfo timelineCreateInfo{};
        timelineCreateInfo.pNext = &timelineInfo;
        vk::ResultValue<vk::UniqueSemaphore> timelineRes = m_device->createSemaphoreUnique(timelineCreateInfo);
        if (timelineRes.result != vk::Result::eSuccess)
        {
            throw std::runtime_error("Failed to create timeline semaphore");
        }
        m_graphicsTimelineSemaphore = std::move(timelineRes.value);
        m_graphicsTimelineValue = 0;
    }

    vk::UniqueSemaphore VulkanDevice::createBinarySemaphore()
    {
        vk::SemaphoreCreateInfo semaphoreInfo{};
        vk::ResultValue<vk::UniqueSemaphore> result = m_device->createSemaphoreUnique(semaphoreInfo);
        ensureSuccess(result.result, "Failed to create semaphore");
        return std::move(result.value);
    }

    void VulkanDevice::ensureAcquireSemaphorePool(uint32_t count)
    {
        const uint32_t desired = std::max(count, 1u);
        while (m_imageAvailableSemaphores.size() < desired)
        {
            m_imageAvailableSemaphores.push_back(createBinarySemaphore());
        }
        if (!m_imageAvailableSemaphores.empty() && m_nextAcquireSemaphore >= m_imageAvailableSemaphores.size())
        {
            m_nextAcquireSemaphore = 0;
        }
    }

    void VulkanDevice::ensureSwapchainSyncObjects(uint32_t imageCount)
    {
        if (imageCount == 0)
        {
            return;
        }

        ensureAcquireSemaphorePool(imageCount);
        while (m_renderFinishedSemaphores.size() < imageCount)
        {
            m_renderFinishedSemaphores.push_back(createBinarySemaphore());
        }
    }

    vk::Semaphore VulkanDevice::nextAcquireSemaphore()
    {
        if (m_imageAvailableSemaphores.empty())
        {
            ensureAcquireSemaphorePool(1);
        }

        const uint32_t poolSize = static_cast<uint32_t>(m_imageAvailableSemaphores.size());
        const uint32_t index = m_nextAcquireSemaphore % poolSize;
        m_nextAcquireSemaphore = (index + 1) % poolSize;
        return m_imageAvailableSemaphores[index].get();
    }

    vk::Semaphore VulkanDevice::renderFinishedSemaphoreForImage(uint32_t imageIndex)
    {
        if (m_renderFinishedSemaphores.size() <= imageIndex)
        {
            ensureSwapchainSyncObjects(imageIndex + 1);
        }
        return m_renderFinishedSemaphores[imageIndex].get();
    }

    void VulkanDevice::setupQueues(const device::RequestQueues &queues)
    {
        (void)queues;
        if (!m_device)
        {
            return;
        }

        m_graphicsQueue = m_device->getQueue(m_graphicsQueueFamily, 0);
        m_graphicsQueueWrapper = std::make_unique<VulkanQueue>(*this, QueueType::GRAPHICS, m_graphicsQueueFamily, m_graphicsQueue);
    }

    void VulkanDevice::enqueueGarbage(uint64_t timelineValue, std::function<void()> &&deleter)
    {
        if (!deleter)
        {
            return;
        }

        if (!m_graphicsTimelineSemaphore || timelineValue == 0)
        {
            deleter();
            return;
        }

        std::lock_guard<std::mutex> lock(m_garbageMutex);
        m_deferredFrees.push_back(DeferredFree{timelineValue, std::move(deleter)});
    }

    uint64_t VulkanDevice::queryCompletedTimeline() const
    {
        if (!m_graphicsTimelineSemaphore || !m_device)
        {
            return m_graphicsTimelineValue;
        }
        const vk::ResultValue<uint64_t> valueResult = m_device->getSemaphoreCounterValueKHR(m_graphicsTimelineSemaphore.get());
        if (valueResult.result != vk::Result::eSuccess)
        {
            throw std::runtime_error("Failed to query timeline semaphore");
        }
        return valueResult.value;
    }

    void VulkanDevice::waitForTimeline(uint64_t value)
    {
        if (!value || !m_graphicsTimelineSemaphore || !m_device)
        {
            return;
        }
        const uint64_t completed = queryCompletedTimeline();
        if (completed >= value)
        {
            return;
        }

        vk::Semaphore semaphore = m_graphicsTimelineSemaphore.get();
        vk::SemaphoreWaitInfo waitInfo{};
        waitInfo.semaphoreCount = 1;
        waitInfo.pSemaphores = &semaphore;
        waitInfo.pValues = &value;
        ensureSuccess(m_device->waitSemaphoresKHR(waitInfo, std::numeric_limits<uint64_t>::max()),
                      "Failed to wait for timeline semaphore");
    }

    void VulkanDevice::collectGarbage(bool force)
    {
        const uint64_t completed = force ? std::numeric_limits<uint64_t>::max() : queryCompletedTimeline();
        std::deque<DeferredFree> pending{};
        {
            std::lock_guard<std::mutex> lock(m_garbageMutex);
            while (!m_deferredFrees.empty() && m_deferredFrees.front().timelineValue <= completed)
            {
                pending.emplace_back(std::move(m_deferredFrees.front()));
                m_deferredFrees.pop_front();
            }
            if (force)
            {
                while (!m_deferredFrees.empty())
                {
                    pending.emplace_back(std::move(m_deferredFrees.front()));
                    m_deferredFrees.pop_front();
                }
            }
        }

        for (DeferredFree &entry : pending)
        {
            if (entry.deleter)
            {
                entry.deleter();
            }
        }

        processFrameResets(completed);
    }

    vk::ImageView VulkanDevice::getVkImageView(ImageView view) const
    {
        const ImageViewResource &resource = const_cast<VulkanDevice *>(this)->requireImageView(view);
        return resource.view.get();
    }

    vk::DescriptorType VulkanDevice::toVkDescriptorType(DescType type) const
    {
        return toVkDescriptorTypeInternal(type);
    }

    VulkanDevice::SamplerResource &VulkanDevice::requireSampler(Sampler sampler)
    {
        if (!sampler.h)
        {
            throw std::runtime_error("Invalid sampler handle");
        }
        std::lock_guard<std::mutex> lock(m_resourceMutex);
        std::unordered_map<uint64_t, SamplerResource>::iterator it = m_samplers.find(sampler.h);
        if (it == m_samplers.end())
        {
            throw std::runtime_error("Unknown sampler handle");
        }
        return it->second;
    }

    VulkanDevice::DescriptorSetLayoutResource &VulkanDevice::requireDescriptorSetLayout(DescriptorSetLayout layout)
    {
        if (!layout.h)
        {
            throw std::runtime_error("Invalid descriptor set layout handle");
        }
        std::lock_guard<std::mutex> lock(m_resourceMutex);
        std::unordered_map<uint64_t, DescriptorSetLayoutResource>::iterator it = m_descriptorSetLayouts.find(layout.h);
        if (it == m_descriptorSetLayouts.end())
        {
            throw std::runtime_error("Unknown descriptor set layout handle");
        }
        return it->second;
    }

    VulkanDevice::DescriptorSetResource &VulkanDevice::requireDescriptorSet(DescriptorSet set)
    {
        if (!set.h)
        {
            throw std::runtime_error("Invalid descriptor set handle");
        }
        std::lock_guard<std::mutex> lock(m_resourceMutex);
        std::unordered_map<uint64_t, DescriptorSetResource>::iterator it = m_descriptorSets.find(set.h);
        if (it == m_descriptorSets.end())
        {
            throw std::runtime_error("Unknown descriptor set handle");
        }
        return it->second;
    }

    vk::DescriptorSetLayout VulkanDevice::getVkDescriptorSetLayoutHandle(DescriptorSetLayout layout)
    {
        DescriptorSetLayoutResource &resource = requireDescriptorSetLayout(layout);
        return resource.layout.get();
    }

    vk::DescriptorSet VulkanDevice::getVkDescriptorSetHandle(DescriptorSet set)
    {
        DescriptorSetResource &resource = requireDescriptorSet(set);
        return resource.set;
    }

    vk::PipelineLayout VulkanDevice::getVkPipelineLayout(PipelineLayout layout)
    {
        if (!layout.h)
        {
            throw std::runtime_error("Invalid pipeline layout handle");
        }
        std::lock_guard<std::mutex> lock(m_resourceMutex);
        std::unordered_map<uint64_t, PipelineLayoutResource>::iterator it = m_pipelineLayouts.find(layout.h);
        if (it == m_pipelineLayouts.end())
        {
            throw std::runtime_error("Unknown pipeline layout handle");
        }
        return it->second.layout.get();
    }

    vk::Pipeline VulkanDevice::getVkPipeline(Pipeline pipeline, vk::PipelineBindPoint &bindPoint)
    {
        if (!pipeline.h)
        {
            throw std::runtime_error("Invalid pipeline handle");
        }
        std::lock_guard<std::mutex> lock(m_resourceMutex);
        std::unordered_map<uint64_t, PipelineResource>::iterator it = m_pipelines.find(pipeline.h);
        if (it == m_pipelines.end())
        {
            throw std::runtime_error("Unknown pipeline handle");
        }
        bindPoint = it->second.bindPoint;
        return it->second.pipeline.get();
    }

    const VulkanDevice::ShaderResource &VulkanDevice::getShaderResource(Shader shader) const
    {
        if (!shader.h)
        {
            throw std::runtime_error("Invalid shader handle");
        }
        std::lock_guard<std::mutex> lock(m_resourceMutex);
        std::unordered_map<uint64_t, ShaderResource>::const_iterator it = m_shaders.find(shader.h);
        if (it == m_shaders.end())
        {
            throw std::runtime_error("Unknown shader handle");
        }
        return it->second;
    }

    const ShaderLayout &VulkanDevice::getShaderLayout(Shader shader) const
    {
        const ShaderResource &resource = getShaderResource(shader);
        return resource.layout;
    }

    VulkanDevice::BufferResource &VulkanDevice::requireBuffer(Buffer buffer)
    {
        if (!buffer.h)
        {
            throw std::runtime_error("Invalid buffer handle");
        }
        std::lock_guard<std::mutex> lock(m_resourceMutex);
        std::unordered_map<uint64_t, BufferResource>::iterator it = m_buffers.find(buffer.h);
        if (it == m_buffers.end())
        {
            throw std::runtime_error("Unknown buffer handle");
        }
        return it->second;
    }

    VulkanDevice::ImageResource &VulkanDevice::requireImage(Image image)
    {
        if (!image.h)
        {
            throw std::runtime_error("Invalid image handle");
        }
        std::lock_guard<std::mutex> lock(m_resourceMutex);
        std::unordered_map<uint64_t, ImageResource>::iterator it = m_images.find(image.h);
        if (it == m_images.end())
        {
            throw std::runtime_error("Unknown image handle");
        }
        return it->second;
    }

    VulkanDevice::ImageViewResource &VulkanDevice::requireImageView(ImageView view)
    {
        if (!view.h)
        {
            throw std::runtime_error("Invalid image view handle");
        }
        std::lock_guard<std::mutex> lock(m_resourceMutex);
        std::unordered_map<uint64_t, ImageViewResource>::iterator it = m_imageViews.find(view.h);
        if (it == m_imageViews.end())
        {
            throw std::runtime_error("Unknown image view handle");
        }
        return it->second;
    }

    device::FormatCaps VulkanDevice::queryFormatCaps(TextureFormat fmt) const
    {
        device::FormatCaps caps{};
        if (!m_physicalDevice)
        {
            return caps;
        }

        const vk::Format vkFormat = toVkFormat(fmt);
        if (vkFormat == vk::Format::eUndefined)
        {
            return caps;
        }

        const vk::FormatProperties formatProps = m_physicalDevice.getFormatProperties(vkFormat);
        const vk::FormatFeatureFlags features = formatProps.optimalTilingFeatures;

        caps.sampleable = static_cast<bool>(features & vk::FormatFeatureFlagBits::eSampledImage);
        caps.linearFilter = static_cast<bool>(features & vk::FormatFeatureFlagBits::eSampledImageFilterLinear);
        caps.colorAttachment = static_cast<bool>(features & vk::FormatFeatureFlagBits::eColorAttachment) ||
                               static_cast<bool>(features & vk::FormatFeatureFlagBits::eColorAttachmentBlend);
        caps.depthAttachment = static_cast<bool>(features & vk::FormatFeatureFlagBits::eDepthStencilAttachment);
        caps.storage = static_cast<bool>(features & vk::FormatFeatureFlagBits::eStorageImage);
        caps.blitSrc = static_cast<bool>(features & vk::FormatFeatureFlagBits::eBlitSrc);
        caps.blitDst = static_cast<bool>(features & vk::FormatFeatureFlagBits::eBlitDst);

        const vk::PhysicalDeviceProperties props = m_physicalDevice.getProperties();
        vk::SampleCountFlags sampleFlags{};
        if (caps.colorAttachment)
        {
            sampleFlags |= props.limits.framebufferColorSampleCounts;
        }
        if (caps.depthAttachment)
        {
            sampleFlags |= props.limits.framebufferDepthSampleCounts;
        }
        if (!sampleFlags)
        {
            sampleFlags = props.limits.framebufferNoAttachmentsSampleCounts;
        }

        const vk::SampleCountFlagBits sampleSteps[6] = {
            vk::SampleCountFlagBits::e1, vk::SampleCountFlagBits::e2, vk::SampleCountFlagBits::e4,
            vk::SampleCountFlagBits::e8, vk::SampleCountFlagBits::e16, vk::SampleCountFlagBits::e32};
        for (int i = 0; i < 6; ++i)
        {
            caps.msaa[i] = static_cast<bool>(sampleFlags & sampleSteps[i]);
        }

        return caps;
    }

    Buffer VulkanDevice::createBuffer(const BufferDesc &desc)
    {
        if (!m_device.get())
        {
            throw std::runtime_error("Logical Vulkan device is not initialized");
        }
        if (!m_allocator)
        {
            throw std::runtime_error("VMA allocator not initialized");
        }
        if (desc.size == 0)
        {
            throw std::runtime_error("Buffer size must be greater than zero");
        }

        vk::BufferCreateInfo bufferInfo{};
        bufferInfo.size = desc.size;
        bufferInfo.usage = toVkBufferUsage(desc.usage);
        bufferInfo.sharingMode = vk::SharingMode::eExclusive;

        vma::AllocationCreateInfo allocInfo{};
        switch (desc.memory)
        {
        case MemoryUsage::CPU_ONLY:
            allocInfo.usage = vma::MemoryUsage::eCpuOnly;
            allocInfo.flags = vma::AllocationCreateFlagBits::eHostAccessRandom;
            break;
        case MemoryUsage::CPU_TO_GPU:
            allocInfo.usage = vma::MemoryUsage::eCpuToGpu;
            allocInfo.flags = vma::AllocationCreateFlagBits::eHostAccessSequentialWrite;
            break;
        case MemoryUsage::GPU_TO_CPU:
            allocInfo.usage = vma::MemoryUsage::eGpuToCpu;
            allocInfo.flags = vma::AllocationCreateFlagBits::eHostAccessRandom;
            break;
        case MemoryUsage::GPU_ONLY:
        default:
            allocInfo.usage = vma::MemoryUsage::eAutoPreferDevice;
            break;
        }

        vk::Buffer rawBuffer{};
        vma::Allocation allocation{};
        const vk::Result result = m_allocator.createBuffer(&bufferInfo, &allocInfo, &rawBuffer, &allocation, nullptr);
        ensureSuccess(result, "Failed to create Vulkan buffer");

        Buffer buffer{};
        buffer.h = g_bufferHandleCounter.fetch_add(1, std::memory_order_relaxed);

        BufferResource resource{};
        resource.desc = desc;
        resource.buffer = rawBuffer;
        resource.allocation = allocation;

        {
            std::lock_guard<std::mutex> lock(m_resourceMutex);
            m_buffers.emplace(buffer.h, std::move(resource));
        }

        return buffer;
    }

    void VulkanDevice::destroy(Buffer buffer)
    {
        if (!buffer.h)
        {
            return;
        }

        std::lock_guard<std::mutex> lock(m_resourceMutex);
        std::unordered_map<uint64_t, BufferResource>::iterator it = m_buffers.find(buffer.h);
        if (it != m_buffers.end())
        {
            if (m_allocator && it->second.buffer)
            {
                const vk::Buffer bufferHandle = it->second.buffer;
                const vma::Allocation allocation = it->second.allocation;
                const uint64_t retireValue = m_graphicsTimelineValue;
                enqueueGarbage(retireValue, [this, bufferHandle, allocation]()
                               {
                    if (m_allocator && bufferHandle)
                    {
                        m_allocator.destroyBuffer(bufferHandle, allocation);
                    } });
            }
            m_buffers.erase(it);
        }
        collectGarbage();
    }

    Image VulkanDevice::createImage(const ImageDesc &desc)
    {
        if (!m_device)
        {
            throw std::runtime_error("Logical Vulkan device is not initialized");
        }
        if (!m_allocator)
        {
            throw std::runtime_error("VMA allocator not initialized");
        }
        if (desc.mipLevels == 0 || desc.arrayLayers == 0)
        {
            throw std::runtime_error("Image must have at least one mip level and array layer");
        }
        if (desc.width == 0 || desc.height == 0 || desc.depth == 0)
        {
            throw std::runtime_error("Image dimensions must be greater than zero");
        }
        if (desc.depth > 1 && desc.arrayLayers > 1)
        {
            throw std::runtime_error("3D images cannot specify array layers");
        }

        const vk::Format format = toVkFormat(desc.format);
        if (format == vk::Format::eUndefined)
        {
            throw std::runtime_error("Unsupported image format");
        }

        vk::ImageCreateInfo imageInfo{};
        imageInfo.imageType = toVkImageType(desc);
        imageInfo.extent = toVkExtent(desc);
        imageInfo.mipLevels = desc.mipLevels;
        const bool is3D = desc.depth > 1;
        imageInfo.arrayLayers = is3D ? 1u : desc.arrayLayers;
        imageInfo.format = format;
        imageInfo.tiling = vk::ImageTiling::eOptimal;
        imageInfo.initialLayout = vk::ImageLayout::eUndefined;
        imageInfo.usage = toVkImageUsage(desc.usage);
        imageInfo.samples = toVkSampleCount(desc.samples);
        imageInfo.sharingMode = vk::SharingMode::eExclusive;
        imageInfo.flags = {};

        if (!is3D && desc.arrayLayers >= 6)
        {
            if (desc.arrayLayers % 6 != 0)
            {
                throw std::runtime_error("Cube-compatible images require layer counts divisible by 6");
            }
            if (desc.width != desc.height)
            {
                throw std::runtime_error("Cube-compatible images require square dimensions");
            }
            imageInfo.flags |= vk::ImageCreateFlagBits::eCubeCompatible;
        }

        vma::AllocationCreateInfo allocInfo{};
        allocInfo.usage = vma::MemoryUsage::eAutoPreferDevice;

        vk::Image rawImage{};
        vma::Allocation allocation{};
        const vk::Result result = m_allocator.createImage(&imageInfo, &allocInfo, &rawImage, &allocation, nullptr);
        ensureSuccess(result, "Failed to create Vulkan image");

        Image image{};
        image.h = g_imageHandleCounter.fetch_add(1, std::memory_order_relaxed);

        ImageResource resource{};
        resource.desc = desc;
        resource.image = rawImage;
        resource.allocation = allocation;

        {
            std::lock_guard<std::mutex> lock(m_resourceMutex);
            m_images.emplace(image.h, std::move(resource));
        }

        return image;
    }

    void VulkanDevice::destroy(Image image)
    {
        if (!image.h)
        {
            return;
        }

        std::lock_guard<std::mutex> lock(m_resourceMutex);
        std::unordered_map<uint64_t, ImageResource>::iterator it = m_images.find(image.h);
        if (it != m_images.end())
        {
            if (it->second.swapchain)
            {
                m_images.erase(it);
                return;
            }
            if (m_allocator && it->second.image)
            {
                const vk::Image imageHandle = it->second.image;
                const vma::Allocation allocation = it->second.allocation;
                const uint64_t retireValue = m_graphicsTimelineValue;
                enqueueGarbage(retireValue, [this, imageHandle, allocation]()
                               {
                    if (m_allocator && imageHandle)
                    {
                        m_allocator.destroyImage(imageHandle, allocation);
                    } });
            }
            m_images.erase(it);
        }
        collectGarbage();
    }

    ImageView VulkanDevice::createImageView(Image image, const ImageViewDesc &desc)
    {
        if (!m_device)
        {
            throw std::runtime_error("Logical Vulkan device is not initialized");
        }
        if (!image.h)
        {
            throw std::runtime_error("Invalid image handle");
        }

        std::lock_guard<std::mutex> lock(m_resourceMutex);
        const std::unordered_map<uint64_t, ImageResource>::const_iterator imageIt = m_images.find(image.h);
        if (imageIt == m_images.end())
        {
            throw std::runtime_error("Attempted to create a view for an unknown image handle");
        }

        const ImageResource &imageResource = imageIt->second;
        ImageViewDesc viewDesc = desc;
        if (viewDesc.format == TextureFormat::UNKNOWN)
        {
            viewDesc.format = imageResource.desc.format;
        }

        const vk::Format format = toVkFormat(viewDesc.format);
        if (format == vk::Format::eUndefined)
        {
            throw std::runtime_error("Unsupported image view format");
        }

        const bool imageIs3D = imageResource.desc.depth > 1;
        ImageViewType requestedType = viewDesc.type;
        const vk::ImageViewType viewType = toVkImageViewType(requestedType);
        if (viewType == vk::ImageViewType::e3D && !imageIs3D)
        {
            throw std::runtime_error("3D image views require 3D images");
        }
        if ((viewType == vk::ImageViewType::eCube || viewType == vk::ImageViewType::eCubeArray) && imageIs3D)
        {
            throw std::runtime_error("Cube image views require 2D array images");
        }
        if (imageIs3D && viewType != vk::ImageViewType::e3D)
        {
            throw std::runtime_error("3D images only support 3D image views in this implementation");
        }

        const uint32_t maxMipLevels = imageResource.desc.mipLevels;
        const uint32_t maxArrayLayers = imageIs3D ? 1u : imageResource.desc.arrayLayers;

        ImageSubresourceRange range = viewDesc.range;
        if (range.baseMip >= maxMipLevels)
        {
            throw std::runtime_error("Image view base mip exceeds image mip levels");
        }
        if (range.levelCount == 0 || range.baseMip + range.levelCount > maxMipLevels)
        {
            range.levelCount = maxMipLevels - range.baseMip;
        }

        if (range.baseArrayLayer >= maxArrayLayers)
        {
            throw std::runtime_error("Image view base array layer exceeds available layers");
        }
        if (range.layerCount == 0 || range.baseArrayLayer + range.layerCount > maxArrayLayers)
        {
            range.layerCount = maxArrayLayers - range.baseArrayLayer;
        }

        if ((viewType == vk::ImageViewType::eCube || viewType == vk::ImageViewType::eCubeArray) &&
            (range.layerCount % 6 != 0 || imageResource.desc.width != imageResource.desc.height ||
             (range.baseArrayLayer % 6) != 0))
        {
            throw std::runtime_error("Cube image views require square images and layer counts divisible by 6");
        }

        vk::ImageAspectFlags aspectFlags = toVkAspectFlags(range.aspect);
        if (range.aspect == AspectMask::COLOR && (isDepthFormat(viewDesc.format) || hasStencil(viewDesc.format)))
        {
            aspectFlags = defaultAspectFlags(viewDesc.format);
        }
        if (!aspectFlags)
        {
            aspectFlags = defaultAspectFlags(viewDesc.format);
        }

        if (!aspectFlags)
        {
            throw std::runtime_error("Invalid aspect mask for image view");
        }

        vk::ImageViewCreateInfo viewInfo{};
        viewInfo.image = imageResource.image;
        viewInfo.viewType = viewType;
        viewInfo.format = format;
        viewInfo.components = vk::ComponentMapping{
            vk::ComponentSwizzle::eIdentity,
            vk::ComponentSwizzle::eIdentity,
            vk::ComponentSwizzle::eIdentity,
            vk::ComponentSwizzle::eIdentity};
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = range.baseMip;
        viewInfo.subresourceRange.levelCount = range.levelCount;
        viewInfo.subresourceRange.baseArrayLayer = range.baseArrayLayer;
        viewInfo.subresourceRange.layerCount = range.layerCount;

        vk::ResultValue<vk::UniqueImageView> viewHandle = m_device->createImageViewUnique(viewInfo);
        ensureSuccess(viewHandle.result, "Failed to create image view");

        ImageView view{};
        view.h = g_imageViewHandleCounter.fetch_add(1, std::memory_order_relaxed);

        ImageViewResource resource{};
        resource.desc = viewDesc;
        resource.desc.range = range;
        resource.desc.range.aspect = fromVkAspectFlags(aspectFlags);
        resource.image = image;
        resource.view = std::move(viewHandle.value);

        m_imageViews.emplace(view.h, std::move(resource));
        return view;
    }

    void VulkanDevice::destroy(ImageView view)
    {
        if (!view.h)
        {
            return;
        }

        std::lock_guard<std::mutex> lock(m_resourceMutex);
        auto it = m_imageViews.find(view.h);
        if (it == m_imageViews.end())
        {
            return;
        }
        if (it->second.swapchain)
        {
            m_imageViews.erase(it);
            return;
        }
        m_imageViews.erase(it);
    }

    Sampler VulkanDevice::createSampler(const SamplerDesc &desc)
    {
        if (!m_device)
        {
            throw std::runtime_error("Logical Vulkan device is not initialized");
        }

        vk::SamplerCreateInfo samplerInfo{};
        samplerInfo.magFilter = toVkFilter(desc.magFilter);
        samplerInfo.minFilter = toVkFilter(desc.minFilter);
        samplerInfo.mipmapMode = toVkMipmapMode(desc.mipmapMode);
        samplerInfo.addressModeU = toVkAddressMode(desc.addressU);
        samplerInfo.addressModeV = toVkAddressMode(desc.addressV);
        samplerInfo.addressModeW = toVkAddressMode(desc.addressW);
        samplerInfo.mipLodBias = desc.lodBias;
        samplerInfo.minLod = desc.minLod;
        samplerInfo.maxLod = std::max(desc.maxLod, desc.minLod);
        samplerInfo.borderColor = toVkBorderColor(desc.borderColor);
        samplerInfo.unnormalizedCoordinates = VK_FALSE;

        const bool supportsAniso = m_info.supportedFeatures.samplerAnisotropy;
        samplerInfo.anisotropyEnable = desc.anisotropyEnable && supportsAniso;
        const float requestedAniso = std::max(desc.maxAnisotropy, 1.0f);
        samplerInfo.maxAnisotropy = samplerInfo.anisotropyEnable
                                        ? std::min(requestedAniso, static_cast<float>(m_info.limits.maxSamplerAnisotropy))
                                        : 1.0f;

        samplerInfo.compareEnable = desc.compareEnable;
        samplerInfo.compareOp = toVkCompareOp(desc.compareOp);

        vk::ResultValue<vk::UniqueSampler> samplerHandle = m_device->createSamplerUnique(samplerInfo);
        ensureSuccess(samplerHandle.result, "Failed to create sampler");

        Sampler sampler{};
        sampler.h = g_samplerHandleCounter.fetch_add(1, std::memory_order_relaxed);

        SamplerResource resource{};
        resource.desc = desc;
        resource.sampler = std::move(samplerHandle.value);

        std::lock_guard<std::mutex> lock(m_resourceMutex);
        m_samplers.emplace(sampler.h, std::move(resource));
        return sampler;
    }

    void VulkanDevice::destroy(Sampler sampler)
    {
        if (!sampler.h)
        {
            return;
        }

        std::lock_guard<std::mutex> lock(m_resourceMutex);
        m_samplers.erase(sampler.h);
    }

    Shader VulkanDevice::createShader(const ShaderDesc &desc)
    {
        if (!m_device)
        {
            throw std::runtime_error("Logical Vulkan device is not initialized");
        }
        if (!desc.code || desc.codeSize == 0)
        {
            throw std::runtime_error("Shader code buffer is empty");
        }
        if ((desc.codeSize % 4) != 0)
        {
            throw std::runtime_error("Shader code size must be a multiple of 4 bytes");
        }
        if (desc.stage == ShaderStage::NONE)
        {
            throw std::runtime_error("Shader stage must be specified");
        }

        const vk::ShaderStageFlagBits stage = toVkShaderStage(desc.stage);

        std::span<const uint32_t> words(static_cast<const uint32_t *>(desc.code), desc.codeSize / sizeof(uint32_t));
        ShaderLayout layout = ShaderCache::loadOrCreate(words, desc.entryPoint ? desc.entryPoint : "main");

        vk::ShaderModuleCreateInfo moduleInfo{};
        moduleInfo.codeSize = desc.codeSize;
        moduleInfo.pCode = words.data();

        vk::ResultValue<vk::UniqueShaderModule> module = m_device->createShaderModuleUnique(moduleInfo);
        ensureSuccess(module.result, "Failed to create shader module");

        Shader shader{};
        shader.h = g_shaderHandleCounter.fetch_add(1, std::memory_order_relaxed);

        ShaderResource resource{};
        resource.stage = desc.stage;
        resource.entryPoint = desc.entryPoint ? desc.entryPoint : "main";
        resource.module = std::move(module.value);
        resource.layout = std::move(layout);

        std::lock_guard<std::mutex> lock(m_resourceMutex);
        m_shaders.emplace(shader.h, std::move(resource));
        return shader;
    }

    void VulkanDevice::destroy(Shader shader)
    {
        if (!shader.h)
        {
            return;
        }

        std::lock_guard<std::mutex> lock(m_resourceMutex);
        m_shaders.erase(shader.h);
    }


    PipelineLayout VulkanDevice::createPipelineLayout(const ShaderLayout &layoutInfo)
    {
        if (!m_device)
        {
            throw std::runtime_error("Logical Vulkan device is not initialized");
        }

        std::vector<vk::DescriptorSetLayout> vkSetLayouts;
        std::vector<DescriptorSetLayout> descriptorHandles;
        if (!layoutInfo.sets.empty())
        {
            uint32_t maxSet = 0;
            for (const SetLayoutInfo &setInfo : layoutInfo.sets)
            {
                maxSet = std::max(maxSet, setInfo.set);
            }
            vkSetLayouts.resize(maxSet + 1);
            descriptorHandles.resize(maxSet + 1);
            for (const SetLayoutInfo &setInfo : layoutInfo.sets)
            {
                DescriptorSetLayout setLayout = createDescriptorSetLayout(setInfo);
                vkSetLayouts[setInfo.set] = getVkDescriptorSetLayoutHandle(setLayout);
                descriptorHandles[setInfo.set] = setLayout;
            }
            for (uint32_t set = 0; set <= maxSet; ++set)
            {
                if (!vkSetLayouts[set])
                {
                    SetLayoutInfo emptyInfo{};
                    emptyInfo.set = set;
                    DescriptorSetLayout placeholder = createDescriptorSetLayout(emptyInfo);
                    vkSetLayouts[set] = getVkDescriptorSetLayoutHandle(placeholder);
                    descriptorHandles[set] = placeholder;
                }
            }
        }

        std::vector<vk::PushConstantRange> ranges;
        ranges.reserve(layoutInfo.pushConstants.size());
        for (const PushConstantRangeInfo &rangeInfo : layoutInfo.pushConstants)
        {
            vk::PushConstantRange range{};
            range.offset = rangeInfo.offset;
            range.size = rangeInfo.size;
            range.stageFlags = toVkShaderStageFlags(static_cast<ShaderStage>(rangeInfo.stages));
            ranges.push_back(range);
        }

        vk::PipelineLayoutCreateInfo layoutCreateInfo{};
        layoutCreateInfo.setLayoutCount = static_cast<uint32_t>(vkSetLayouts.size());
        layoutCreateInfo.pSetLayouts = vkSetLayouts.data();
        layoutCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(ranges.size());
        layoutCreateInfo.pPushConstantRanges = ranges.data();

        vk::ResultValue<vk::UniquePipelineLayout> layout = m_device->createPipelineLayoutUnique(layoutCreateInfo);
        ensureSuccess(layout.result, "Failed to create pipeline layout");

        PipelineLayout handle{};
        handle.h = g_pipelineLayoutHandleCounter.fetch_add(1, std::memory_order_relaxed);

        PipelineLayoutResource resource{};
        resource.layout = std::move(layout.value);
        resource.layoutInfo = layoutInfo;
        resource.setLayouts = std::move(descriptorHandles);

        {
            std::lock_guard<std::mutex> lock(m_resourceMutex);
            m_pipelineLayouts.emplace(handle.h, std::move(resource));
        }

        return handle;
    }

    void VulkanDevice::destroy(PipelineLayout layout)
    {
        if (!layout.h)
        {
            return;
        }

        std::lock_guard<std::mutex> lock(m_resourceMutex);
        auto it = m_pipelineLayouts.find(layout.h);
        if (it != m_pipelineLayouts.end())
        {
            for (const DescriptorSetLayout &setLayout : it->second.setLayouts)
            {
                m_descriptorSetLayouts.erase(setLayout.h);
            }
            m_pipelineLayouts.erase(it);
        }
    }

    const ShaderLayout &VulkanDevice::getPipelineLayoutInfo(PipelineLayout layout) const
    {
        if (!layout.h)
        {
            throw std::runtime_error("Invalid pipeline layout handle");
        }
        std::lock_guard<std::mutex> lock(m_resourceMutex);
        std::unordered_map<uint64_t, PipelineLayoutResource>::const_iterator it = m_pipelineLayouts.find(layout.h);
        if (it == m_pipelineLayouts.end())
        {
            throw std::runtime_error("Unknown pipeline layout handle");
        }
        return it->second.layoutInfo;
    }

    DescriptorSetLayout VulkanDevice::createDescriptorSetLayout(const SetLayoutInfo &info)
    {
        if (!m_device)
        {
            throw std::runtime_error("Logical device is not initialized");
        }
        SetLayoutInfo sortedInfo = info;
        std::sort(sortedInfo.bindings.begin(), sortedInfo.bindings.end(),
                  [](const BindingInfo &a, const BindingInfo &b) { return a.binding < b.binding; });
        std::vector<vk::DescriptorSetLayoutBinding> bindings;
        bindings.reserve(sortedInfo.bindings.size());
        std::vector<vk::DescriptorBindingFlags> bindingFlags;
        bindingFlags.reserve(sortedInfo.bindings.size());
        std::vector<uint32_t> descriptorCounts;
        descriptorCounts.reserve(sortedInfo.bindings.size());

        bool updateAfterBind = false;
        for (const BindingInfo &bindingInfo : sortedInfo.bindings)
        {
            vk::DescriptorSetLayoutBinding vkBinding = makeDescriptorBinding(bindingInfo);
            bindings.push_back(vkBinding);
            descriptorCounts.push_back(vkBinding.descriptorCount);

            vk::DescriptorBindingFlags flags{};
            if (bindingInfo.bindless)
            {
                flags |= vk::DescriptorBindingFlagBits::ePartiallyBound;
                flags |= vk::DescriptorBindingFlagBits::eUpdateAfterBind;
                updateAfterBind = true;
            }
            bindingFlags.push_back(flags);
        }

        vk::DescriptorSetLayoutBindingFlagsCreateInfo bindingFlagsInfo{};
        if (!bindingFlags.empty())
        {
            bindingFlagsInfo.bindingCount = static_cast<uint32_t>(bindingFlags.size());
            bindingFlagsInfo.pBindingFlags = bindingFlags.data();
        }

        vk::DescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();
        layoutInfo.pNext = bindingFlags.empty() ? nullptr : &bindingFlagsInfo;
        if (updateAfterBind)
        {
            layoutInfo.flags |= vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool;
        }

        vk::ResultValue<vk::UniqueDescriptorSetLayout> layout = m_device->createDescriptorSetLayoutUnique(layoutInfo);
        ensureSuccess(layout.result, "Failed to create descriptor set layout");

        DescriptorSetLayout handle{};
        handle.h = g_descriptorSetLayoutHandleCounter.fetch_add(1, std::memory_order_relaxed);
        handle.set = sortedInfo.set;

        DescriptorSetLayoutResource resource{};
        resource.info = std::move(sortedInfo);
        resource.layout = std::move(layout.value);
        resource.updateAfterBind = updateAfterBind;
        resource.descriptorCounts = std::move(descriptorCounts);

        std::lock_guard<std::mutex> lock(m_resourceMutex);
        m_descriptorSetLayouts.emplace(handle.h, std::move(resource));
        return handle;
    }

    void VulkanDevice::destroy(DescriptorSetLayout layout)
    {
        if (!layout.h)
        {
            return;
        }
        std::lock_guard<std::mutex> lock(m_resourceMutex);
        m_descriptorSetLayouts.erase(layout.h);
    }

    DescriptorSetLayout VulkanDevice::getDescriptorSetLayout(PipelineLayout layout, DescSetIndex set) const
    {
        if (!layout.h)
        {
            return {};
        }
        std::lock_guard<std::mutex> lock(m_resourceMutex);
        std::unordered_map<uint64_t, PipelineLayoutResource>::const_iterator it = m_pipelineLayouts.find(layout.h);
        if (it == m_pipelineLayouts.end())
        {
            throw std::runtime_error("Unknown pipeline layout handle");
        }
        for (const DescriptorSetLayout &handle : it->second.setLayouts)
        {
            if (handle.set == set)
            {
                auto layoutIt = m_descriptorSetLayouts.find(handle.h);
                if (layoutIt == m_descriptorSetLayouts.end())
                {
                    continue;
                }
                if (layoutIt->second.info.bindings.empty())
                {
                    return {};
                }
                return handle;
            }
        }
        return {};
    }

    VulkanDevice::DescriptorPoolResource &VulkanDevice::acquireDescriptorPool(bool transient, bool updateAfterBind, uint64_t epoch)
    {
        for (DescriptorPoolResource &pool : m_descriptorPools)
        {
            if (pool.transient != transient || pool.updateAfterBind != updateAfterBind)
            {
                continue;
            }
            if (transient)
            {
                if (pool.epoch != 0 && pool.epoch != epoch)
                {
                    continue;
                }
                if (pool.epoch == 0)
                {
                    pool.epoch = epoch;
                }
            }
            if (pool.allocated < pool.maxSets)
            {
                return pool;
            }
        }

        DescriptorPoolResource poolResource{};
        poolResource.transient = transient;
        poolResource.updateAfterBind = updateAfterBind;
        poolResource.maxSets = kDefaultDescriptorPoolMaxSets;
        poolResource.pool = createDescriptorPool(transient, updateAfterBind);
        if (transient)
        {
            poolResource.epoch = epoch;
        }
        m_descriptorPools.push_back(std::move(poolResource));
        return m_descriptorPools.back();
    }

    vk::UniqueDescriptorPool VulkanDevice::createDescriptorPool(bool transient, bool updateAfterBind)
    {
        if (m_basePoolSizes.empty())
        {
            initializeDescriptorPools();
        }
        vk::DescriptorPoolCreateFlags flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
        if (updateAfterBind)
        {
            flags |= vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind;
        }
        vk::DescriptorPoolCreateInfo poolInfo{};
        poolInfo.flags = flags;
        poolInfo.maxSets = kDefaultDescriptorPoolMaxSets;
        poolInfo.poolSizeCount = static_cast<uint32_t>(m_basePoolSizes.size());
        poolInfo.pPoolSizes = m_basePoolSizes.data();
        vk::ResultValue<vk::UniqueDescriptorPool> pool = m_device->createDescriptorPoolUnique(poolInfo);
        ensureSuccess(pool.result, "Failed to create descriptor pool");
        return std::move(pool.value);
    }

    uint64_t VulkanDevice::ensureActiveEpoch()
    {
        if (m_activeFrameEpoch == 0)
        {
            beginFrameEpoch();
        }
        return m_activeFrameEpoch;
    }

    void VulkanDevice::beginFrameEpoch()
    {
        if (m_activeFrameEpoch != 0)
        {
            return;
        }
        m_activeFrameEpoch = ++m_frameEpochCounter;
    }

    void VulkanDevice::enqueueFrameReset(uint64_t epoch, uint64_t timelineValue)
    {
        if (epoch == 0)
        {
            return;
        }
        if (timelineValue == 0)
        {
            timelineValue = m_graphicsTimelineValue;
        }
        m_pendingFrameResets.push_back(FrameResetRequest{epoch, timelineValue});
        m_activeFrameEpoch = 0;
    }

    void VulkanDevice::processFrameResets(uint64_t completedValue)
    {
        while (!m_pendingFrameResets.empty())
        {
            const FrameResetRequest &request = m_pendingFrameResets.front();
            if (completedValue < request.timelineValue)
            {
                break;
            }
            resetTransientPoolsForEpoch(request.epoch);
            m_pendingFrameResets.pop_front();
        }
    }

    void VulkanDevice::resetTransientPoolsForEpoch(uint64_t epoch)
    {
        if (epoch == 0)
        {
            return;
        }
        std::lock_guard<std::mutex> lock(m_resourceMutex);
        for (DescriptorPoolResource &pool : m_descriptorPools)
        {
            if (!pool.transient || pool.epoch != epoch)
            {
                continue;
            }
            if (m_device)
            {
                m_device->resetDescriptorPool(pool.pool.get());
            }
            pool.allocated = 0;
            pool.epoch = 0;
        }

        for (auto it = m_descriptorSets.begin(); it != m_descriptorSets.end();)
        {
            DescriptorSetResource &resource = it->second;
            if (resource.transient && resource.epoch == epoch && resource.pendingRelease)
            {
                it = m_descriptorSets.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    void VulkanDevice::resetAllTransientPools()
    {
        std::lock_guard<std::mutex> lock(m_resourceMutex);
        for (DescriptorPoolResource &pool : m_descriptorPools)
        {
            if (!pool.transient)
            {
                continue;
            }
            if (m_device)
            {
                m_device->resetDescriptorPool(pool.pool.get());
            }
            pool.allocated = 0;
            pool.epoch = 0;
        }

        for (auto it = m_descriptorSets.begin(); it != m_descriptorSets.end();)
        {
            if (it->second.transient)
            {
                it = m_descriptorSets.erase(it);
            }
            else
            {
                ++it;
            }
        }

        m_pendingFrameResets.clear();
        m_activeFrameEpoch = 0;
        m_frameActive = false;
    }

    void VulkanDevice::releaseSwapchainResources()
    {
        std::lock_guard<std::mutex> lock(m_resourceMutex);
        for (const ImageView &handle : m_swapchainImageViewHandles)
        {
            m_imageViews.erase(handle.h);
        }
        for (const Image &handle : m_swapchainImageHandles)
        {
            m_images.erase(handle.h);
        }
        m_swapchainImageViewHandles.clear();
        m_swapchainImageHandles.clear();
        m_swapchainImageLayouts.clear();
    }

    DescriptorSet VulkanDevice::allocateDescriptorSet(DescriptorSetLayout layout, bool transient)
    {
        if (!layout.h)
        {
            throw std::runtime_error("Invalid descriptor set layout handle");
        }

        DescriptorSetLayoutResource &layoutResource = requireDescriptorSetLayout(layout);

        std::lock_guard<std::mutex> lock(m_resourceMutex);
        const uint64_t epoch = transient ? ensureActiveEpoch() : 0;
        DescriptorPoolResource &pool = acquireDescriptorPool(transient, layoutResource.updateAfterBind, epoch);

        vk::DescriptorSetLayout vkLayout = layoutResource.layout.get();
        vk::DescriptorSetAllocateInfo allocInfo{};
        allocInfo.descriptorPool = pool.pool.get();
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &vkLayout;

        vk::ResultValue<std::vector<vk::DescriptorSet>> allocResult = m_device->allocateDescriptorSets(allocInfo);
        ensureSuccess(allocResult.result, "Failed to allocate descriptor set");
        vk::DescriptorSet rawSet = allocResult.value.at(0);
        pool.allocated++;

        DescriptorSet handle{};
        handle.h = g_descriptorSetHandleCounter.fetch_add(1, std::memory_order_relaxed);
        handle.set = layout.set;
        handle.transient = transient;
        handle.epoch = epoch;

        DescriptorSetResource resource{};
        resource.layout = layout;
        resource.set = rawSet;
        resource.pool = pool.pool.get();
        resource.transient = transient;
        resource.epoch = epoch;
        resource.pendingRelease = false;

        m_descriptorSets.emplace(handle.h, std::move(resource));
        return handle;
    }

    void VulkanDevice::free(DescriptorSet set)
    {
        if (!set.h)
        {
            return;
        }
        std::lock_guard<std::mutex> lock(m_resourceMutex);
        std::unordered_map<uint64_t, DescriptorSetResource>::iterator it = m_descriptorSets.find(set.h);
        if (it == m_descriptorSets.end())
        {
            return;
        }
        DescriptorSetResource &resource = it->second;
        if (resource.transient)
        {
            resource.pendingRelease = true;
            return;
        }
        if (resource.pool && resource.set)
        {
            m_device->freeDescriptorSets(resource.pool, resource.set);
            for (DescriptorPoolResource &pool : m_descriptorPools)
            {
                if (pool.pool.get() == resource.pool && pool.allocated > 0)
                {
                    pool.allocated--;
                    break;
                }
            }
        }
        m_descriptorSets.erase(it);
    }

    void VulkanDevice::updateDescriptors(std::span<const DescriptorWriteOperation> writes)
    {
        if (!m_device || writes.empty())
        {
            return;
        }

        std::vector<vk::WriteDescriptorSet> vkWrites;
        vkWrites.reserve(writes.size());
        std::vector<vk::DescriptorImageInfo> imageInfos;
        imageInfos.reserve(writes.size());
        std::vector<vk::DescriptorBufferInfo> bufferInfos;
        bufferInfos.reserve(writes.size());

        for (const DescriptorWriteOperation &write : writes)
        {
            DescriptorSetResource &setResource = requireDescriptorSet(write.set);
            vk::WriteDescriptorSet vkWrite{};
            vkWrite.dstSet = setResource.set;
            vkWrite.dstBinding = write.binding;
            vkWrite.dstArrayElement = write.arrayIndex;
            vkWrite.descriptorCount = 1;
            vkWrite.descriptorType = toVkDescriptorType(write.type);

            if (write.isImage)
            {
                vk::DescriptorImageInfo imageInfo{};
                if (write.type == DescType::Sampler)
                {
                    SamplerResource &samplerRes = requireSampler(write.image.sampler);
                    imageInfo.sampler = samplerRes.sampler.get();
                }
                else
                {
                    const ImageViewResource &viewRes = requireImageView(write.image.view);
                    imageInfo.imageView = viewRes.view.get();
                    imageInfo.imageLayout = toVkImageLayout(write.image.layout);
                    if (write.image.sampler.h)
                    {
                        SamplerResource &samplerRes = requireSampler(write.image.sampler);
                        imageInfo.sampler = samplerRes.sampler.get();
                    }
                }
                imageInfos.push_back(imageInfo);
                vkWrite.pImageInfo = &imageInfos.back();
            }
            else
            {
                BufferResource &bufferRes = requireBuffer(write.buffer.buffer);
                vk::DescriptorBufferInfo bufferInfo{};
                bufferInfo.buffer = bufferRes.buffer;
                bufferInfo.offset = write.buffer.offset;
                const uint64_t maxRange = std::numeric_limits<uint64_t>::max();
                bufferInfo.range = (write.buffer.range == maxRange)
                                       ? (bufferRes.desc.size - bufferInfo.offset)
                                       : write.buffer.range;
                bufferInfos.push_back(bufferInfo);
                vkWrite.pBufferInfo = &bufferInfos.back();
            }

            vkWrites.push_back(vkWrite);
        }

        m_device->updateDescriptorSets(vkWrites, {});
    }

    Pipeline VulkanDevice::createGraphicsPipeline(const GraphicsPipelineDesc &desc, PipelineLayout layout)
    {
        if (!m_device)
        {
            throw std::runtime_error("Logical Vulkan device is not initialized");
        }
        if (!desc.shaders || desc.shaderCount == 0)
        {
            throw std::runtime_error("Graphics pipeline requires at least one shader stage");
        }
        if (desc.bindingCount > 0 && !desc.bindings)
        {
            throw std::runtime_error("Graphics pipeline bindings pointer is null");
        }
        if (desc.attributeCount > 0 && !desc.attributes)
        {
            throw std::runtime_error("Graphics pipeline attributes pointer is null");
        }
        if (desc.colorFormatCount > 0 && !desc.colorFormats)
        {
            throw std::runtime_error("Graphics pipeline color formats pointer is null");
        }
        if (desc.blendAttachmentCount > 0 && !desc.blendAttachments)
        {
            throw std::runtime_error("Graphics pipeline blend attachments pointer is null");
        }

        vk::PipelineLayout vkLayout = getVkPipelineLayout(layout);

        std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
        shaderStages.reserve(desc.shaderCount);
        for (uint32_t i = 0; i < desc.shaderCount; ++i)
        {
            const Shader handle = desc.shaders[i];
            const ShaderResource &shader = getShaderResource(handle);
            vk::PipelineShaderStageCreateInfo stageInfo{};
            stageInfo.stage = toVkShaderStage(shader.stage);
            stageInfo.module = shader.module.get();
            stageInfo.pName = shader.entryPoint.c_str();
            shaderStages.push_back(stageInfo);
        }

        std::vector<vk::VertexInputBindingDescription> bindingDescriptions;
        bindingDescriptions.reserve(desc.bindingCount);
        for (uint32_t i = 0; i < desc.bindingCount; ++i)
        {
            const VertexBinding &binding = desc.bindings[i];
            vk::VertexInputBindingDescription bindingDesc{};
            bindingDesc.binding = binding.binding;
            bindingDesc.stride = binding.stride;
            bindingDesc.inputRate = binding.perInstance ? vk::VertexInputRate::eInstance : vk::VertexInputRate::eVertex;
            bindingDescriptions.push_back(bindingDesc);
        }

        std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;
        attributeDescriptions.reserve(desc.attributeCount);
        for (uint32_t i = 0; i < desc.attributeCount; ++i)
        {
            const VertexAttribute &attrib = desc.attributes[i];
            const vk::Format vkFormat = toVkFormat(attrib.format);
            if (vkFormat == vk::Format::eUndefined)
            {
                throw std::runtime_error("Unsupported vertex attribute format");
            }
            vk::VertexInputAttributeDescription attribDesc{};
            attribDesc.location = attrib.location;
            attribDesc.binding = attrib.binding;
            attribDesc.offset = attrib.offset;
            attribDesc.format = vkFormat;
            attributeDescriptions.push_back(attribDesc);
        }

        vk::PipelineVertexInputStateCreateInfo vertexInput{};
        vertexInput.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
        vertexInput.pVertexBindingDescriptions = bindingDescriptions.data();
        vertexInput.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInput.pVertexAttributeDescriptions = attributeDescriptions.data();

        vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.topology = toVkPrimitiveTopology(desc.topology);
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        vk::PipelineViewportStateCreateInfo viewportState{};
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        vk::PipelineRasterizationStateCreateInfo raster{};
        raster.polygonMode = toVkPolygonMode(desc.raster.polygon);
        raster.cullMode = toVkCullMode(desc.raster.cull);
        raster.frontFace = toVkFrontFace(desc.raster.front);
        raster.depthClampEnable = desc.raster.depthClamp;
        raster.depthBiasEnable = (desc.raster.depthBiasConstant != 0.f) || (desc.raster.depthBiasSlope != 0.f);
        raster.depthBiasConstantFactor = desc.raster.depthBiasConstant;
        raster.depthBiasSlopeFactor = desc.raster.depthBiasSlope;
        raster.lineWidth = 1.0f;

        vk::PipelineMultisampleStateCreateInfo multisample{};
        multisample.rasterizationSamples = toVkSampleCount(desc.msaa.samples);
        multisample.sampleShadingEnable = desc.msaa.sampleShading;
        multisample.minSampleShading = desc.msaa.minSampleShading;

        vk::PipelineDepthStencilStateCreateInfo depthState{};
        depthState.depthTestEnable = desc.depth.depthTest;
        depthState.depthWriteEnable = desc.depth.depthWrite;
        depthState.depthCompareOp = toVkCompareOp(desc.depth.depthCompare);
        depthState.depthBoundsTestEnable = VK_FALSE;
        depthState.stencilTestEnable = desc.depth.stencilTest;

        std::vector<vk::PipelineColorBlendAttachmentState> colorBlendAttachments;
        colorBlendAttachments.reserve(desc.blendAttachmentCount);
        for (uint32_t i = 0; i < desc.blendAttachmentCount; ++i)
        {
            const ColorBlendAttachment &attachment = desc.blendAttachments[i];
            vk::PipelineColorBlendAttachmentState state{};
            state.blendEnable = attachment.enable;
            state.srcColorBlendFactor = toVkBlendFactor(attachment.srcColor);
            state.dstColorBlendFactor = toVkBlendFactor(attachment.dstColor);
            state.colorBlendOp = toVkBlendOp(attachment.colorOp);
            state.srcAlphaBlendFactor = toVkBlendFactor(attachment.srcAlpha);
            state.dstAlphaBlendFactor = toVkBlendFactor(attachment.dstAlpha);
            state.alphaBlendOp = toVkBlendOp(attachment.alphaOp);
            state.colorWriteMask = toVkColorWriteMask(attachment.writeMask);
            colorBlendAttachments.push_back(state);
        }

        vk::PipelineColorBlendStateCreateInfo blendState{};
        blendState.logicOpEnable = VK_FALSE;
        blendState.attachmentCount = static_cast<uint32_t>(colorBlendAttachments.size());
        blendState.pAttachments = colorBlendAttachments.data();

        std::vector<vk::DynamicState> dynamicStates = {
            vk::DynamicState::eViewport,
            vk::DynamicState::eScissor};
        vk::PipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        std::vector<vk::Format> colorFormats(desc.colorFormatCount);
        for (uint32_t i = 0; i < desc.colorFormatCount; ++i)
        {
            colorFormats[i] = toVkFormat(desc.colorFormats[i]);
        }

        vk::PipelineRenderingCreateInfo renderingInfo{};
        renderingInfo.colorAttachmentCount = static_cast<uint32_t>(colorFormats.size());
        renderingInfo.pColorAttachmentFormats = colorFormats.data();
        if (desc.depthFormat != TextureFormat::UNKNOWN)
        {
            renderingInfo.depthAttachmentFormat = toVkFormat(desc.depthFormat);
        }

        vk::GraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.pNext = &renderingInfo;
        pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
        pipelineInfo.pStages = shaderStages.data();
        pipelineInfo.pVertexInputState = &vertexInput;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &raster;
        pipelineInfo.pMultisampleState = &multisample;
        pipelineInfo.pDepthStencilState = desc.depth.depthTest ? &depthState : nullptr;
        pipelineInfo.pColorBlendState = &blendState;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = vkLayout;
        pipelineInfo.renderPass = VK_NULL_HANDLE;
        pipelineInfo.subpass = 0;

        vk::ResultValue<vk::UniquePipeline> pipelineResult = m_device->createGraphicsPipelineUnique({}, pipelineInfo);
        ensureSuccess(pipelineResult.result, "Failed to create graphics pipeline");
        vk::UniquePipeline pipeline = std::move(pipelineResult.value);

        Pipeline handle{};
        handle.h = g_pipelineHandleCounter.fetch_add(1, std::memory_order_relaxed);

        PipelineResource resource{};
        resource.pipeline = std::move(pipeline);
        resource.layout = layout;
        resource.bindPoint = vk::PipelineBindPoint::eGraphics;

        {
            std::lock_guard<std::mutex> lock(m_resourceMutex);
            m_pipelines.emplace(handle.h, std::move(resource));
        }

        return handle;
    }

    Pipeline VulkanDevice::createComputePipeline(const ComputePipelineDesc &desc, PipelineLayout layout)
    {
        if (!m_device)
        {
            throw std::runtime_error("Logical Vulkan device is not initialized");
        }
        if (!desc.shader)
        {
            throw std::runtime_error("Compute pipeline requires a shader handle");
        }

        vk::PipelineLayout vkLayout = getVkPipelineLayout(layout);
        const ShaderResource &shader = getShaderResource(*desc.shader);

        vk::PipelineShaderStageCreateInfo stageInfo{};
        stageInfo.stage = toVkShaderStage(shader.stage);
        stageInfo.module = shader.module.get();
        stageInfo.pName = shader.entryPoint.c_str();

        vk::ComputePipelineCreateInfo pipelineInfo{};
        pipelineInfo.stage = stageInfo;
        pipelineInfo.layout = vkLayout;

        vk::ResultValue<vk::UniquePipeline> pipelineResult = m_device->createComputePipelineUnique({}, pipelineInfo);
        ensureSuccess(pipelineResult.result, "Failed to create compute pipeline");
        vk::UniquePipeline pipeline = std::move(pipelineResult.value);

        Pipeline handle{};
        handle.h = g_pipelineHandleCounter.fetch_add(1, std::memory_order_relaxed);

        PipelineResource resource{};
        resource.pipeline = std::move(pipeline);
        resource.layout = layout;
        resource.bindPoint = vk::PipelineBindPoint::eCompute;

        {
            std::lock_guard<std::mutex> lock(m_resourceMutex);
            m_pipelines.emplace(handle.h, std::move(resource));
        }

        return handle;
    }

    void VulkanDevice::destroy(Pipeline pipeline)
    {
        if (!pipeline.h)
        {
            return;
        }

        std::lock_guard<std::mutex> lock(m_resourceMutex);
        m_pipelines.erase(pipeline.h);
    }

    CommandList *VulkanDevice::createCommandList(QueueType type)
    {
        if (type != QueueType::GRAPHICS)
        {
            throw std::runtime_error("Only graphics command lists are supported at the moment");
        }
        return new VulkanCommandList(*this, type, m_graphicsQueueFamily);
    }

    void VulkanDevice::destroy(CommandList *list)
    {
        if (!list)
        {
            return;
        }

        VulkanCommandList *vkList = dynamic_cast<VulkanCommandList *>(list);
        if (!vkList)
        {
            throw std::runtime_error("Attempted to destroy command list created by another backend");
        }
        vkList->waitForCompletion();
        delete vkList;
    }

    Queue *VulkanDevice::getQueue(QueueType type)
    {
        if (type == QueueType::GRAPHICS)
        {
            return m_graphicsQueueWrapper.get();
        }
        return nullptr;
    }

    bool VulkanDevice::acquireNextImage(uint32_t &outImageIndex)
    {
        if (m_headless || !m_swapchain)
        {
            outImageIndex = 0;
            return false;
        }

        vk::Semaphore acquireSemaphore = nextAcquireSemaphore();

        vk::ResultValue<uint32_t> result = m_device->acquireNextImageKHR(
            m_swapchain.get(),
            std::numeric_limits<uint64_t>::max(),
            acquireSemaphore,
            VK_NULL_HANDLE);

        if (result.result == vk::Result::eErrorOutOfDateKHR)
        {
            createSwapchain(m_preferredColorFormat);
            acquireSemaphore = nextAcquireSemaphore();
            result = m_device->acquireNextImageKHR(
                m_swapchain.get(),
                std::numeric_limits<uint64_t>::max(),
                acquireSemaphore,
                VK_NULL_HANDLE);
        }

        if (result.result != vk::Result::eSuccess && result.result != vk::Result::eSuboptimalKHR)
        {
            throw std::runtime_error("Failed to acquire swapchain image");
        }

        m_currentSwapchainImage = result.value;
        m_hasSwapchainImage = true;
        m_acquireWaitPending = true;
        m_pendingAcquireSemaphore = acquireSemaphore;
        m_pendingRenderFinishedSemaphore = renderFinishedSemaphoreForImage(m_currentSwapchainImage);
        outImageIndex = m_currentSwapchainImage;
        return true;
    }

    void VulkanDevice::present(uint32_t imageIndex)
    {
        if (m_headless || !m_swapchain)
        {
            return;
        }

        vk::Semaphore waitSemaphores[1];
        uint32_t waitCount = 0;
        if (m_activeRenderFinishedSemaphore)
        {
            waitSemaphores[0] = m_activeRenderFinishedSemaphore;
            waitCount = 1;
        }
        vk::SwapchainKHR swapchainHandle = m_swapchain.get();
        vk::PresentInfoKHR presentInfo{};
        presentInfo.waitSemaphoreCount = waitCount;
        presentInfo.pWaitSemaphores = waitCount ? waitSemaphores : nullptr;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &swapchainHandle;
        presentInfo.pImageIndices = &imageIndex;

        vk::Result result = m_graphicsQueue.presentKHR(presentInfo);
        if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)
        {
            createSwapchain(m_preferredColorFormat);
        }
        else if (result != vk::Result::eSuccess)
        {
            throw std::runtime_error("Failed to present swapchain image");
        }

        m_hasSwapchainImage = false;
        m_activeRenderFinishedSemaphore = VK_NULL_HANDLE;
        collectGarbage();
    }

    void VulkanDevice::waitIdle()
    {
        if (m_device)
        {
            ensureSuccess(m_device->waitIdle(), "Failed to wait for device idle");
            collectGarbage(true);
            resetAllTransientPools();
        }
    }

    FrameEpoch VulkanDevice::beginFrame()
    {
        if (!m_frameActive)
        {
            beginFrameEpoch();
            m_frameActive = true;
        }
        return FrameEpoch{m_activeFrameEpoch};
    }

    void VulkanDevice::endFrame()
    {
        if (!m_frameActive)
        {
            return;
        }
        if (m_activeFrameEpoch != 0)
        {
            if (m_lastSubmittedTimelineValue != 0 && m_graphicsTimelineSemaphore)
            {
                enqueueFrameReset(m_activeFrameEpoch, m_lastSubmittedTimelineValue);
            }
            else
            {
                resetTransientPoolsForEpoch(m_activeFrameEpoch);
                m_activeFrameEpoch = 0;
            }
        }
        m_frameActive = false;
        m_lastSubmittedTimelineValue = 0;
        collectGarbage();
    }

    uint32_t VulkanDevice::getSwapchainImageCount() const
    {
        return static_cast<uint32_t>(m_swapchainImageViewHandles.size());
    }

    Image VulkanDevice::getSwapchainImage(uint32_t index) const
    {
        if (index >= m_swapchainImageHandles.size())
        {
            return {};
        }
        return m_swapchainImageHandles[index];
    }

    ImageView VulkanDevice::getSwapchainImageView(uint32_t index) const
    {
        if (index >= m_swapchainImageViewHandles.size())
        {
            return {};
        }
        return m_swapchainImageViewHandles[index];
    }

    std::pair<uint32_t, uint32_t> VulkanDevice::getSwapchainExtent() const
    {
        return {m_swapchainExtent.width, m_swapchainExtent.height};
    }

    TextureFormat VulkanDevice::getSwapchainFormat() const
    {
        return m_swapchainTextureFormat;
    }

    VulkanCommandList::VulkanCommandList(VulkanDevice &device, QueueType type, uint32_t familyIndex)
        : m_device(device), m_queueType(type)
    {
        vk::CommandPoolCreateInfo poolInfo{};
        poolInfo.queueFamilyIndex = familyIndex;
        poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
        vk::ResultValue<vk::UniqueCommandPool> poolResult = m_device.m_device->createCommandPoolUnique(poolInfo);
        ensureSuccess(poolResult.result, "Failed to create command pool");
        m_commandPool = std::move(poolResult.value);

        vk::CommandBufferAllocateInfo allocInfo{};
        allocInfo.commandPool = m_commandPool.get();
        allocInfo.level = vk::CommandBufferLevel::ePrimary;
        allocInfo.commandBufferCount = 1;
        m_commandBuffer = m_device.m_device->allocateCommandBuffers(allocInfo).value.front();
    }

    void VulkanCommandList::ensureRecording(const char *action) const
    {
        if (!m_recording)
        {
            throw std::runtime_error(std::string("CommandList::") + action + " called outside begin/end");
        }
    }

    const VulkanDevice::ImageViewResource &VulkanCommandList::getImageViewResource(ImageView view) const
    {
        return m_device.requireImageView(view);
    }

    void VulkanCommandList::waitForCompletion()
    {
        if (!m_pendingExecution)
        {
            return;
        }
        m_device.waitForTimeline(m_submittedTimelineValue);
        m_pendingExecution = false;
        m_submittedTimelineValue = 0;
    }

    void VulkanCommandList::begin()
    {
        if (m_recording)
        {
            throw std::runtime_error("CommandList already recording");
        }
        if (m_rendering)
        {
            throw std::runtime_error("CommandList rendering state invalid");
        }

        waitForCompletion();

        m_commandBuffer.reset();

        vk::CommandBufferBeginInfo beginInfo{};
        beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
        ensureSuccess(m_commandBuffer.begin(beginInfo), "Failed to begin command buffer");
        m_recording = true;
        m_hasWork = false;
    }

    void VulkanCommandList::end()
    {
        if (!m_recording)
        {
            throw std::runtime_error("CommandList::end without begin");
        }
        if (m_rendering)
        {
            endRendering();
        }

        ensureSuccess(m_commandBuffer.end(), "Failed to end command buffer");
        m_recording = false;
        m_hasWork = true;
    }

    void VulkanCommandList::barrier(uint32_t bufferCount, const BufferBarrier *buffers,
                                    uint32_t imageCount, const ImageBarrier *images)
    {
        ensureRecording("barrier");

        std::vector<vk::BufferMemoryBarrier2> bufferBarriers;
        bufferBarriers.reserve(bufferCount);
        for (uint32_t i = 0; i < bufferCount; ++i)
        {
            const BufferBarrier &barrier = buffers[i];
            VulkanDevice::BufferResource &bufferResource = m_device.requireBuffer(barrier.buffer);
            vk::BufferMemoryBarrier2 vkBarrier{};
            vkBarrier.srcStageMask = toVkPipelineStage2(barrier.srcStage);
            vkBarrier.srcAccessMask = toVkAccess2(barrier.srcAccess);
            vkBarrier.dstStageMask = toVkPipelineStage2(barrier.dstStage);
            vkBarrier.dstAccessMask = toVkAccess2(barrier.dstAccess);
            vkBarrier.buffer = bufferResource.buffer;
            vkBarrier.offset = barrier.offset;
            vkBarrier.size = barrier.size == ~0ull ? bufferResource.desc.size - barrier.offset : barrier.size;
            bufferBarriers.push_back(vkBarrier);
        }

        std::vector<vk::ImageMemoryBarrier2> imageBarriers;
        imageBarriers.reserve(imageCount);
        for (uint32_t i = 0; i < imageCount; ++i)
        {
            const ImageBarrier &barrier = images[i];
            VulkanDevice::ImageResource &imageResource = m_device.requireImage(barrier.image);
            vk::ImageMemoryBarrier2 vkBarrier{};
            vkBarrier.srcStageMask = toVkPipelineStage2(barrier.srcStage);
            vkBarrier.srcAccessMask = toVkAccess2(barrier.srcAccess);
            vkBarrier.dstStageMask = toVkPipelineStage2(barrier.dstStage);
            vkBarrier.dstAccessMask = toVkAccess2(barrier.dstAccess);
            vkBarrier.oldLayout = toVkImageLayout(barrier.oldLayout);
            vkBarrier.newLayout = toVkImageLayout(barrier.newLayout);
            vkBarrier.image = imageResource.image;
            vkBarrier.subresourceRange.aspectMask = toVkAspectFlags(barrier.range.aspect);
            vkBarrier.subresourceRange.baseMipLevel = barrier.range.baseMip;
            vkBarrier.subresourceRange.levelCount = barrier.range.levelCount;
            vkBarrier.subresourceRange.baseArrayLayer = barrier.range.baseArrayLayer;
            vkBarrier.subresourceRange.layerCount = barrier.range.layerCount;
            imageBarriers.push_back(vkBarrier);
        }

        vk::DependencyInfo dependencyInfo{};
        dependencyInfo.bufferMemoryBarrierCount = static_cast<uint32_t>(bufferBarriers.size());
        dependencyInfo.pBufferMemoryBarriers = bufferBarriers.data();
        dependencyInfo.imageMemoryBarrierCount = static_cast<uint32_t>(imageBarriers.size());
        dependencyInfo.pImageMemoryBarriers = imageBarriers.data();

        m_commandBuffer.pipelineBarrier2(dependencyInfo);
    }

    void VulkanCommandList::beginRendering(const RenderingDesc &desc)
    {
        ensureRecording("beginRendering");
        if (m_rendering)
        {
            throw std::runtime_error("Rendering already active");
        }
        if (desc.colorCount > 0 && !desc.color)
        {
            throw std::runtime_error("RenderingDesc color array is null");
        }

        std::vector<vk::RenderingAttachmentInfo> colorAttachments;
        colorAttachments.reserve(desc.colorCount);
        vk::Extent2D extent{desc.width, desc.height};

        for (uint32_t i = 0; i < desc.colorCount; ++i)
        {
            const RenderingAttachment &attachment = desc.color[i];
            if (!attachment.imageView)
            {
                throw std::runtime_error("Rendering color attachment missing image view");
            }

            ImageView handle = *reinterpret_cast<ImageView *>(attachment.imageView);
            const VulkanDevice::ImageViewResource &viewResource = getImageViewResource(handle);

            vk::RenderingAttachmentInfo info{};
            info.imageView = viewResource.view.get();
            info.imageLayout = toVkImageLayout(attachment.layout);
            info.loadOp = attachment.load ? vk::AttachmentLoadOp::eLoad : vk::AttachmentLoadOp::eClear;
            info.storeOp = attachment.store ? vk::AttachmentStoreOp::eStore : vk::AttachmentStoreOp::eDontCare;
            info.clearValue.color = vk::ClearColorValue(
                std::array<float, 4>{attachment.clearColor.r, attachment.clearColor.g,
                                     attachment.clearColor.b, attachment.clearColor.a});
            colorAttachments.push_back(info);

            if (extent.width == 0 || extent.height == 0)
            {
                extent = deriveExtent(desc.width, desc.height, m_device.requireImage(viewResource.image).desc);
            }
        }

        vk::RenderingAttachmentInfo depthAttachment{};
        const RenderingAttachment *depthDesc = desc.depth;
        bool hasDepth = depthDesc && depthDesc->imageView;
        if (hasDepth)
        {
            ImageView depthHandle = *reinterpret_cast<ImageView *>(depthDesc->imageView);
            const VulkanDevice::ImageViewResource &viewResource = getImageViewResource(depthHandle);
            depthAttachment.imageView = viewResource.view.get();
            depthAttachment.imageLayout = toVkImageLayout(depthDesc->layout);
            depthAttachment.loadOp = depthDesc->load ? vk::AttachmentLoadOp::eLoad : vk::AttachmentLoadOp::eClear;
            depthAttachment.storeOp = depthDesc->store ? vk::AttachmentStoreOp::eStore : vk::AttachmentStoreOp::eDontCare;
            depthAttachment.clearValue.depthStencil.depth = depthDesc->clearDS.depth;
            depthAttachment.clearValue.depthStencil.stencil = depthDesc->clearDS.stencil;
            if (extent.width == 0 || extent.height == 0)
            {
                extent = deriveExtent(desc.width, desc.height, m_device.requireImage(viewResource.image).desc);
            }
        }

        if (extent.width == 0 || extent.height == 0)
        {
            throw std::runtime_error("Rendering extent is invalid");
        }

        vk::RenderingInfo renderingInfo{};
        renderingInfo.renderArea = vk::Rect2D({0, 0}, extent);
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size());
        renderingInfo.pColorAttachments = colorAttachments.data();
        renderingInfo.pDepthAttachment = hasDepth ? &depthAttachment : nullptr;

        m_commandBuffer.beginRendering(renderingInfo);
        m_rendering = true;
    }

    void VulkanCommandList::endRendering()
    {
        if (!m_rendering)
        {
            return;
        }
        m_commandBuffer.endRendering();
        m_rendering = false;
    }

    void VulkanCommandList::bindPipeline(Pipeline pipeline)
    {
        ensureRecording("bindPipeline");
        vk::PipelineBindPoint bindPoint;
        vk::Pipeline vkPipeline = m_device.getVkPipeline(pipeline, bindPoint);
        m_commandBuffer.bindPipeline(bindPoint, vkPipeline);
        m_boundBindPoint = bindPoint;
        m_hasWork = true;
    }

    void VulkanCommandList::bindPipelineLayout(PipelineLayout layout)
    {
        m_boundLayout = layout;
    }

    void VulkanCommandList::bindDescriptorSet(const PipelineLayout &layout, DescSetIndex set, const DescriptorSet &descriptorSet,
                                              std::span<const uint32_t> dynamicOffsets)
    {
        ensureRecording("bindDescriptorSet");
        PipelineLayout targetLayout = layout.h ? layout : m_boundLayout;
        if (!targetLayout.h)
        {
            throw std::runtime_error("No pipeline layout bound for descriptor set binding");
        }
        vk::PipelineLayout vkLayout = m_device.getVkPipelineLayout(targetLayout);
        vk::DescriptorSet vkSet = m_device.getVkDescriptorSetHandle(descriptorSet);
        if (!vkSet)
        {
            throw std::runtime_error("Invalid descriptor set handle");
        }
        if (!dynamicOffsets.empty())
        {
            m_commandBuffer.bindDescriptorSets(m_boundBindPoint, vkLayout, set, 1, &vkSet,
                                               static_cast<uint32_t>(dynamicOffsets.size()), dynamicOffsets.data());
        }
        else
        {
            m_commandBuffer.bindDescriptorSets(m_boundBindPoint, vkLayout, set, vkSet, {});
        }
        m_hasWork = true;
    }

    void VulkanCommandList::bindVertexBuffers(uint32_t firstBinding, uint32_t count,
                                              const Buffer *buffers, const uint64_t *offsets)
    {
        ensureRecording("bindVertexBuffers");
        std::vector<vk::Buffer> vkBuffers(count);
        std::vector<vk::DeviceSize> vkOffsets(count);
        for (uint32_t i = 0; i < count; ++i)
        {
            VulkanDevice::BufferResource &resource = m_device.requireBuffer(buffers[i]);
            vkBuffers[i] = resource.buffer;
            vkOffsets[i] = offsets ? offsets[i] : 0;
        }
        m_commandBuffer.bindVertexBuffers(firstBinding, vkBuffers, vkOffsets);
    }

    void VulkanCommandList::bindIndexBuffer(Buffer buffer, uint64_t offset, IndexType type)
    {
        ensureRecording("bindIndexBuffer");
        VulkanDevice::BufferResource &resource = m_device.requireBuffer(buffer);
        m_commandBuffer.bindIndexBuffer(resource.buffer, offset, toVkIndexType(type));
    }

    void VulkanCommandList::pushConstants(PipelineLayout layout, ShaderStage stages,
                                          uint32_t offset, uint32_t size, const void *data)
    {
        ensureRecording("pushConstants");
        vk::PipelineLayout vkLayout = m_device.getVkPipelineLayout(layout.h ? layout : m_boundLayout);
        if (!vkLayout)
        {
            throw std::runtime_error("No pipeline layout bound for push constants");
        }
        m_commandBuffer.pushConstants(vkLayout, toVkShaderStageFlags(stages), offset, size, data);
    }

    void VulkanCommandList::setViewport(float x, float y, float width, float height,
                                        float minDepth, float maxDepth)
    {
        ensureRecording("setViewport");
        vk::Viewport viewport{};
        viewport.x = x;
        viewport.y = y;
        viewport.width = width;
        viewport.height = height;
        viewport.minDepth = minDepth;
        viewport.maxDepth = maxDepth;
        m_commandBuffer.setViewport(0, 1, &viewport);
    }

    void VulkanCommandList::setScissor(int32_t x, int32_t y,
                                       uint32_t width, uint32_t height)
    {
        ensureRecording("setScissor");
        vk::Rect2D rect{};
        rect.offset = vk::Offset2D{x, y};
        rect.extent = vk::Extent2D{width, height};
        m_commandBuffer.setScissor(0, 1, &rect);
    }

    void VulkanCommandList::draw(uint32_t vtxCount, uint32_t instCount,
                                 uint32_t firstVtx, uint32_t firstInst)
    {
        ensureRecording("draw");
        m_commandBuffer.draw(vtxCount, instCount, firstVtx, firstInst);
    }

    void VulkanCommandList::drawIndexed(uint32_t idxCount, uint32_t instCount,
                                        uint32_t firstIdx, int32_t vtxOffset, uint32_t firstInst)
    {
        ensureRecording("drawIndexed");
        m_commandBuffer.drawIndexed(idxCount, instCount, firstIdx, vtxOffset, firstInst);
    }

    void VulkanCommandList::drawIndirect(Buffer cmdBuf, uint64_t cmdOffset,
                                         uint32_t drawCount, uint32_t stride)
    {
        ensureRecording("drawIndirect");
        VulkanDevice::BufferResource &resource = m_device.requireBuffer(cmdBuf);
        m_commandBuffer.drawIndirect(resource.buffer, cmdOffset, drawCount, stride);
    }

    void VulkanCommandList::drawIndexedIndirect(Buffer cmdBuf, uint64_t cmdOffset,
                                                uint32_t drawCount, uint32_t stride)
    {
        ensureRecording("drawIndexedIndirect");
        VulkanDevice::BufferResource &resource = m_device.requireBuffer(cmdBuf);
        m_commandBuffer.drawIndexedIndirect(resource.buffer, cmdOffset, drawCount, stride);
    }

    void VulkanCommandList::drawIndexedIndirectCount(Buffer cmdBuf, uint64_t cmdOffset,
                                                     Buffer countBuf, uint64_t countOffset,
                                                     uint32_t maxDraws, uint32_t stride)
    {
        ensureRecording("drawIndexedIndirectCount");
        VulkanDevice::BufferResource &cmdResource = m_device.requireBuffer(cmdBuf);
        VulkanDevice::BufferResource &countResource = m_device.requireBuffer(countBuf);
        m_commandBuffer.drawIndexedIndirectCount(cmdResource.buffer, cmdOffset,
                                                 countResource.buffer, countOffset,
                                                 maxDraws, stride);
    }

    void VulkanCommandList::dispatch(uint32_t gx, uint32_t gy, uint32_t gz)
    {
        ensureRecording("dispatch");
        m_commandBuffer.dispatch(gx, gy, gz);
    }

    void VulkanCommandList::copyBuffer(Buffer src, uint64_t srcOff,
                                       Buffer dst, uint64_t dstOff, uint64_t size)
    {
        ensureRecording("copyBuffer");
        VulkanDevice::BufferResource &srcResource = m_device.requireBuffer(src);
        VulkanDevice::BufferResource &dstResource = m_device.requireBuffer(dst);
        vk::BufferCopy copyRegion{};
        copyRegion.srcOffset = srcOff;
        copyRegion.dstOffset = dstOff;
        copyRegion.size = (size == ~0ull) ? (dstResource.desc.size - dstOff) : size;
        m_commandBuffer.copyBuffer(srcResource.buffer, dstResource.buffer, copyRegion);
    }

    void VulkanCommandList::copyBufferToImage(Buffer src, uint64_t srcOff,
                                              Image dst, ImageLayout layout,
                                              uint32_t width, uint32_t height)
    {
        ensureRecording("copyBufferToImage");
        VulkanDevice::BufferResource &srcResource = m_device.requireBuffer(src);
        VulkanDevice::ImageResource &dstResource = m_device.requireImage(dst);

        vk::BufferImageCopy region{};
        region.bufferOffset = srcOff;
        region.imageSubresource.aspectMask = defaultAspectFlags(dstResource.desc.format);
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = dstResource.desc.arrayLayers;
        region.imageExtent = vk::Extent3D{width, height, 1};

        m_commandBuffer.copyBufferToImage(srcResource.buffer,
                                          dstResource.image,
                                          toVkImageLayout(layout), region);
    }

    void VulkanCommandList::beginLabel(const char *name, float r, float g, float b, float a)
    {
        (void)name;
        (void)r;
        (void)g;
        (void)b;
        (void)a;
    }

    void VulkanCommandList::endLabel()
    {
    }

    VulkanQueue::VulkanQueue(VulkanDevice &device, QueueType type, uint32_t familyIndex, vk::Queue queue)
        : m_device(device), m_type(type), m_familyIndex(familyIndex), m_queue(queue)
    {
    }

    void VulkanQueue::submit(CommandList **lists, uint32_t count)
    {
        if (count == 0)
        {
            return;
        }

        std::vector<vk::CommandBufferSubmitInfo> cmdInfos;
        cmdInfos.reserve(count);

        std::vector<VulkanCommandList *> submittedLists;
        submittedLists.reserve(count);

        for (uint32_t i = 0; i < count; ++i)
        {
            VulkanCommandList *vkList = dynamic_cast<VulkanCommandList *>(lists[i]);
            if (!vkList)
            {
                throw std::runtime_error("Queue::submit expects Vulkan command lists");
            }
            if (!vkList->isReadyForSubmit())
            {
                throw std::runtime_error("Command list must be ended before submission");
            }

            vk::CommandBufferSubmitInfo info{};
            info.commandBuffer = vkList->getCommandBuffer();
            cmdInfos.push_back(info);
            submittedLists.push_back(vkList);
        }

        std::vector<vk::SemaphoreSubmitInfo> waitInfos;
        if (m_device.m_acquireWaitPending && m_device.m_pendingAcquireSemaphore)
        {
            vk::SemaphoreSubmitInfo wait{};
            wait.semaphore = m_device.m_pendingAcquireSemaphore;
            wait.stageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
            waitInfos.push_back(wait);
            m_device.m_acquireWaitPending = false;
            m_device.m_pendingAcquireSemaphore = VK_NULL_HANDLE;
        }

        std::vector<vk::SemaphoreSubmitInfo> signalInfos;
        if (m_device.m_pendingRenderFinishedSemaphore)
        {
            vk::SemaphoreSubmitInfo signalRender{};
            signalRender.semaphore = m_device.m_pendingRenderFinishedSemaphore;
            signalRender.stageMask = vk::PipelineStageFlagBits2::eAllGraphics;
            signalInfos.push_back(signalRender);
            m_device.m_activeRenderFinishedSemaphore = m_device.m_pendingRenderFinishedSemaphore;
            m_device.m_pendingRenderFinishedSemaphore = VK_NULL_HANDLE;
        }

        vk::SemaphoreSubmitInfo signalTimeline{};
        signalTimeline.semaphore = m_device.m_graphicsTimelineSemaphore.get();
        signalTimeline.stageMask = vk::PipelineStageFlagBits2::eAllCommands;
        signalTimeline.value = ++m_device.m_graphicsTimelineValue;
        signalInfos.push_back(signalTimeline);

        vk::SubmitInfo2 submitInfo{};
        submitInfo.waitSemaphoreInfoCount = static_cast<uint32_t>(waitInfos.size());
        submitInfo.pWaitSemaphoreInfos = waitInfos.data();
        submitInfo.commandBufferInfoCount = static_cast<uint32_t>(cmdInfos.size());
        submitInfo.pCommandBufferInfos = cmdInfos.data();
        submitInfo.signalSemaphoreInfoCount = static_cast<uint32_t>(signalInfos.size());
        submitInfo.pSignalSemaphoreInfos = signalInfos.data();

        ensureSuccess(m_queue.submit2(submitInfo, nullptr), "Failed to submit command buffers");
        m_device.m_lastSubmittedTimelineValue = signalTimeline.value;
        m_device.collectGarbage();

        for (VulkanCommandList *list : submittedLists)
        {
            list->markSubmitted(signalTimeline.value);
        }
    }

    void VulkanQueue::present()
    {
        if (m_type != QueueType::GRAPHICS || !m_device.m_hasSwapchainImage)
        {
            return;
        }

        m_device.present(m_device.m_currentSwapchainImage);
    }

}
