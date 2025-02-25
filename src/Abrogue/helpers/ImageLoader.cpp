module;

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

module ImageLoader;

ImageLoader::ImageLoader(std::string_view filePath)
{
	data = stbi_load(filePath.data(), &width, &height, &channels, STBI_grey);
}

ImageLoader::~ImageLoader()
{
	if(data)
		stbi_image_free(data);
}

ImageLoader::ImageLoader(ImageLoader&& rhs)
{
	*this = std::move(rhs);
}

ImageLoader& ImageLoader::operator=(ImageLoader&& rhs)
{
	std::swap(width, rhs.width);
	std::swap(height, rhs.height);
	std::swap(channels, rhs.channels);
	std::swap(data, rhs.data);
	return *this;
}
