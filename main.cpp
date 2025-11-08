#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <array>
#include <chrono>
#include <core/Core.hpp>
#include <cstdlib>
#include <gfx/demo_shaders.hpp>
#include <gfx/gfx_material.hpp>
#include <gfx/gfx_vulkan.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <limits>
#include <vector>

int main(int argc, char **argv)
{
    pixl::core::Core::Core::Init(pixl::core::CoreInitData{});

    bool sdlInitialized = false;
    bool headless = false;
    SDL_Window *window = nullptr;

    int nb_dri = SDL_GetNumVideoDrivers();

    for (int i = 0; i < nb_dri; ++i)
    {
        const char *driver = SDL_GetVideoDriver(i);
        std::cout << "Available video driver: " << driver << std::endl;
    }

    SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "wayland");

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        headless = true;
    }
    else
    {
        sdlInitialized = true;
        const int initialWidth = 1280;
        const int initialHeight = 720;
        window = SDL_CreateWindow("PixL Engine Demo",
                                  initialWidth,
                                  initialHeight,
                                  SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
        if (!window)
        {
            std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << ". Falling back to headless mode." << std::endl;
            SDL_Quit();
            sdlInitialized = false;
            headless = true;
        }
    }

    pixl::gfx::DeviceCreateInfo deviceInfo{};
    deviceInfo.api = pixl::gfx::device::API::VULKAN;
    deviceInfo.windowHandle = headless ? nullptr : window;
    deviceInfo.headless = headless;
    std::unique_ptr<pixl::gfx::Device> device = pixl::gfx::Device::Create(deviceInfo);
    if (!device)
    {
        if (window)
        {
            SDL_DestroyWindow(window);
        }
        if (sdlInitialized)
        {
            SDL_Quit();
        }
        return -1;
    }

    pixl::gfx::device::DeviceInfo info = device->getDeviceInfo();

    std::cout << "Using device: " << info.name << " (VRAM: " << info.vramBytes / (1024 * 1024) << " MB)" << std::endl;
    std::cout << "API Version: " << info.apiVersionMajor << "." << info.apiVersionMinor << std::endl;

    std::cout << "Enabled Features:" << std::endl;
    const pixl::gfx::device::Features &features = device->getEnabledFeatures();
    if (features.descriptorIndexing)
        std::cout << " - Descriptor Indexing" << std::endl;
    if (features.dynamicRendering)
        std::cout << " - Dynamic Rendering" << std::endl;
    if (features.timelineSemaphore)
        std::cout << " - Timeline Semaphore" << std::endl;
    if (features.bufferDeviceAddress)
        std::cout << " - Buffer Device Address" << std::endl;
    if (features.shaderInt64)
        std::cout << " - Shader Int64" << std::endl;
    if (features.shaderFloat16)
        std::cout << " - Shader Float16" << std::endl;
    if (features.subgroupBasic)
        std::cout << " - Subgroup Basic" << std::endl;
    if (features.subgroupVote)
        std::cout << " - Subgroup Vote" << std::endl;
    if (features.subgroupBallot)
        std::cout << " - Subgroup Ballot" << std::endl;
    if (features.subgroupShuffle)
        std::cout << " - Subgroup Shuffle" << std::endl;
    if (features.mesh != pixl::gfx::device::MeshShaderSupport::NONE)
        std::cout << " - Mesh Shaders" << std::endl;
    if (features.rayTracing != pixl::gfx::device::RayTracingSupport::NONE)
        std::cout << " - Ray Tracing" << std::endl;
    if (features.vrs != pixl::gfx::device::VRSTier::NONE)
        std::cout << " - Variable Rate Shading" << std::endl;
    if (features.conservativeRaster)
        std::cout << " - Conservative Rasterization" << std::endl;
    if (features.fragmentShaderInterlock)
        std::cout << " - Fragment Shader Interlock" << std::endl;
    if (features.samplerAnisotropy)
        std::cout << " - Sampler Anisotropy" << std::endl;
    if (features.sparseBinding)
        std::cout << " - Sparse Binding" << std::endl;
    if (features.residency2)
        std::cout << " - Residency 2" << std::endl;

    std::cout << "Device Limits:" << std::endl;
    const pixl::gfx::device::Limits &limits = device->getLimits();
    std::cout << " - Max 2D Texture Dimension: " << limits.maxTextureDim2D << std::endl;
    std::cout << " - Max Uniform Buffer Range: " << limits.maxUniformBufferRange << " bytes" << std::endl;
    std::cout << " - Max Storage Buffer Range: " << limits.maxStorageBufferRange << " bytes" << std::endl;
    std::cout << " - Max Push Constants Size: " << limits.maxPushConstantsSize << " bytes" << std::endl;
    std::cout << " - Max Compute Work Group Invocations: " << limits.maxComputeWorkGroupInvocations << std::endl;
    std::cout << " - Max Draw Indirect Count: " << limits.maxDrawIndirectCount << std::endl;
    std::cout << " - Subgroup Size: " << limits.subgroupSizeMin;
    if (limits.subgroupSizeMin != limits.subgroupSizeMax)
        std::cout << " - " << limits.subgroupSizeMax;
    std::cout << std::endl;
    if (features.mesh != pixl::gfx::device::MeshShaderSupport::NONE)
    {
        std::cout << " - Max Mesh Work Group Invocations: " << limits.maxMeshWorkGroupInvocations << std::endl;
        std::cout << " - Max Mesh Output Vertices: " << limits.maxMeshOutputVertices << std::endl;
        std::cout << " - Max Mesh Output Primitives: " << limits.maxMeshOutputPrimitives << std::endl;
    }
    if (features.rayTracing != pixl::gfx::device::RayTracingSupport::NONE)
    {
        std::cout << " - Max Ray Recursion Depth: " << limits.maxRayRecursionDepth << std::endl;
    }
    if (features.samplerAnisotropy)
    {
        std::cout << " - Max Sampler Anisotropy: " << limits.maxSamplerAnisotropy << std::endl;
    }

    if (headless)
    {
        std::cout << "Running in headless mode; demo window unavailable." << std::endl;
        device.reset();
        if (window)
        {
            SDL_DestroyWindow(window);
        }
        if (sdlInitialized)
        {
            SDL_Quit();
        }
        pixl::core::Core::Quit();
        return 0;
    }

    pixl::gfx::ShaderDesc vertexDesc{};
    vertexDesc.stage = pixl::gfx::ShaderStage::VERTEX;
    vertexDesc.code = pixl::gfx::demo::kDemoVertexSpv.data();
    vertexDesc.codeSize = pixl::gfx::demo::kDemoVertexSpv.size() * sizeof(uint32_t);
    vertexDesc.entryPoint = "main";

    pixl::gfx::ShaderDesc fragmentDesc{};
    fragmentDesc.stage = pixl::gfx::ShaderStage::FRAGMENT;
    fragmentDesc.code = pixl::gfx::demo::kDemoFragmentSpv.data();
    fragmentDesc.codeSize = pixl::gfx::demo::kDemoFragmentSpv.size() * sizeof(uint32_t);
    fragmentDesc.entryPoint = "main";

    pixl::gfx::Shader vertexShader = device->createShader(vertexDesc);
    pixl::gfx::Shader fragmentShader = device->createShader(fragmentDesc);

    pixl::gfx::MaterialPipelineLayoutCache layoutCache(*device);
    std::array<pixl::gfx::Shader, 2> shaderStages{vertexShader, fragmentShader};
    pixl::gfx::PipelineLayout demoPipelineLayout = layoutCache.getOrCreate(shaderStages);
    const pixl::gfx::ShaderLayout &demoLayoutInfo = device->getPipelineLayoutInfo(demoPipelineLayout);

    pixl::gfx::BindingIndex samplerBinding = std::numeric_limits<pixl::gfx::BindingIndex>::max();
    for (const pixl::gfx::SetLayoutInfo &setInfo : demoLayoutInfo.sets)
    {
        if (setInfo.set != 1)
        {
            continue;
        }
        for (const pixl::gfx::BindingInfo &binding : setInfo.bindings)
        {
            if (binding.type == pixl::gfx::DescType::CombinedImageSampler)
            {
                samplerBinding = binding.binding;
                break;
            }
        }
        break;
    }

    pixl::gfx::TextureFormat swapchainFormat = device->getSwapchainFormat();
    if (swapchainFormat == pixl::gfx::TextureFormat::UNKNOWN)
    {
        swapchainFormat = info.preferredColorFormat;
    }

    pixl::gfx::GraphicsPipelineDesc pipelineDesc{};
    pipelineDesc.topology = pixl::gfx::PrimitiveTopology::TRIANGLE_STRIP;
    pipelineDesc.shaders = shaderStages.data();
    pipelineDesc.shaderCount = static_cast<uint32_t>(shaderStages.size());
    pipelineDesc.colorFormats = &swapchainFormat;
    pipelineDesc.colorFormatCount = swapchainFormat == pixl::gfx::TextureFormat::UNKNOWN ? 0u : 1u;
    pipelineDesc.depth.depthTest = false;
    pipelineDesc.depth.depthWrite = false;
    pipelineDesc.raster.cull = pixl::gfx::CullMode::NONE;
    pipelineDesc.msaa.samples = pixl::gfx::SampleCount::_1;
    pixl::gfx::ColorBlendAttachment blendAttachment{};
    blendAttachment.enable = false;
    blendAttachment.writeMask = 0xF;
    pipelineDesc.blendAttachments = &blendAttachment;
    pipelineDesc.blendAttachmentCount = pipelineDesc.colorFormatCount;

    pixl::gfx::Pipeline pipeline{};
    if (pipelineDesc.colorFormatCount > 0)
    {
        pipeline = device->createGraphicsPipeline(pipelineDesc, demoPipelineLayout);
    }
    else
    {
        std::cout << "Swapchain format unknown; skipping pipeline creation.\n";
    }

    pixl::gfx::Queue *graphicsQueue = device->getQueue(pixl::gfx::QueueType::GRAPHICS);
    if (!graphicsQueue)
    {
        std::cerr << "Graphics queue is unavailable." << std::endl;
        if (pipeline.h)
        {
            device->destroy(pipeline);
        }
        device->destroy(vertexShader);
        device->destroy(fragmentShader);
        layoutCache.clear();
        device.reset();
        SDL_DestroyWindow(window);
        SDL_Quit();
        pixl::core::Core::Quit();
        return -1;
    }

    pixl::gfx::MaterialDescriptorTable descriptorTable(*device, demoPipelineLayout, false);

    pixl::gfx::ImageDesc imageDesc{};
    imageDesc.width = 1;
    imageDesc.height = 1;
    imageDesc.format = pixl::gfx::TextureFormat::R8G8B8A8_UNORM;
    imageDesc.usage = pixl::gfx::TextureUsage::SAMPLED | pixl::gfx::TextureUsage::TRANSFER_DST;
    pixl::gfx::Image demoImage = device->createImage(imageDesc);

    pixl::gfx::ImageViewDesc viewDesc{};
    viewDesc.type = pixl::gfx::ImageViewType::TYPE_2D;
    viewDesc.format = imageDesc.format;
    viewDesc.range.aspect = pixl::gfx::AspectMask::COLOR;
    viewDesc.range.levelCount = 1;
    viewDesc.range.layerCount = 1;
    pixl::gfx::ImageView demoView = device->createImageView(demoImage, viewDesc);

    pixl::gfx::SamplerDesc samplerDesc{};
    pixl::gfx::Sampler demoSampler = device->createSampler(samplerDesc);

    if (samplerBinding == std::numeric_limits<pixl::gfx::BindingIndex>::max())
    {
        std::cout << "No material sampler binding available in reflected layout.\n";
    }
    else if (descriptorTable.sets().material.h)
    {
        pixl::gfx::DescriptorWriter writer(*device);
        pixl::gfx::WriteImg imgWrite{};
        imgWrite.view = demoView;
        imgWrite.sampler = demoSampler;
        imgWrite.layout = pixl::gfx::ImageLayout::SHADER_READ_ONLY;
        writer.image(descriptorTable.sets().material, samplerBinding, 0, pixl::gfx::DescType::CombinedImageSampler, imgWrite).submit();
    }

    // Transition texture to shader read.
    {
        pixl::gfx::CommandList *initList = device->createCommandList(pixl::gfx::QueueType::GRAPHICS);
        initList->begin();
        pixl::gfx::ImageBarrier texBarrier{};
        texBarrier.image = demoImage;
        texBarrier.srcStage = pixl::gfx::PipelineStage::TOP;
        texBarrier.dstStage = pixl::gfx::PipelineStage::DRAW;
        texBarrier.dstAccess = pixl::gfx::Access::SHADER_SAMPLED_READ;
        texBarrier.oldLayout = pixl::gfx::ImageLayout::UNDEFINED;
        texBarrier.newLayout = pixl::gfx::ImageLayout::SHADER_READ_ONLY;
        texBarrier.range.aspect = pixl::gfx::AspectMask::COLOR;
        initList->barrier(0, nullptr, 1, &texBarrier);
        initList->end();
        pixl::gfx::CommandList *initLists[] = {initList};
        graphicsQueue->submit(initLists, 1);
        device->waitIdle();
        device->destroy(initList);
    }

    pixl::gfx::CommandList *commandList = device->createCommandList(pixl::gfx::QueueType::GRAPHICS);
    std::vector<pixl::gfx::ImageLayout> backbufferLayouts;
    auto startTime = std::chrono::steady_clock::now();
    bool running = true;

    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT || event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
            {
                running = false;
            }
        }
        if (!running)
        {
            break;
        }

        device->beginFrame();

        uint32_t swapImageCount = device->getSwapchainImageCount();
        if (backbufferLayouts.size() != swapImageCount)
        {
            backbufferLayouts.assign(swapImageCount, pixl::gfx::ImageLayout::UNDEFINED);
        }

        const bool canRender = pipeline.h && descriptorTable.sets().material.h &&
                               samplerBinding != std::numeric_limits<pixl::gfx::BindingIndex>::max();
        bool hasSwapchain = swapImageCount > 0 && canRender && graphicsQueue != nullptr;
        uint32_t imageIndex = 0;
        if (hasSwapchain)
        {
            hasSwapchain = device->acquireNextImage(imageIndex);
        }

        if (hasSwapchain)
        {
            pixl::gfx::Image swapImage = device->getSwapchainImage(imageIndex);
            pixl::gfx::ImageView swapView = device->getSwapchainImageView(imageIndex);
            auto extent = device->getSwapchainExtent();

            commandList->begin();
            pixl::gfx::ImageBarrier toColor{};
            toColor.image = swapImage;
            toColor.srcStage = pixl::gfx::PipelineStage::TOP;
            toColor.dstStage = pixl::gfx::PipelineStage::DRAW;
            toColor.dstAccess = pixl::gfx::Access::COLOR_ATTACHMENT_WRITE;
            const pixl::gfx::ImageLayout previousLayout = backbufferLayouts[imageIndex];
            toColor.oldLayout = previousLayout;
            toColor.newLayout = pixl::gfx::ImageLayout::COLOR_ATTACHMENT;
            toColor.range.aspect = pixl::gfx::AspectMask::COLOR;
            commandList->barrier(0, nullptr, 1, &toColor);

            pixl::gfx::RenderingAttachment colorAttachment{};
            colorAttachment.imageView = &swapView;
            colorAttachment.layout = pixl::gfx::ImageLayout::COLOR_ATTACHMENT;
            colorAttachment.load = false;
            colorAttachment.store = true;
            colorAttachment.clearColor = {0.05f, 0.05f, 0.12f, 1.0f};

            pixl::gfx::RenderingDesc rendering{};
            rendering.color = &colorAttachment;
            rendering.colorCount = 1;
            rendering.width = extent.first;
            rendering.height = extent.second;

            commandList->beginRendering(rendering);
            commandList->bindPipeline(pipeline);
            commandList->bindPipelineLayout(demoPipelineLayout);
            commandList->bindDescriptorSet(demoPipelineLayout, 1, descriptorTable.sets().material);

            const float timeSeconds = std::chrono::duration<float>(std::chrono::steady_clock::now() - startTime).count();
            glm::mat4 mvp = glm::rotate(glm::mat4(1.0f), timeSeconds, glm::vec3(0.0f, 0.0f, 1.0f));
            commandList->pushConstants(demoPipelineLayout, pixl::gfx::ShaderStage::VERTEX, 0, sizeof(glm::mat4), glm::value_ptr(mvp));
            commandList->draw(4, 1, 0, 0);
            commandList->endRendering();

            pixl::gfx::ImageBarrier toPresent{};
            toPresent.image = swapImage;
            toPresent.srcStage = pixl::gfx::PipelineStage::DRAW;
            toPresent.dstStage = pixl::gfx::PipelineStage::BOTTOM;
            toPresent.srcAccess = pixl::gfx::Access::COLOR_ATTACHMENT_WRITE;
            toPresent.dstAccess = pixl::gfx::Access::NONE;
            toPresent.oldLayout = pixl::gfx::ImageLayout::COLOR_ATTACHMENT;
            toPresent.newLayout = pixl::gfx::ImageLayout::PRESENT;
            toPresent.range.aspect = pixl::gfx::AspectMask::COLOR;
            commandList->barrier(0, nullptr, 1, &toPresent);
            if (!backbufferLayouts.empty())
            {
                backbufferLayouts[imageIndex] = pixl::gfx::ImageLayout::PRESENT;
            }

            commandList->end();

            pixl::gfx::CommandList *lists[] = {commandList};
            graphicsQueue->submit(lists, 1);
            device->present(imageIndex);
        }

        device->endFrame();
        if (!hasSwapchain)
        {
            SDL_Delay(10);
        }
    }

    device->waitIdle();
    descriptorTable.release();
    device->destroy(commandList);
    device->destroy(demoSampler);
    device->destroy(demoView);
    device->destroy(demoImage);
    if (pipeline.h)
    {
        device->destroy(pipeline);
    }

    device->destroy(vertexShader);
    device->destroy(fragmentShader);
    layoutCache.clear();

    device.reset();
    if (window)
    {
        SDL_DestroyWindow(window);
    }
    if (sdlInitialized)
    {
        SDL_Quit();
    }

    pixl::core::Core::Quit();

    return 0;
}
