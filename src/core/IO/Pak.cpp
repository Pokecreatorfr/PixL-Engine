#include <core/Errors.hpp>
#include <core/IO/Pak.hpp>
#include <filesystem>
#include <algorithm>

using namespace pixl::core::io;

bool Pak::IsInitialized_ = false;
std::map<std::string, PixL_Pak::Context *> Pak::MountedPaks_ = {};

int Pak::Init()
{
    if (IsInitialized_)
    {
        return 0;
    }
    IsInitialized_ = true;
    return 0;
}

int Pak::Quit()
{
    if (!IsInitialized_)
    {
        return 0;
    }
    for (auto &pair : MountedPaks_)
    {
        delete pair.second;
    }
    MountedPaks_.clear();

    IsInitialized_ = false;
    return 0;
}

int Pak::MountPakFile(const std::string &pakFilePath)
{
    if (IsMounted(pakFilePath))
    {
        return 0;
    }

    std::filesystem::path absPath = std::filesystem::absolute(pakFilePath);
    std::error_code ec;
    if (!std::filesystem::exists(absPath, ec))
    {
        PIXL_LOG_AND_RETURN_ERROR(-1, pixl::core::Errc::FileNotFound, "Pak", "Pak file not found: {}", pakFilePath);
    }

    PixL_Pak::Context *context = PixL_Pak::Open(absPath.string());
    if (context == nullptr)
    {
        PIXL_LOG_AND_RETURN_ERROR(-1, pixl::core::Errc::FileNotFound, "Pak", "Failed to open pak file: {}", pakFilePath);
    }

    MountedPaks_[absPath.string()] = context;
    return 0;
}

int Pak::UnmountPakFile(const std::string &pakFilePath)
{
    std::filesystem::path absPath = std::filesystem::absolute(pakFilePath);
    auto it = MountedPaks_.find(absPath.string());
    if (it == MountedPaks_.end())
    {
        PIXL_LOG_AND_RETURN_ERROR(-1, pixl::core::Errc::FileNotFound, "Pak", "Pak file not mounted: {}", pakFilePath);
    }
    delete it->second;
    MountedPaks_.erase(it);

    return 0;
}

bool Pak::IsPathInPak(const std::string &pak, const std::string &filePath)
{
    std::filesystem::path absPak = std::filesystem::absolute(pak);
    auto it = MountedPaks_.find(absPak.string());
    if (it == MountedPaks_.end())
    {
        PIXL_LOG_AND_RETURN_ERROR(-1, pixl::core::Errc::FileNotFound, "Pak", "Pak file not found: {}", pak);
    }

    const auto &context = it->second;
    for (const auto &file : context->files)
    {
        if (file.name == filePath)
        {
            return true;
        }
    }

    return false;
}

std::vector<char> Pak::ReadFileFromPak(const std::string &pakFilePath, const std::string &filePath)
{
    std::vector<char> data = {};

    std::filesystem::path absPak = std::filesystem::absolute(pakFilePath);
    auto it = MountedPaks_.find(absPak.string());
    if (it == MountedPaks_.end())
    {
        return data;
    }

    const auto &context = it->second;
    data = context->readFile(filePath);

    return data;
}

int Pak::MountDirectory(const std::filesystem::path &directory, bool recursive)
{
    if (directory.empty())
    {
        return 0;
    }

    std::error_code ec;
    if (!std::filesystem::exists(directory, ec))
    {
        return 0;
    }

    std::vector<std::filesystem::path> pakFiles;
    if (recursive)
    {
        for (auto it = std::filesystem::recursive_directory_iterator(directory, ec); !ec && it != std::filesystem::recursive_directory_iterator(); ++it)
        {
            if (it->is_regular_file() && it->path().extension() == ".pak")
            {
                pakFiles.push_back(it->path());
            }
        }
    }
    else
    {
        for (auto it = std::filesystem::directory_iterator(directory, ec); !ec && it != std::filesystem::directory_iterator(); ++it)
        {
            if (it->is_regular_file() && it->path().extension() == ".pak")
            {
                pakFiles.push_back(it->path());
            }
        }
    }

    for (const auto &pak : pakFiles)
    {
        MountPakFile(pak.string());
    }

    return 0;
}

std::vector<std::string> Pak::ListMountedPaks()
{
    std::vector<std::string> result;
    result.reserve(MountedPaks_.size());
    for (const auto &p : MountedPaks_)
    {
        result.push_back(p.first);
    }
    std::sort(result.begin(), result.end());
    return result;
}

bool Pak::TryRead(const std::string &virtualPath, std::vector<char> &outData)
{
    for (const auto &pair : MountedPaks_)
    {
        const auto &ctx = pair.second;
        for (const auto &file : ctx->files)
        {
            if (file.name == virtualPath)
            {
                outData = ctx->readFile(virtualPath);
                return !outData.empty();
            }
        }
    }
    return false;
}

bool Pak::HasFile(const std::string &virtualPath)
{
    for (const auto &pair : MountedPaks_)
    {
        const auto &ctx = pair.second;
        for (const auto &file : ctx->files)
        {
            if (file.name == virtualPath)
            {
                return true;
            }
        }
    }
    return false;
}
