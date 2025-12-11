#include <core/Errors.hpp>
#include <core/IO/IO.hpp>
#include <gfx/PipelineCache.hpp>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <sstream>

using namespace pixl::gfx;

PipelineCache::PipelineCache(Gpu *gpu)
    : state_(std::make_shared<State>(gpu))
{
}

void PipelineCache::SetSearchPaths(std::vector<std::string> paths)
{
    if (!state_)
    {
        return;
    }
    state_->searchPaths = std::move(paths);
}

std::vector<char> PipelineCache::LoadShaderFile(const std::string &path)
{
    auto tryLoad = [](const std::string &p) -> std::vector<char>
    {
        if (!pixl::core::io::IO::FileExists(p))
        {
            return {};
        }
        return pixl::core::io::IO::ReadFileToBuffer(p);
    };

    auto data = tryLoad(path);
    if (!data.empty())
    {
        return data;
    }

    std::filesystem::path p(path);
    std::string filename = p.filename().string();
    for (const auto &root : state_->searchPaths)
    {
        std::filesystem::path candidate = root.empty() ? std::filesystem::path(filename) : std::filesystem::path(root) / filename;
        data = tryLoad(candidate.string());
        if (!data.empty())
        {
            return data;
        }
    }
    return {};
}

ShaderHandle PipelineCache::LoadShader(const std::string &path, ShaderStage stage)
{
    if (!state_ || !state_->gpu)
    {
        PIXL_LOG_ERROR(pixl::core::Errc::UninitialisedSubsystem, "PipelineCache", "GPU not set");
        return InvalidHandle;
    }

    std::string finalPath = path;
    std::filesystem::path p(path);
    std::string ext = p.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c)
                   { return static_cast<char>(std::tolower(c)); });

    if (ext == ".glsl" || ext == ".vert" || ext == ".frag" || ext == ".vs" || ext == ".fs")
    {
        std::filesystem::path outPath = p;
        outPath += ".spv";
        std::stringstream cmd;
        cmd << "glslc -std=450 -O -fshader-stage=" << (stage == ShaderStage::Vertex ? "vert" : "frag")
            << " -o \"" << outPath.string() << "\" \"" << path << "\"";
        int ret = std::system(cmd.str().c_str());
        if (ret != 0)
        {
            PIXL_LOG_AND_RETURN_ERROR(InvalidHandle, pixl::core::Errc::UnknownError, "PipelineCache", "glslc failed for {}", path);
        }
        finalPath = outPath.string();
    }

    auto bytes = LoadShaderFile(finalPath);
    if (bytes.empty())
    {
        PIXL_LOG_AND_RETURN_ERROR(InvalidHandle, pixl::core::Errc::FileNotFound, "PipelineCache", "Shader not found: {}", finalPath);
    }

    ShaderDesc desc{};
    ShaderStageSource src{};
    src.language = ShaderLanguage::SPV;
    src.stage = stage;
    src.entryPoint = "main";
    src.source.assign(bytes.begin(), bytes.end());
    desc.stages.push_back(src);
    return state_->gpu->CreateShader(desc);
}

PipelineCache::PipelineBuildResult PipelineCache::CreatePipeline(const PipelineDescriptor &desc)
{
    PipelineBuildResult built{};
    if (!state_ || !state_->gpu)
    {
        PIXL_LOG_ERROR(pixl::core::Errc::UninitialisedSubsystem, "PipelineCache", "GPU not set");
        return built;
    }

    if (!desc.computeShader.empty())
    {
        PIXL_LOG_ERROR(pixl::core::Errc::InvalidParameter, "PipelineCache", "Compute pipelines are not supported by backend yet");
        return built;
    }

    ShaderHandle vs = LoadShader(desc.vertexShader, ShaderStage::Vertex);
    ShaderHandle fs = LoadShader(desc.fragmentShader, ShaderStage::Fragment);

    if (vs == InvalidHandle || fs == InvalidHandle)
    {
        if (vs != InvalidHandle)
        {
            state_->gpu->DestroyShader(vs);
        }
        if (fs != InvalidHandle)
        {
            state_->gpu->DestroyShader(fs);
        }
        return built;
    }

    GraphicsPipelineDesc pdesc{};
    pdesc.vertexShader = vs;
    pdesc.fragmentShader = fs;
    pdesc.vertexLayout = desc.vertexLayout;
    pdesc.primitive = desc.primitive;
    pdesc.depthTest = desc.depthTest;
    pdesc.depthWrite = desc.depthWrite;

    if (pdesc.vertexLayout.buffers.empty() && pdesc.vertexLayout.attributes.empty())
    {
        VertexBufferLayout buf{};
        buf.stride = sizeof(float) * 5;
        buf.rate = VertexInputRate::PerVertex;

        VertexAttribute pos{};
        pos.location = 0;
        pos.bufferSlot = 0;
        pos.format = VertexFormat::Float2;
        pos.offset = 0;

        VertexAttribute col{};
        col.location = 1;
        col.bufferSlot = 0;
        col.format = VertexFormat::Float3;
        col.offset = sizeof(float) * 2;

        pdesc.vertexLayout.buffers.push_back(buf);
        pdesc.vertexLayout.attributes.push_back(pos);
        pdesc.vertexLayout.attributes.push_back(col);
    }

    built.pipeline = state_->gpu->CreateGraphicsPipeline(pdesc);
    if (built.pipeline == InvalidHandle)
    {
        state_->gpu->DestroyShader(vs);
        state_->gpu->DestroyShader(fs);
        return built;
    }

    built.vertexShader = vs;
    built.fragmentShader = fs;
    return built;
}

PipelinePtr PipelineCache::MakeManagedPipeline(const std::string &key, const PipelineBuildResult &built)
{
    if (!state_ || built.pipeline == InvalidHandle)
    {
        return nullptr;
    }

    auto state = state_;
    PipelinePtr ptr(new PipelineRef(built.pipeline, built.vertexShader, built.fragmentShader), [state, key](PipelineRef *ref)
                    {
        if (state->gpu)
        {
            if (ref->pipeline != InvalidHandle)
            {
                state->gpu->DestroyPipeline(ref->pipeline);
            }
            if (ref->vertexShader != InvalidHandle)
            {
                state->gpu->DestroyShader(ref->vertexShader);
            }
            if (ref->fragmentShader != InvalidHandle)
            {
                state->gpu->DestroyShader(ref->fragmentShader);
            }
        }

        auto it = state->cache.find(key);
        if (it != state->cache.end())
        {
            auto locked = it->second.lock();
            if (!locked || locked.get() == ref)
            {
                state->cache.erase(it);
            }
        }

        delete ref; });

    state_->cache[key] = ptr;
    return ptr;
}

PipelinePtr PipelineCache::GetPipeline(const PipelineDescriptor &desc)
{
    if (!state_)
    {
        return nullptr;
    }

    std::string key = PipelineLibrary::MakeKey(desc);
    auto it = state_->cache.find(key);
    if (it != state_->cache.end())
    {
        if (auto existing = it->second.lock())
        {
            return existing;
        }
    }

    auto built = CreatePipeline(desc);
    if (built.pipeline == InvalidHandle)
    {
        return nullptr;
    }

    return MakeManagedPipeline(key, built);
}

void PipelineCache::Clear()
{
    if (!state_)
    {
        return;
    }

    std::vector<PipelinePtr> livePipelines;
    livePipelines.reserve(state_->cache.size());

    for (auto &kv : state_->cache)
    {
        if (auto pipeline = kv.second.lock())
        {
            livePipelines.push_back(std::move(pipeline));
        }
    }

    state_->cache.clear();
}
