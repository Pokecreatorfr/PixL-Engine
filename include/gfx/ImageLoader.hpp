#pragma once

#include <vector>
#include <string>

namespace pixl::gfx
{
    struct LoadedImage
    {
        int width = 0;
        int height = 0;
        int channels = 0;
        std::vector<unsigned char> pixels;
    };

    class ImageLoader
    {
    public:
        static bool LoadFromMemory(const std::vector<char> &buffer, LoadedImage &outImage, bool flipY = true);
    };
}

