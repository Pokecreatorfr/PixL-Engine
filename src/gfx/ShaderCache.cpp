#include <gfx/gfx_reflect.hpp>
#include <gfx/gfx_shader_cache.hpp>

#include <filesystem>
#include <fstream>

namespace pixl::gfx
{
    namespace
    {
        constexpr uint32_t kCacheMagic = 0x50534843; // 'PSHC' , le magicnumber
        constexpr uint32_t kCacheVersion = 1;

        std::filesystem::path cacheDirectory()
        {
            std::filesystem::path path = std::filesystem::path("cache") / "shaders";
            return path;
        }

        std::filesystem::path cacheFilePath(uint64_t spirvHash)
        {
            auto dir = cacheDirectory();
            return dir / (std::to_string(spirvHash) + ".pixlshader");
        }
    }

    ShaderLayout ShaderCache::loadOrCreate(std::span<const uint32_t> spirvWords, std::string_view)
    {
        ShaderLayout layout{};
        if (spirvWords.empty())
        {
            return layout;
        }

        ShaderLayout reflected = reflectShaderLayout(spirvWords);
        const uint64_t spirvHash = reflected.spirvHash;

        ShaderLayout cached = loadFromDisk(spirvHash);
        if (!cached.sets.empty() || !cached.pushConstants.empty())
        {
            if (cached.layoutHash == reflected.layoutHash && cached.spirvHash == spirvHash)
            {
                return cached;
            }
        }

        writeToDisk(reflected);
        return reflected;
    }

    ShaderLayout ShaderCache::loadFromDisk(uint64_t spirvHash)
    {
        std::filesystem::path path = cacheFilePath(spirvHash);
        if (!std::filesystem::exists(path))
        {
            return {};
        }

        std::ifstream file(path, std::ios::binary);
        if (!file)
        {
            return {};
        }

        struct Header
        {
            uint32_t magic;
            uint32_t version;
            uint64_t spirvHash;
            uint64_t layoutHash;
            uint32_t setCount;
            uint32_t pushCount;
        } header{};

        file.read(reinterpret_cast<char *>(&header), sizeof(header));
        if (!file || header.magic != kCacheMagic || header.version != kCacheVersion || header.spirvHash != spirvHash)
        {
            return {};
        }

        ShaderLayout layout{};
        layout.layoutHash = header.layoutHash;
        layout.spirvHash = header.spirvHash;
        layout.sets.resize(header.setCount);
        for (uint32_t i = 0; i < header.setCount; ++i)
        {
            uint32_t setIndex = 0;
            uint32_t bindingCount = 0;
            file.read(reinterpret_cast<char *>(&setIndex), sizeof(setIndex));
            file.read(reinterpret_cast<char *>(&bindingCount), sizeof(bindingCount));
            layout.sets[i].set = setIndex;
            layout.sets[i].bindings.resize(bindingCount);
            for (uint32_t b = 0; b < bindingCount; ++b)
            {
                BindingInfo info{};
                uint32_t type = 0;
                uint32_t bindless = 0;
                file.read(reinterpret_cast<char *>(&info.binding), sizeof(info.binding));
                file.read(reinterpret_cast<char *>(&type), sizeof(type));
                file.read(reinterpret_cast<char *>(&info.count), sizeof(info.count));
                file.read(reinterpret_cast<char *>(&info.stages), sizeof(info.stages));
                file.read(reinterpret_cast<char *>(&bindless), sizeof(bindless));
                info.type = static_cast<DescType>(type);
                info.bindless = bindless != 0;
                layout.sets[i].bindings[b] = info;
            }
        }

        layout.pushConstants.resize(header.pushCount);
        for (uint32_t i = 0; i < header.pushCount; ++i)
        {
            file.read(reinterpret_cast<char *>(&layout.pushConstants[i].stages), sizeof(layout.pushConstants[i].stages));
            file.read(reinterpret_cast<char *>(&layout.pushConstants[i].offset), sizeof(layout.pushConstants[i].offset));
            file.read(reinterpret_cast<char *>(&layout.pushConstants[i].size), sizeof(layout.pushConstants[i].size));
        }

        if (!file)
        {
            return {};
        }

        return layout;
    }

    void ShaderCache::writeToDisk(const ShaderLayout &layout)
    {
        std::filesystem::path dir = cacheDirectory();
        std::error_code ec{};
        std::filesystem::create_directories(dir, ec);

        std::filesystem::path path = cacheFilePath(layout.spirvHash);
        std::ofstream file(path, std::ios::binary | std::ios::trunc);
        if (!file)
        {
            return;
        }

        struct Header
        {
            uint32_t magic;
            uint32_t version;
            uint64_t spirvHash;
            uint64_t layoutHash;
            uint32_t setCount;
            uint32_t pushCount;
        } header{};

        header.magic = kCacheMagic;
        header.version = kCacheVersion;
        header.spirvHash = layout.spirvHash;
        header.layoutHash = layout.layoutHash;
        header.setCount = static_cast<uint32_t>(layout.sets.size());
        header.pushCount = static_cast<uint32_t>(layout.pushConstants.size());

        file.write(reinterpret_cast<const char *>(&header), sizeof(header));
        for (const SetLayoutInfo &set : layout.sets)
        {
            uint32_t setIndex = set.set;
            uint32_t bindingCount = static_cast<uint32_t>(set.bindings.size());
            file.write(reinterpret_cast<const char *>(&setIndex), sizeof(setIndex));
            file.write(reinterpret_cast<const char *>(&bindingCount), sizeof(bindingCount));
            for (const BindingInfo &binding : set.bindings)
            {
                uint32_t type = static_cast<uint32_t>(binding.type);
                uint32_t bindless = binding.bindless ? 1u : 0u;
                file.write(reinterpret_cast<const char *>(&binding.binding), sizeof(binding.binding));
                file.write(reinterpret_cast<const char *>(&type), sizeof(type));
                file.write(reinterpret_cast<const char *>(&binding.count), sizeof(binding.count));
                file.write(reinterpret_cast<const char *>(&binding.stages), sizeof(binding.stages));
                file.write(reinterpret_cast<const char *>(&bindless), sizeof(bindless));
            }
        }

        for (const PushConstantRangeInfo &range : layout.pushConstants)
        {
            file.write(reinterpret_cast<const char *>(&range.stages), sizeof(range.stages));
            file.write(reinterpret_cast<const char *>(&range.offset), sizeof(range.offset));
            file.write(reinterpret_cast<const char *>(&range.size), sizeof(range.size));
        }
    }
}
