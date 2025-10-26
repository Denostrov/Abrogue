module;

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

export module ImageLoader;

import std;

//Class for loading image data using stb
export class ImageLoader
{
public:
    ImageLoader() = default;
    ImageLoader(std::string_view filePath)
    {
        data = stbi_load(filePath.data(), &width, &height, &channels, STBI_grey);
        if (!data)
        {
            data = static_cast<std::uint8_t*>(malloc(32 * 32));
            for (std::size_t i{}; i < 32 * 32; i++)
            {
                data[i] = 255 * (i % 2);
            }
            width = 32;
            height = 32;
            channels = 1;
        }
    }
    ~ImageLoader()
    {
        if(data)
            stbi_image_free(data);
    }

    ImageLoader(ImageLoader&& rhs) noexcept
    {
        *this = std::move(rhs);
    }
    ImageLoader& operator=(ImageLoader&& rhs) noexcept
    {
        width = rhs.width;
        height = rhs.height;
        channels = rhs.channels;
        std::swap(data, rhs.data);
        return *this;
    }

    std::int32_t width{}, height{}, channels{};
    std::uint8_t* data{};
};

