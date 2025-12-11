#include <gfx/PipelineLibrary.hpp>

#include <core/Errors.hpp>
#include <core/IO/IO.hpp>
#include <filesystem>
#include <sstream>
#include <cstdlib>
#include <algorithm>

using namespace pixl::gfx;

Gpu *PipelineLibrary::gpu_ = nullptr;
std::map<std::string, PipelineLibrary::PipelineEntry> PipelineLibrary::pipelines_{};
std::vector<std::string> PipelineLibrary::searchPaths_{
    "",
    "shaders",
    "resources/shaders",
    "build/shaders",
    "build/mingw/x86_64/debug/shaders",
    "build/mingw/x86_64/release/shaders"};

void PipelineLibrary::SetGpu(Gpu *gpu)
{
    gpu_ = gpu;
}

void PipelineLibrary::SetSearchPaths(std::vector<std::string> paths)
{
    searchPaths_ = std::move(paths);
}

std::string PipelineLibrary::MakeKey(const PipelineDescriptor &desc)
{
    std::ostringstream oss;
    oss << desc.vertexShader << "|" << desc.fragmentShader << "|" << static_cast<int>(desc.primitive) << "|"
        << (desc.depthTest ? "D1" : "D0") << (desc.depthWrite ? "W1" : "W0");
    for (const auto &buf : desc.vertexLayout.buffers)
    {
        oss << "|B" << buf.stride << ":" << static_cast<int>(buf.rate);
    }
    for (const auto &attr : desc.vertexLayout.attributes)
    {
        oss << "|A" << attr.location << ":" << attr.bufferSlot << ":" << static_cast<int>(attr.format) << ":" << attr.offset;
    }
    return oss.str();
}

std::vector<char> PipelineLibrary::LoadShaderFile(const std::string &path)
{
    auto tryLoad = [](const std::string &p) -> std::vector<char> {
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
    for (const auto &root : searchPaths_)
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

ShaderHandle PipelineLibrary::CreateShaderFromFile(const std::string &path, ShaderStage stage)
{
    if (!gpu_)
    {
        PIXL_LOG_AND_RETURN_ERROR(InvalidHandle, pixl::core::Errc::UninitialisedSubsystem, "PipelineLibrary", "GPU not set");
    }

    std::string finalPath = path;
    std::filesystem::path p(path);
    std::string ext = p.extension().string();
    auto toLower = [](std::string s) {
        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return s;
    };
    ext = toLower(ext);

    // If GLSL provided, compile to SPV with glslc.
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
            PIXL_LOG_AND_RETURN_ERROR(InvalidHandle, pixl::core::Errc::UnknownError, "PipelineLibrary", "glslc failed for {}", path);
        }
        finalPath = outPath.string();
    }

    auto bytes = LoadShaderFile(finalPath);
    if (bytes.empty())
    {
        PIXL_LOG_AND_RETURN_ERROR(InvalidHandle, pixl::core::Errc::FileNotFound, "PipelineLibrary", "Shader not found: {}", finalPath);
    }

    ShaderDesc sdesc{};
    ShaderStageSource src{};
    src.language = ShaderLanguage::SPV;
    src.stage = stage;
    src.entryPoint = "main";
    src.source.assign(bytes.begin(), bytes.end());
    sdesc.stages.push_back(src);
    return gpu_->CreateShader(sdesc);
}

PipelineHandle PipelineLibrary::CreateGraphicsPipeline(const PipelineDescriptor &desc, ShaderHandle vs, ShaderHandle fs)
{
    if (!gpu_)
    {
        PIXL_LOG_AND_RETURN_ERROR(InvalidHandle, pixl::core::Errc::UninitialisedSubsystem, "PipelineLibrary", "GPU not set");
    }

    GraphicsPipelineDesc pdesc{};
    pdesc.vertexShader = vs;
    pdesc.fragmentShader = fs;
    pdesc.vertexLayout = desc.vertexLayout;
    pdesc.primitive = desc.primitive;
    pdesc.depthTest = desc.depthTest;
    pdesc.depthWrite = desc.depthWrite;

    // Provide a default layout if none specified (pos float2 + color float3)
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

    return gpu_->CreateGraphicsPipeline(pdesc);
}

PipelineHandle PipelineLibrary::LoadGraphicsPipeline(const PipelineDescriptor &desc, bool forceReload)
{
    std::string key = MakeKey(desc);
    auto found = pipelines_.find(key);
    if (found != pipelines_.end())
    {
        if (!forceReload)
        {
            return found->second.pipeline;
        }
        if (gpu_)
        {
            if (found->second.pipeline != InvalidHandle)
                gpu_->DestroyPipeline(found->second.pipeline);
            if (found->second.vs != InvalidHandle)
                gpu_->DestroyShader(found->second.vs);
            if (found->second.fs != InvalidHandle)
                gpu_->DestroyShader(found->second.fs);
        }
        pipelines_.erase(found);
    }

    ShaderHandle vs = InvalidHandle;
    ShaderHandle fs = InvalidHandle;

    if (!desc.computeShader.empty())
    {
        PIXL_LOG_ERROR(pixl::core::Errc::InvalidParameter, "PipelineLibrary", "Compute pipelines are not supported by backend yet");
        return InvalidHandle;
    }

    vs = CreateShaderFromFile(desc.vertexShader, ShaderStage::Vertex);
    fs = CreateShaderFromFile(desc.fragmentShader, ShaderStage::Fragment);

    if (vs == InvalidHandle || fs == InvalidHandle)
    {
        return InvalidHandle;
    }

    PipelineHandle pipeline = CreateGraphicsPipeline(desc, vs, fs);
    if (pipeline == InvalidHandle)
    {
        gpu_->DestroyShader(vs);
        gpu_->DestroyShader(fs);
        return InvalidHandle;
    }

    pipelines_[key] = PipelineEntry{vs, fs, pipeline};
    return pipeline;
}

PipelineHandle PipelineLibrary::FindPipeline(const std::string &key)
{
    auto it = pipelines_.find(key);
    if (it != pipelines_.end())
    {
        return it->second.pipeline;
    }
    return InvalidHandle;
}

void PipelineLibrary::Shutdown()
{
    if (!gpu_)
    {
        pipelines_.clear();
        return;
    }

    for (auto &p : pipelines_)
    {
        if (p.second.pipeline != InvalidHandle)
        {
            gpu_->DestroyPipeline(p.second.pipeline);
        }
        if (p.second.vs != InvalidHandle)
        {
            gpu_->DestroyShader(p.second.vs);
        }
        if (p.second.fs != InvalidHandle)
        {
            gpu_->DestroyShader(p.second.fs);
        }
    }
    pipelines_.clear();
}
