#include <gtest/gtest.h>

#include <gfx/gfx_api.hpp>
#include <gfx/gfx_material.hpp>
#include <gfx/demo_shaders.hpp>
#include <array>
#include <vector>
#include <gfx/demo_shaders.hpp>

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

TEST(DescriptorTests, AllocateAndUpdateDescriptors)
{
    auto device = createHeadlessDevice();
    ASSERT_NE(device, nullptr);

    SetLayoutInfo setInfo{};
    setInfo.set = 0;
    BindingInfo uboBinding{};
    uboBinding.binding = 0;
    uboBinding.type = DescType::UniformBuffer;
    uboBinding.count = 1;
    uboBinding.stages = to_u(ShaderStage::VERTEX) | to_u(ShaderStage::FRAGMENT);
    setInfo.bindings.push_back(uboBinding);

    BindingInfo samplerBinding{};
    samplerBinding.binding = 1;
    samplerBinding.type = DescType::Sampler;
    samplerBinding.count = 1;
    samplerBinding.stages = to_u(ShaderStage::FRAGMENT);
    setInfo.bindings.push_back(samplerBinding);

    DescriptorSetLayout layout = device->createDescriptorSetLayout(setInfo);
    DescriptorSet descriptorSet = device->allocateDescriptorSet(layout, true);

    BufferDesc bufferDesc{};
    bufferDesc.size = 256;
    bufferDesc.usage = BufferUsage::UNIFORM | BufferUsage::TRANSFER_DST;
    bufferDesc.memory = MemoryUsage::CPU_TO_GPU;
    Buffer buffer = device->createBuffer(bufferDesc);

    SamplerDesc samplerDesc{};
    Sampler sampler = device->createSampler(samplerDesc);

    DescriptorWriter writer(*device);
    WriteBuf writeBuf{};
    writeBuf.buffer = buffer;
    writeBuf.range = bufferDesc.size;
    WriteImg writeImg{};
    writeImg.sampler = sampler;

    ASSERT_NO_THROW({
        writer.buffer(descriptorSet, 0, 0, DescType::UniformBuffer, writeBuf)
            .image(descriptorSet, 1, 0, DescType::Sampler, writeImg)
            .submit();
    });

    device->destroy(sampler);
    device->destroy(buffer);
    device->destroy(layout);
}

TEST(DescriptorTests, AllocateBindlessDescriptorSet)
{
    auto device = createHeadlessDevice();
    ASSERT_NE(device, nullptr);

    SetLayoutInfo setInfo{};
    setInfo.set = 0;
    BindingInfo bindless{};
    bindless.binding = 10;
    bindless.type = DescType::SampledImage;
    bindless.count = 0;
    bindless.bindless = true;
    bindless.stages = to_u(ShaderStage::FRAGMENT);
    setInfo.bindings.push_back(bindless);

    DescriptorSetLayout layout = device->createDescriptorSetLayout(setInfo);
    EXPECT_NE(layout.h, 0u);

    DescriptorSet descriptorSet = device->allocateDescriptorSet(layout, true);
    EXPECT_NE(descriptorSet.h, 0u);

    device->destroy(layout);
}

TEST(DescriptorTests, PipelineLayoutCacheFromShaders)
{
    auto device = createHeadlessDevice();
    ASSERT_NE(device, nullptr);

    ShaderDesc vertexDesc{};
    vertexDesc.stage = ShaderStage::VERTEX;
    vertexDesc.code = demo::kDemoVertexSpv.data();
    vertexDesc.codeSize = demo::kDemoVertexSpv.size() * sizeof(uint32_t);
    vertexDesc.entryPoint = "main";

    ShaderDesc fragmentDesc{};
    fragmentDesc.stage = ShaderStage::FRAGMENT;
    fragmentDesc.code = demo::kDemoFragmentSpv.data();
    fragmentDesc.codeSize = demo::kDemoFragmentSpv.size() * sizeof(uint32_t);
    fragmentDesc.entryPoint = "main";

    Shader vertexShader = device->createShader(vertexDesc);
    Shader fragmentShader = device->createShader(fragmentDesc);

    MaterialPipelineLayoutCache layoutCache(*device);
    std::array<Shader, 2> shaders{vertexShader, fragmentShader};
    PipelineLayout layout = layoutCache.getOrCreate(shaders);
    EXPECT_NE(layout.h, 0u);

    MaterialDescriptorTable descriptorTable(*device, layout, true);
    EXPECT_NE(descriptorTable.sets().material.h, 0u);

    layoutCache.clear();
    device->destroy(vertexShader);
    device->destroy(fragmentShader);
}

TEST(DescriptorTests, TransientPoolResetReuse)
{
    auto device = createHeadlessDevice();
    ASSERT_NE(device, nullptr);

    SetLayoutInfo setInfo{};
    setInfo.set = 1;
    BindingInfo samplerBinding{};
    samplerBinding.binding = 0;
    samplerBinding.type = DescType::CombinedImageSampler;
    samplerBinding.count = 1;
    samplerBinding.stages = to_u(ShaderStage::FRAGMENT);
    setInfo.bindings.push_back(samplerBinding);

    DescriptorSetLayout layout = device->createDescriptorSetLayout(setInfo);

    ImageDesc imageDesc{};
    imageDesc.width = 1;
    imageDesc.height = 1;
    imageDesc.format = TextureFormat::R8G8B8A8_UNORM;
    imageDesc.usage = TextureUsage::SAMPLED | TextureUsage::TRANSFER_DST;
    Image image = device->createImage(imageDesc);

    ImageViewDesc viewDesc{};
    viewDesc.type = ImageViewType::TYPE_2D;
    viewDesc.format = imageDesc.format;
    viewDesc.range.aspect = AspectMask::COLOR;
    viewDesc.range.levelCount = 1;
    viewDesc.range.layerCount = 1;
    ImageView view = device->createImageView(image, viewDesc);

    SamplerDesc samplerDesc{};
    Sampler sampler = device->createSampler(samplerDesc);

    constexpr int kIterations = 3;
    constexpr int kSetCount = 1000;
    for (int frame = 0; frame < kIterations; ++frame)
    {
        device->beginFrame();
        std::vector<DescriptorSet> sets;
        sets.reserve(kSetCount);
        DescriptorWriter writer(*device);
        for (int i = 0; i < kSetCount; ++i)
        {
            DescriptorSet set = device->allocateDescriptorSet(layout, true);
            ASSERT_NE(set.h, 0u);
            sets.push_back(set);
            WriteImg write{};
            write.view = view;
            write.sampler = sampler;
            write.layout = ImageLayout::SHADER_READ_ONLY;
            writer.image(set, 0, 0, DescType::CombinedImageSampler, write);
        }
        writer.submit();
        for (DescriptorSet &set : sets)
        {
            device->free(set);
        }
        device->endFrame();
    }

    EXPECT_NO_THROW(device->waitIdle());

    device->destroy(sampler);
    device->destroy(view);
    device->destroy(image);
    device->destroy(layout);
}
