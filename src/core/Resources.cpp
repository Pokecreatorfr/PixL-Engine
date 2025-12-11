#include <core/Resources.hpp>

#include <core/Errors.hpp>
#include <core/IO/IO.hpp>
#include <gfx/TextureCache.hpp>
#include <nlohmann/json.hpp>

using namespace pixl;
using namespace pixl::core;

ResourceManifest Resources::manifest_{};
bool Resources::hasManifest_ = false;

namespace
{
    std::vector<std::string> ReadStringArray(const nlohmann::json &obj, const char *key)
    {
        std::vector<std::string> out;
        if (!obj.contains(key))
        {
            return out;
        }
        if (!obj[key].is_array())
        {
            return out;
        }
        for (const auto &v : obj[key])
        {
            if (v.is_string())
            {
                out.push_back(v.get<std::string>());
            }
        }
        return out;
    }
}

bool Resources::ParseManifestJson(const std::string &jsonText, ResourceManifest &outManifest)
{
    nlohmann::json j;
    try
    {
        j = nlohmann::json::parse(jsonText);
    }
    catch (const std::exception &e)
    {
        PIXL_LOG_ERROR_WITH_CAUSE(pixl::core::Errc::InvalidParameter, "Resources", e.what(), "Failed to parse manifest JSON");
        return false;
    }

    outManifest.entryScene = j.value("entryScene", "");
    outManifest.startupScripts = ReadStringArray(j, "startupScripts");
    outManifest.shaders = ReadStringArray(j, "shaders");
    outManifest.scenes = ReadStringArray(j, "scenes");
    outManifest.scripts = ReadStringArray(j, "scripts");
    outManifest.audio = ReadStringArray(j, "audio");
    outManifest.images = ReadStringArray(j, "images");

    return true;
}

bool Resources::LoadManifest(const std::string &path)
{
    std::error_code ec;
    if (!std::filesystem::exists(path, ec))
    {
        hasManifest_ = false;
        return false;
    }

    std::vector<char> data = pixl::core::io::IO::ReadFileToBuffer(path);
    if (data.empty())
    {
        hasManifest_ = false;
        return false;
    }

    std::string text(data.begin(), data.end());
    ResourceManifest manifest{};
    if (!ParseManifestJson(text, manifest))
    {
        hasManifest_ = false;
        return false;
    }

    manifest_ = manifest;
    hasManifest_ = true;
    return true;
}

const ResourceManifest *Resources::GetManifest()
{
    return hasManifest_ ? &manifest_ : nullptr;
}

bool Resources::HasManifest()
{
    return hasManifest_;
}

pixl::gfx::TexturePtr pixl::core::Resources::LoadTexture(const std::string &path)
{
    auto *cache = pixl::gfx::Gfx::GetTextureCache();
    if (!cache)
    {
        PIXL_LOG_ERROR(pixl::core::Errc::UninitialisedSubsystem, "Resources", "TextureCache not available");
        return {};
    }
    return cache->GetTexture(path);
}

pixl::gfx::TexturePtr pixl::core::Resources::LoadTexture(uint32_t id)
{
    const auto *manifest = GetManifest();
    if (!manifest || id >= manifest->images.size())
    {
        PIXL_LOG_ERROR(pixl::core::Errc::InvalidParameter, "Resources", "Texture id {} out of bounds", id);
        return {};
    }

    return LoadTexture(manifest->images[id]);
}
