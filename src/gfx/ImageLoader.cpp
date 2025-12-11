#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <gfx/ImageLoader.hpp>

using namespace pixl::gfx;

bool ImageLoader::LoadFromMemory(const std::vector<char> &buffer, LoadedImage &outImage, bool flipY)
{
    if (buffer.empty())
    {
        return false;
    }

    stbi_set_flip_vertically_on_load(flipY ? 1 : 0);

    int w = 0, h = 0, ch = 0;
    unsigned char *data = stbi_load_from_memory(reinterpret_cast<const unsigned char *>(buffer.data()),
                                                static_cast<int>(buffer.size()),
                                                &w, &h, &ch, STBI_rgb_alpha);
    if (!data)
    {
        return false;
    }

    outImage.width = w;
    outImage.height = h;
    outImage.channels = 4;
    outImage.pixels.assign(data, data + (w * h * 4));
    stbi_image_free(data);
    return true;
}
