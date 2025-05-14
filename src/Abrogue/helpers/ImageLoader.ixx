export module ImageLoader;

export import std;

//Class for loading image data using stb
export class ImageLoader
{
public:
	ImageLoader() = default;
	ImageLoader(std::string_view filePath);
	~ImageLoader();

	ImageLoader(ImageLoader&& rhs);
	ImageLoader& operator=(ImageLoader&& rhs);

	std::int32_t width{}, height{}, channels{};
	std::uint8_t* data{};
};

