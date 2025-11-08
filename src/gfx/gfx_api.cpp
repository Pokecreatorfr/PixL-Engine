#include <gfx/gfx_api.hpp>
#include <gfx/gfx_vulkan.hpp>

namespace pixl::gfx
{
    std::unique_ptr<Device> pixl::gfx::Device::Create(const DeviceCreateInfo &info)
    {
        switch (info.api)
        {
        case device::API::VULKAN:
            return std::make_unique<VulkanDevice>(info);
        default:
            throw std::runtime_error("Unsupported graphics API");
        }
    }
}