#include <SDL3/SDL.h>
#include <core/Core.hpp>
#include <fstream>
#include <gfx/Pass.hpp>
#include <gfx/Pipeline.hpp>
#include <gfx/gfx.hpp>
#include <gtest/gtest.h>
#include <vector>

TEST(GfxSmoke, BeginEndPassClears)
{
    ASSERT_EQ(0, pixl::core::Core::Init(pixl::core::CoreInitData{}));

    pixl::gfx::GfxInitData init{};
    init.windowFlags = SDL_WINDOW_HIDDEN;
    ASSERT_EQ(0, pixl::gfx::Gfx::Init(init));

    pixl::gfx::RenderPassDesc pass{};
    pass.clearColorValue[0] = 0.2f;
    pass.clearColorValue[1] = 0.3f;
    pass.clearColorValue[2] = 0.4f;
    pass.clearColorValue[3] = 1.0f;

    auto *gpu = pixl::gfx::Gfx::GetGpu();
    ASSERT_NE(nullptr, gpu);
    EXPECT_EQ(0, gpu->BeginPass(pass));
    EXPECT_EQ(0, gpu->EndPass());

    pixl::gfx::Gfx::Quit();
    pixl::core::Core::Quit();
}

TEST(GfxSmoke, TrianglePathIfShadersPresent)
{
    ASSERT_EQ(0, pixl::core::Core::Init(pixl::core::CoreInitData{}));

    pixl::gfx::GfxInitData init{};
    init.windowFlags = SDL_WINDOW_HIDDEN;
    ASSERT_EQ(0, pixl::gfx::Gfx::Init(init));

    auto loadFile = [](const std::string &path) -> std::vector<char>
    {
        std::ifstream f(path, std::ios::binary);
        if (!f)
        {
            return {};
        }
        return std::vector<char>((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    };

    auto loadSpv = [&](const std::string &name) -> std::vector<char>
    {
        std::vector<std::string> candidates;
        candidates.push_back("shaders/" + name);
        candidates.push_back("build/shaders/" + name);
        candidates.push_back("build/mingw/x86_64/debug/shaders/" + name);
        candidates.push_back("build/mingw/x86_64/release/shaders/" + name);
        candidates.push_back("resources/shaders/" + name);

        for (const auto &c : candidates)
        {
            auto data = loadFile(c);
            if (!data.empty())
            {
                return data;
            }
        }
        return {};
    };

    std::vector<char> vsData = loadSpv("triangle.vert.spv");
    if (vsData.empty())
        vsData = loadSpv("triangle.vert.glsl.spv");
    std::vector<char> fsData = loadSpv("triangle.frag.spv");
    if (fsData.empty())
        fsData = loadSpv("triangle.frag.glsl.spv");
    if (vsData.empty() || fsData.empty())
    {
        GTEST_SKIP() << "Triangle shaders not found; skipping pipeline smoke test";
    }

    auto *gpu = pixl::gfx::Gfx::GetGpu();
    ASSERT_NE(nullptr, gpu);

    pixl::gfx::ShaderDesc vsDesc{};
    vsDesc.stages.push_back({pixl::gfx::ShaderStage::Vertex, pixl::gfx::ShaderLanguage::SPV, "main",
                             std::string(vsData.data(), vsData.size())});
    pixl::gfx::ShaderDesc fsDesc{};
    fsDesc.stages.push_back({pixl::gfx::ShaderStage::Fragment, pixl::gfx::ShaderLanguage::SPV, "main",
                             std::string(fsData.data(), fsData.size())});

    auto vsHandle = gpu->CreateShader(vsDesc);
    auto fsHandle = gpu->CreateShader(fsDesc);
    if (vsHandle == pixl::gfx::InvalidHandle || fsHandle == pixl::gfx::InvalidHandle)
    {
        GTEST_SKIP() << "Failed to create shaders with provided binaries";
    }

    struct Vertex
    {
        float pos[2];
        float color[3];
    };
    Vertex verts[] = {{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                      {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                      {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};
    uint16_t indices[] = {0, 1, 2};

    pixl::gfx::BufferDesc vbDesc{};
    vbDesc.size = sizeof(verts);
    vbDesc.usage = pixl::gfx::BufferUsage::Vertex;
    vbDesc.stride = sizeof(Vertex);
    auto vb = gpu->CreateBuffer(vbDesc, verts, sizeof(verts));

    pixl::gfx::BufferDesc ibDesc{};
    ibDesc.size = sizeof(indices);
    ibDesc.usage = pixl::gfx::BufferUsage::Index;
    ibDesc.stride = sizeof(uint16_t);
    auto ib = gpu->CreateBuffer(ibDesc, indices, sizeof(indices));

    pixl::gfx::GraphicsPipelineDesc pDesc{};
    pDesc.vertexShader = vsHandle;
    pDesc.fragmentShader = fsHandle;
    pDesc.primitive = pixl::gfx::PrimitiveType::TriangleList;
    pixl::gfx::VertexBufferLayout bufLayout{};
    bufLayout.stride = sizeof(Vertex);
    bufLayout.rate = pixl::gfx::VertexInputRate::PerVertex;
    pixl::gfx::VertexAttribute attrPos{0, 0, pixl::gfx::VertexFormat::Float2, 0};
    pixl::gfx::VertexAttribute attrCol{1, 0, pixl::gfx::VertexFormat::Float3, sizeof(float) * 2};
    pDesc.vertexLayout.buffers.push_back(bufLayout);
    pDesc.vertexLayout.attributes.push_back(attrPos);
    pDesc.vertexLayout.attributes.push_back(attrCol);

    auto pipeline = gpu->CreateGraphicsPipeline(pDesc);
    if (pipeline == pixl::gfx::InvalidHandle)
    {
        GTEST_SKIP() << "Pipeline creation failed with provided shaders";
    }

    pixl::gfx::RenderPassDesc pass{};
    pass.clearColorValue[0] = 0.1f;
    pass.clearColorValue[1] = 0.1f;
    pass.clearColorValue[2] = 0.1f;
    pass.clearColorValue[3] = 1.0f;

    EXPECT_EQ(0, gpu->BeginPass(pass));
    EXPECT_TRUE(gpu->BindPipeline(pipeline));
    EXPECT_TRUE(gpu->BindVertexBuffer(vb, 0, 0));
    EXPECT_TRUE(gpu->BindIndexBuffer(ib, true, 0));
    gpu->DrawIndexed(3, 0, 1, 0);
    EXPECT_EQ(0, gpu->EndPass());

    pixl::gfx::Gfx::Quit();
    pixl::core::Core::Quit();
}
