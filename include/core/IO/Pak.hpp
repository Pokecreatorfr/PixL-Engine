#pragma once
#include <PixL-Pak/include/PixL_Pak.hpp>
#include <cstdint>
#include <filesystem>
#include <map>
#include <string>
#include <vector>

namespace pixl::core::io
{

    class Pak
    {
    public:
        static int Init();
        static int Quit();
        static int MountPakFile(const std::string &pakFilePath);
        static int UnmountPakFile(const std::string &pakFilePath);
        static int MountDirectory(const std::filesystem::path &directory, bool recursive = false);
        static std::vector<std::string> ListMountedPaks();
        static bool TryRead(const std::string &virtualPath, std::vector<char> &outData);
        static bool HasFile(const std::string &virtualPath);

        static int IsMounted(const std::string &pakFilePath)
        {
            return MountedPaks_.find(std::filesystem::absolute(pakFilePath).string()) != MountedPaks_.end() ? 1 : 0;
        }

        static bool IsPathInPak(const std::string &pak, const std::string &filePath);
        static std::vector<char> ReadFileFromPak(const std::string &pakFilePath, const std::string &filePath);

    private:
        static bool IsInitialized_;
        static std::map<std::string, PixL_Pak::Context *> MountedPaks_;
    };

}
