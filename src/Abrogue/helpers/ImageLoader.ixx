export module ImageLoader;

export import std;

export class ImageLoader
{
public:
	ImageLoader() = default;
	ImageLoader(std::string_view filePath);
	~ImageLoader();
	ImageLoader(ImageLoader&& rhs);
	ImageLoader& operator=(ImageLoader&& rhs);

	int width{}, height{}, channels{};
	std::uint8_t* data{};
};

