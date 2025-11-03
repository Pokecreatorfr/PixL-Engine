#pragma once
#include <core/IO/Pak.hpp>
#include <core/Log.hpp>
#include <cstdio>

namespace pixl::core::io
{

    class IO
    {
    public:
        static int Init();
        static int Quit();

        static std::vector<char> ReadFileToBuffer(const std::string &filepath);
        static bool WriteBufferToFile(const std::string &filepath, const std::vector<char> &buffer);
        static bool FileExists(const std::string &filepath);

    private:
        static bool IsInitialized_;
    };

}