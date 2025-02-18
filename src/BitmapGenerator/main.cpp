#define STB_TRUETYPE_IMPLEMENTATION
#include "stb/stb_truetype.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

import std;

using namespace std::literals;

std::array<std::uint64_t, 256> codepoints{
	128, 0x263a, 0x263b, 0x2665, 0x2666, 0x2663, 0x2660, 0x2022, 0x25d8, 0x25cb, 0x25d9, 0x2642, 0x2640, 0x266a, 0x266b, 0x263c,
	0x25ba, 0x25c4, 0x2195, 0x203c, 0x00b6, 0x00a7, 0x25ac, 0x21a8, 0x2191, 0x2193, 0x2192, 0x2190, 0x221f, 0x2194, 0x25b2, 0x25bc,
	32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
	48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
	64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
	80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
	96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
	112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 0x2302,
	0x00c7, 0x00fc, 0x00e9, 0x00e2, 0x00e4, 0x00e0, 0x00e5, 0x00e7, 0x00ea, 0x00eb, 0x00e8, 0x00ef, 0x00ee, 0x00ec, 0x00c4, 0x00c5,
	0x00c9, 0x00e6, 0x00c6, 0x00f4, 0x00f6, 0x00f2, 0x00fb, 0x00f9, 0x00ff, 0x00d6, 0x00dc, 0x00a2, 0x00a3, 0x00a5, 0x20a7, 0x0192,
	0x00e1, 0x00ed, 0x00f3, 0x00fa, 0x00f1, 0x00d1, 0x00aa, 0x00ba, 0x00bf, 0x2310, 0x00ac, 0x00bd, 0x00bc, 0x00a1, 0x00ab, 0x00bb,
	0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2561, 0x2562, 0x2556, 0x2555, 0x2563, 0x2551, 0x2557, 0x255d, 0x255c, 0x255b, 0x2510,
	0x2514, 0x2534, 0x252c, 0x251c, 0x2500, 0x253c, 0x255e, 0x255f, 0x255a, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256c, 0x2567,
	0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256b, 0x256a, 0x2518, 0x250c, 0x2588, 0x2584, 0x258c, 0x2590, 0x2580,
	0x03b1, 0x00df, 0x0393, 0x03c0, 0x03a3, 0x03c3, 0x00b5, 0x03c4, 0x03a6, 0x0398, 0x03a9, 0x03b4, 0x221e, 0x03c6, 0x03b5, 0x2229,
	0x2261, 0x00b1, 0x2265, 0x2264, 0x2320, 0x2321, 0x00f7, 0x2248, 0x00b0, 0x2219, 0x00b7, 0x221a, 0x207f, 0x00b2, 0x25a0, 0x00a0};

struct CodepointData
{
	CodepointData() = default;
	CodepointData(stbtt_fontinfo const& fontInfo, float scale, int codepoint, int padding)
	{
		auto pixelStep = padding == 0 ? 32.0f : 128.0f / padding;
		sdfData = stbtt_GetCodepointSDF(&fontInfo, scale, codepoint, padding, 128, pixelStep, &width, &height, &xOffset, &yOffset);
	}
	~CodepointData()
	{
		if(sdfData)
			stbtt_FreeSDF(sdfData, nullptr);
	}
	CodepointData(CodepointData&& rhs)
	{
		*this = std::move(rhs);
	}
	CodepointData& operator=(CodepointData&& rhs)
	{
		std::swap(width, rhs.width);
		std::swap(height, rhs.height);
		std::swap(xOffset, rhs.xOffset);
		std::swap(yOffset, rhs.yOffset);
		std::swap(sdfData, rhs.sdfData);
		return *this;
	}

	int width{}, height{}, xOffset{}, yOffset{};
	unsigned char* sdfData{};
};

struct FontData
{
	std::uint32_t size{};
	int ascent{}, descent{}, lineGap{};
	int maxWidth{}, maxHeight{};
	std::uint32_t leftOffset{}, upOffset{};

	std::array<CodepointData, codepoints.size()> codepointData{};
};

//Choose largest font size that fits within boundaries
auto getLargestFontData(stbtt_fontinfo const& fontInfo, std::int32_t tileWidth, std::int32_t tileHeight, int padding)
{
	FontData result{};

	//Iterate over sizes until glyphs exceed tile size
	for(std::uint32_t size = 2; ; size += 2)
	{
		FontData currentFontData{};
		currentFontData.size = size;

		auto currentScale = stbtt_ScaleForPixelHeight(&fontInfo, (float)size);

		stbtt_GetFontVMetrics(&fontInfo, &currentFontData.ascent, &currentFontData.descent, &currentFontData.lineGap);
		currentFontData.ascent = std::ceil(currentFontData.ascent * currentScale);
		currentFontData.descent = std::ceil(currentFontData.descent * currentScale);
		currentFontData.lineGap = std::ceil(currentFontData.lineGap * currentScale);

		bool stopSearch{};
		for(std::size_t i = 0; i < codepoints.size(); i++)
		{
			auto& data = currentFontData.codepointData[i];
			data = CodepointData(fontInfo, currentScale, codepoints[i], padding);

			int horizontalExtent = data.xOffset + padding;
			auto totalWidth = data.width + std::abs(horizontalExtent);
			if(horizontalExtent < 0 && std::abs(horizontalExtent) > currentFontData.leftOffset)
				currentFontData.leftOffset = std::abs(horizontalExtent);

			int verticalExtent = currentFontData.ascent + data.yOffset + padding;
			auto totalHeight = data.height + std::abs(verticalExtent);
			if(verticalExtent < 0 && std::abs(verticalExtent) > currentFontData.upOffset)
				currentFontData.upOffset = std::abs(verticalExtent);

			if(totalWidth > tileWidth - 2 || totalHeight > tileHeight - 2)
			{
				stopSearch = true;
				break;
			}

			if(totalWidth > currentFontData.maxWidth)
				currentFontData.maxWidth = totalWidth;
			if(totalHeight > currentFontData.maxHeight)
				currentFontData.maxHeight = totalHeight;
		}

		if(stopSearch)
		{
			std::println("Selected font size: {} maxWidth: {} maxHeight: {}", currentFontData.size, currentFontData.maxWidth, currentFontData.maxHeight);
			break;
		}

		result = std::move(currentFontData);
	}

	return result;
}

auto main(int argc, char** argv) -> int
{
	if(argc < 2)
	{
		std::println("Usage: BitmapGenerator <file_path> [options]\n"
					 "\toptions:\n"
					 "\t\t--width <value>\tSpecify width of a single tile [2..1024]. Default: 32"
					 "\t\t--height <value>\tSpecify height of a single tile [2..1024]. Default: 64"
					 "\t\t--output <value>\tSpecify output file. Default: tiles.png"
					 "\t\t--padding <value>\tSpecify glyph padding [0..256]. Default: 4"
					 "\t\t--debug\tRender debug boundaries on bitmap.");
		return 1;
	}

	//Parse command line args
	std::uint32_t tileWidth{32u}, tileHeight{64u}, padding{4u};
	std::string outputFile{"tiles.png"};
	bool debug{};
	for(std::uint32_t i = 2; i < argc; i++)
	{
		if(argv[i] == "--width"sv)
		{
			tileWidth = std::atoi(argv[i + 1]);
			if(tileWidth <= 2 || tileWidth > 1024)
			{
				std::println("Invalid width argument");
				return 1;
			}
			i++;
		}
		else if(argv[i] == "--height"sv)
		{
			tileHeight = std::atoi(argv[i + 1]);
			if(tileHeight <= 2 || tileHeight > 1024)
			{
				std::println("Invalid height argument");
				return 1;
			}
			i++;
		}
		else if(argv[i] == "--output"sv)
		{
			outputFile = argv[i + 1];
			i++;
		}
		else if(argv[i] == "--padding"sv)
		{
			padding = std::atoi(argv[i + 1]);
			if(padding > 256)
			{
				std::println("Invalid padding argument");
				return 1;
			}
			i++;
		}
		else if(argv[i] == "--debug"sv)
		{
			debug = true;
		}
	}
	std::string fileName = argv[1];

	//Open and read font file
	auto file = std::fopen(fileName.data(), "rb");
	std::fseek(file, 0, SEEK_END);
	auto fileSize = std::ftell(file);
	std::rewind(file);
	std::vector<unsigned char> data(fileSize, 0);
	auto result = std::fread(data.data(), sizeof(unsigned char), fileSize, file);

	//Init stb font
	stbtt_fontinfo fontInfo;
	stbtt_InitFont(&fontInfo, data.data(), stbtt_GetFontOffsetForIndex(data.data(), 0));

	//Get sdf and size data
	auto fontData = getLargestFontData(fontInfo, tileWidth, tileHeight, padding);
	if(fontData.size == 0)
	{
		std::println("Couldn't find suitable font size");
		return 1;
	}
	std::println("Font ascent: {} descent: {} lineGap: {}", fontData.ascent, fontData.descent, fontData.lineGap);

	//Final tile texture is 16x16
	std::size_t textureWidth{(size_t)tileWidth * 16}, textureHeight{(size_t)tileHeight * 16};
	std::vector<unsigned char> finalBitmap;
	finalBitmap.resize(textureWidth * textureHeight, debug ? 255 : 0);

	//Draw tile edges in debug mode
	if(debug)
	{
		for(std::size_t i = 0; i < 16; i++)
		{
			for(std::size_t j = 0; j < textureHeight; j++)
			{
				auto tileStride = i * tileWidth;
				auto textureStride = j * textureWidth;
				finalBitmap[(std::size_t)tileWidth - 1 + tileStride + textureStride] = 96;
				finalBitmap[tileStride + textureStride] = 160;
			}
		}
		for(std::size_t i = 0; i < 16; i++)
		{
			for(std::size_t j = 0; j < textureWidth; j++)
			{
				auto tileStride = i * tileHeight;
				finalBitmap[j + ((std::size_t)tileHeight - 1 + tileStride) * textureWidth] = 96;
				finalBitmap[j + tileStride * textureWidth] = 160;
			}
		}
	}

	//Fill final texture
	int leftMargin = (tileWidth - fontData.maxWidth) / 2;
	int upMargin = (tileHeight - fontData.maxHeight) / 2;
	std::uint64_t glyphRow{}, glyphColumn{};
	for(std::uint64_t i = 0; i < codepoints.size(); i++)
	{
		auto& data = fontData.codepointData[i];
		std::println("{:X} padding: {} width: {} height: {} xOffset: {} yOffset: {}",
					 codepoints[i], padding, data.width, data.height, data.xOffset, data.yOffset);

		for(std::uint64_t i = 0; i < data.width; i++)
		{
			for(std::uint64_t j = 0; j < data.height; j++)
			{
				auto horizontalTileStride = glyphColumn * tileWidth;
				auto verticalTileStride = glyphRow * tileHeight;
				auto leftOffset = leftMargin + data.xOffset + padding + fontData.leftOffset;
				auto upOffset = upMargin + fontData.ascent + data.yOffset + padding + fontData.upOffset;
				finalBitmap[horizontalTileStride + leftOffset + i + (verticalTileStride + upOffset + j) * textureWidth] = data.sdfData[i + j * data.width];
			}
		}

		glyphColumn++;
		if(glyphColumn == 16)
		{
			glyphColumn = 0;
			glyphRow++;
		}
	}

	stbi_write_png(outputFile.c_str(), textureWidth, textureHeight, 1, finalBitmap.data(), 0);

	return 0;
}