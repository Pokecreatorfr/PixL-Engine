#pragma once
#include <PixL-Pak/include/PixL_Pak.hpp>
#include <cstdint>
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
        static int IsMounted(const std::string &pakFilePath)
        {
            return MountedPaks_.find(pakFilePath) != MountedPaks_.end() ? 1 : 0;
        }

        static bool IsPathInPak(const std::string &pak, const std::string &filePath);
        static std::vector<char> ReadFileFromPak(const std::string &pakFilePath, const std::string &filePath);

    private:
        static bool IsInitialized_;
        static std::map<std::string, PixL_Pak::Context *> MountedPaks_;
    };

}