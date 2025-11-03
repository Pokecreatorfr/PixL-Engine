#include <core/Errors.hpp>
#include <core/IO/Pak.hpp>

using namespace pixl::core::io;

bool Pak::IsInitialized_ = false;
std::map<std::string, PixL_Pak::Context *> Pak::MountedPaks_ = {};

int Pak::Init()
{
    return 0;
}

int Pak::Quit()
{
    if (!IsInitialized_)
    {
        return 0;
    }
    IsInitialized_ = false;
    return 0;
}

int Pak::MountPakFile(const std::string &pakFilePath)
{
    if (IsMounted(pakFilePath))
    {
        return 0;
    }

    PixL_Pak::Context *context = PixL_Pak::Open(pakFilePath);
    if (context == nullptr)
    {
        PIXL_LOG_AND_RETURN_ERROR(-1, pixl::core::Errc::FileNotFound, "Pak", "Failed to open pak file: {}", pakFilePath);
    }

    MountedPaks_[pakFilePath] = context;
    return 0;
}

int Pak::UnmountPakFile(const std::string &pakFilePath)
{
    auto it = MountedPaks_.find(pakFilePath);
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
    auto it = MountedPaks_.find(pak);
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

    auto it = MountedPaks_.find(pakFilePath);
    if (it == MountedPaks_.end())
    {
        return data;
    }

    const auto &context = it->second;
    data = context->readFile(filePath);

    return data;
}
