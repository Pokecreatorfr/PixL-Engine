#include <gtest/gtest.h>

#include <gfx/gfx_api.hpp>

using namespace pixl::gfx;

namespace
{
    std::unique_ptr<Device> createHeadlessDevice()
    {
        DeviceCreateInfo info{};
        info.api = device::API::VULKAN;
        info.headless = true;
        info.enableValidation = false;
        return Device::Create(info);
    }
}

TEST(VulkanDeviceTests, HeadlessDeviceCreation)
{
    std::unique_ptr<Device> device;
    ASSERT_NO_THROW({ device = createHeadlessDevice(); });
    ASSERT_NE(device, nullptr);
    EXPECT_EQ(device->getDeviceInfo().api, device::API::VULKAN);
}

TEST(VulkanDeviceTests, BufferLifecycle)
{
    auto device = createHeadlessDevice();
    BufferDesc desc{};
    desc.size = 4096;
    desc.usage = BufferUsage::VERTEX | BufferUsage::TRANSFER_DST;
    desc.memory = MemoryUsage::CPU_TO_GPU;

    Buffer buffer{};
    ASSERT_NO_THROW({ buffer = device->createBuffer(desc); });
    EXPECT_NE(buffer.h, 0u);
    EXPECT_NO_THROW(device->destroy(buffer));
    EXPECT_NO_THROW(device->waitIdle());
}

TEST(VulkanDeviceTests, ImageLifecycle)
{
    auto device = createHeadlessDevice();
    ImageDesc desc{};
    desc.width = 128;
    desc.height = 128;
    desc.format = TextureFormat::R8G8B8A8_UNORM;
    desc.usage = TextureUsage::COLOR_ATTACHMENT | TextureUsage::TRANSFER_DST;

    Image image{};
    ASSERT_NO_THROW({ image = device->createImage(desc); });
    EXPECT_NE(image.h, 0u);
    EXPECT_NO_THROW(device->destroy(image));
    EXPECT_NO_THROW(device->waitIdle());
}
