#define STB_TRUETYPE_IMPLEMENTATION
#include "stb/stb_truetype.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

import std;

auto main(int argc, char** argv) -> int
{
	std::string fileName = "C:/Users/denis/source/repos/DejaVuSansMono.ttf";

	auto file = std::fopen(fileName.data(), "rb");
	std::fseek(file, 0, SEEK_END);
	auto fileSize = std::ftell(file);
	std::rewind(file);
	std::vector<unsigned char> data(fileSize, 0);
	auto result = std::fread(data.data(), sizeof(unsigned char), fileSize, file);

	stbtt_fontinfo fontInfo;
	stbtt_InitFont(&fontInfo, data.data(), stbtt_GetFontOffsetForIndex(data.data(), 0));

	auto scale = stbtt_ScaleForPixelHeight(&fontInfo, 32.0f);

	int ascent{}, descent{}, lineGap{};
	stbtt_GetFontVMetrics(&fontInfo, &ascent, &descent, &lineGap);
	ascent = int(ascent * scale);
	descent = int(descent * scale);
	lineGap = int(lineGap * scale);
	std::println("Font ascent: {} descent: {} lineGap: {}", ascent, descent, lineGap);

	std::vector<unsigned char> finalBitmap;
	finalBitmap.resize((std::size_t)512 * 1024, 255);

	std::uint64_t glyphRow{}, glyphColumn{};
	for(std::uint64_t glyphIndex = 32; glyphIndex < 128; glyphIndex++)
	{
		int width{}, height{}, xOffset{}, yOffset{};
		auto sdfData = stbtt_GetCodepointSDF(&fontInfo, scale, glyphIndex, 0, 128, 20.0f, &width, &height, &xOffset, &yOffset);
		stbtt_FreeSDF(sdfData, nullptr);
		auto padding = 4;
		sdfData = stbtt_GetCodepointSDF(&fontInfo, scale, glyphIndex, padding, 128, 20.0f, &width, &height, &xOffset, &yOffset);

		int leftExtent{}, upExtent{}, rightExtent{}, downExtent{};
		stbtt_GetCodepointBitmapBox(&fontInfo, glyphIndex, scale, scale, &leftExtent, &upExtent, &rightExtent, &downExtent);

		std::println("{} padding: {} width: {} height: {} xOffset: {} yOffset: {} leftExtent: {} upExtent: {} rightExtent: {} downExtent: {}",
					 (char)glyphIndex, padding, width, height, xOffset, yOffset, leftExtent, upExtent, rightExtent, downExtent);

		for(std::uint64_t i = 0; i < width; i++)
		{
			for(std::uint64_t j = 0; j < height; j++)
			{
				finalBitmap[glyphColumn * 32 + leftExtent + i + (ascent + upExtent + j + glyphRow * 64) * 512] = sdfData[i + j * width];
			}
		}
		stbtt_FreeSDF(sdfData, nullptr);

		glyphColumn++;
		if(glyphColumn == 16)
		{
			glyphColumn = 0;
			glyphRow++;
		}
	}

	stbi_write_png("lmao.png", 512, 1024, 1, finalBitmap.data(), 0);

	return 0;
}