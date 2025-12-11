#pragma once

#include <gfx/Gpu.hpp>
#include <gfx/PipelineLibrary.hpp>
#include <gfx/PipelineRef.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace pixl::gfx
{
    class PipelineCache
    {
    public:
        explicit PipelineCache(Gpu *gpu);

        void SetSearchPaths(std::vector<std::string> paths);
        PipelinePtr GetPipeline(const PipelineDescriptor &desc);
        void Clear();

    private:
        struct State
        {
            explicit State(Gpu *gpuPtr) : gpu(gpuPtr) {}

            Gpu *gpu = nullptr;
            std::unordered_map<std::string, std::weak_ptr<PipelineRef>> cache;
            std::vector<std::string> searchPaths{
                "",
                "shaders",
                "resources/shaders",
                "build/shaders",
                "build/mingw/x86_64/debug/shaders",
                "build/mingw/x86_64/release/shaders"};
        };

        struct PipelineBuildResult
        {
            PipelineHandle pipeline = InvalidHandle;
            ShaderHandle vertexShader = InvalidHandle;
            ShaderHandle fragmentShader = InvalidHandle;
        };

        std::vector<char> LoadShaderFile(const std::string &path);
        ShaderHandle LoadShader(const std::string &path, ShaderStage stage);
        PipelineBuildResult CreatePipeline(const PipelineDescriptor &desc);
        PipelinePtr MakeManagedPipeline(const std::string &key, const PipelineBuildResult &built);

        std::shared_ptr<State> state_;
    };
}
