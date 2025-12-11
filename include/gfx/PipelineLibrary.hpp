#pragma once

#include <gfx/Gpu.hpp>
#include <gfx/Pipeline.hpp>
#include <map>
#include <string>
#include <vector>

namespace pixl::gfx
{
    struct PipelineDescriptor
    {
        std::string name;
        std::string vertexShader;   // path or filename
        std::string fragmentShader; // optional (leave empty for compute-only)
        std::string computeShader;  // optional (not yet supported by backend)
        VertexLayout vertexLayout{};
        PrimitiveType primitive = PrimitiveType::TriangleList;
        bool depthTest = false;
        bool depthWrite = false;
    };

    class PipelineLibrary
    {
    public:
        static void SetGpu(Gpu *gpu);
        static void SetSearchPaths(std::vector<std::string> paths);
        static std::string MakeKey(const PipelineDescriptor &desc);
        static PipelineHandle LoadGraphicsPipeline(const PipelineDescriptor &desc, bool forceReload = false);
        static PipelineHandle FindPipeline(const std::string &key);
        static void Shutdown();

    private:
        struct PipelineEntry
        {
            ShaderHandle vs = InvalidHandle;
            ShaderHandle fs = InvalidHandle;
            PipelineHandle pipeline = InvalidHandle;
        };

        static std::vector<char> LoadShaderFile(const std::string &path);
        static ShaderHandle CreateShaderFromFile(const std::string &path, ShaderStage stage);
        static PipelineHandle CreateGraphicsPipeline(const PipelineDescriptor &desc, ShaderHandle vs, ShaderHandle fs);

        static std::map<std::string, PipelineEntry> pipelines_;
        static std::vector<std::string> searchPaths_;
        static Gpu *gpu_;
    };
}
