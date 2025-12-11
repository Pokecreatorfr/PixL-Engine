#pragma once

#include <gfx/TextureRef.hpp>
#include <gfx/gfx.hpp>
#include <string>
#include <vector>

namespace pixl::core
{
    struct ResourceManifest
    {
        std::string entryScene;
        std::vector<std::string> startupScripts;
        std::vector<std::string> shaders;
        std::vector<std::string> pipelines;
        std::vector<std::string> scenes;
        std::vector<std::string> scripts;
        std::vector<std::string> audio;
        std::vector<std::string> images;
        std::vector<std::string> materials;
    };

    class Resources
    {
    public:
        static bool LoadManifest(const std::string &path);
        static const ResourceManifest *GetManifest();
        static bool HasManifest();
        static pixl::gfx::TexturePtr LoadTexture(uint32_t id);
        static pixl::gfx::TexturePtr LoadTexture(const std::string &path);

    private:
        static bool ParseManifestJson(const std::string &jsonText, ResourceManifest &outManifest);
        static ResourceManifest manifest_;
        static bool hasManifest_;
    };
}
