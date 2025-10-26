#define STB_TRUETYPE_IMPLEMENTATION
#include "stb/stb_truetype.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

import Helpers;

using namespace std::literals;

//Mapping of Unicode codepoints to extended ascii
std::array codepoints{
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
    0x2261, 0x00b1, 0x2265, 0x2264, 0x2320, 0x2321, 0x00f7, 0x2248, 0x00b0, 0x2219, 0x00b7, 0x221a, 0x207f, 0x00b2, 0x25a0, 0x00a0
};

//Functor for freeing stb bitmap
struct STBTTDeleter
{
    void operator()(std::uint8_t* data) const { stbtt_FreeSDF(data, nullptr); }
};

//Struct for data related to a single codepoint
struct CodepointData
{
    CodepointData() = default;

    //Get the bitmap data for a codepoint
    template <bool noSDF>
    CodepointData(stbtt_fontinfo const& fontInfo, float scale, std::int32_t codepoint, std::int32_t padding, std::int32_t edge, BoolSequence<noSDF>)
        : data(stbtt_GetCodepointBitmap(&fontInfo, scale, scale, codepoint, &width, &height, &xOffset, &yOffset))
    {
        if constexpr (noSDF)
        {
            data = std::unique_ptr<std::uint8_t, STBTTDeleter>(
                stbtt_GetCodepointBitmap(&fontInfo, scale, scale, codepoint, &width, &height, &xOffset, &yOffset));
        }
        else
        {
            data = std::unique_ptr<std::uint8_t, STBTTDeleter>(stbtt_GetCodepointSDF(&fontInfo, scale, codepoint, padding, edge,
                                                                                     padding == 0
                                                                                         ? 128.0f
                                                                                         : static_cast<float>(edge) / static_cast<float>(padding),
                                                                                     &width, &height, &xOffset, &yOffset));
        }
    }

    CodepointData(CodepointData&& rhs) = default;

    CodepointData& operator=(CodepointData&& rhs) = default;

    //Fill the corresponding tile in the texture with codepoint data
    template <bool debug>
    void fillTexture(std::vector<std::uint8_t>& texture, std::int32_t originX, std::int32_t originY, std::int32_t textureWidth, BoolSequence<debug>) const
    {
        auto topLeftX = originX + xOffset;
        auto topLeftY = originY + yOffset;
        for (std::int64_t i = 0; i < width; i++)
        {
            for (std::int64_t j = 0; j < height; j++)
            {
                texture[topLeftX + i + (topLeftY + j) * textureWidth] = data.get()[i + j * width];
            }
        }

        if constexpr (debug)
        {
            texture[originX + originY * textureWidth] = 128;
            texture[topLeftX + topLeftY * textureWidth] = 255;
        }
    }

    std::int32_t width{}, height{}; ///< Width and height of the glyph bounding box
    std::int32_t xOffset{}, yOffset{}; ///< Offset from the glyph origin to the top left corner of the bounding box
    std::unique_ptr<std::uint8_t, STBTTDeleter> data; ///< Raw pixel data array [width x height]
};

struct FontData
{
    FontData() = default;
    //Choose the largest font size that fits within boundaries
    template <bool noSDF>
    FontData(stbtt_fontinfo const& fontInfo, std::int32_t tileWidth, std::int32_t tileHeight, std::int32_t padding, std::int32_t edge, BoolSequence<noSDF>)
    {
        //Iterate over sizes until glyphs exceed tile size
        for (std::int32_t currentSize{2}; ; currentSize += 2)
        {
            FontData currentFontData;
            currentFontData.size = currentSize;
            auto currentScale = stbtt_ScaleForPixelHeight(&fontInfo, static_cast<float>(currentSize));

            stbtt_GetFontVMetrics(&fontInfo, &currentFontData.ascent, &currentFontData.descent, &currentFontData.lineGap);
            currentFontData.ascent = std::ceil(static_cast<float>(currentFontData.ascent) * currentScale);
            currentFontData.descent = std::ceil(static_cast<float>(currentFontData.descent) * currentScale);
            currentFontData.lineGap = std::ceil(static_cast<float>(currentFontData.lineGap) * currentScale);

            for (std::size_t i{}; i < codepoints.size(); i++)
            {
                auto& data = currentFontData.codepointData[i];
                data = CodepointData(fontInfo, currentScale, codepoints[i], padding, edge, BoolSequence<noSDF>{});

                currentFontData.maxLeftExtent = std::min(currentFontData.maxLeftExtent, data.xOffset);
                currentFontData.maxRightExtent = std::max(currentFontData.maxRightExtent, data.xOffset + data.width);
                currentFontData.maxUpExtent = std::min(currentFontData.maxUpExtent, data.yOffset);
                currentFontData.maxDownExtent = std::max(currentFontData.maxDownExtent, data.yOffset + data.height);

                if (currentFontData.maxRightExtent - currentFontData.maxLeftExtent > tileWidth - 2 ||
                    currentFontData.maxDownExtent - currentFontData.maxUpExtent > tileHeight - 2)
                    goto endSearch;
            }

            *this = std::move(currentFontData);
        }

    endSearch:
        std::println("Selected font size: {}\nascent: {} descent: {} lineGap: {}\nmaxLeftExtent: {} maxUpExtent: {} maxRightExtent: {} maxDownExtent: {}",
                     size, ascent, descent, lineGap, maxLeftExtent, maxUpExtent, maxRightExtent, maxDownExtent);
    }

    template <bool debug>
    void fillTexture(std::string_view outputFile, std::int32_t tileWidth, std::int32_t tileHeight, BoolSequence<debug>)
    {
        if (size == 0)
        {
            std::println("Couldn't find suitable font size");
            return;
        }

        //Final tile texture is 16x16
        std::int32_t textureWidth{tileWidth * 16}, textureHeight{tileHeight * 16};
        std::vector<std::uint8_t> finalBitmap(textureWidth * textureHeight);

        //Draw tile edges in debug mode
        if constexpr (debug)
        {
            std::ranges::fill(finalBitmap, 255);
            for (std::int32_t i = 0; i < 16; i++)
            {
                for (std::int32_t j = 0; j < textureHeight; j++)
                {
                    auto tileStride = i * tileWidth;
                    auto textureStride = j * textureWidth;
                    finalBitmap[tileWidth - 1 + tileStride + textureStride] = 96;
                    finalBitmap[tileStride + textureStride] = 160;
                }
            }
            for (std::int32_t i = 0; i < 16; i++)
            {
                for (std::int32_t j = 0; j < textureWidth; j++)
                {
                    auto tileStride = i * tileHeight;
                    finalBitmap[j + (tileHeight - 1 + tileStride) * textureWidth] = 96;
                    finalBitmap[j + tileStride * textureWidth] = 160;
                }
            }
        }

        //Fill final texture
        auto originX = (tileWidth - (maxRightExtent - maxLeftExtent)) / 2 - maxLeftExtent;
        auto originY = (tileHeight - (maxDownExtent - maxUpExtent)) / 2 - maxUpExtent;
        for (std::int32_t row = 0; row < 16; row++)
        {
            for (std::int32_t column = 0; column < 16; column++)
            {
                codepointData[column + row * 16].fillTexture(finalBitmap, originX + column * tileWidth, originY + row * tileHeight,
                                                             textureWidth, BoolSequence<debug>{});
            }
        }

        stbi_write_png(outputFile.data(), textureWidth, textureHeight, 1, finalBitmap.data(), 0);
    }

    std::int32_t size{}; ///< Font size in pixels
    std::int32_t ascent{}; ///< Distance above baseline
    std::int32_t descent{}; ///< Distance below baseline
    std::int32_t lineGap{}; ///< Distance between text lines
    std::int32_t maxLeftExtent{}, maxUpExtent{}, maxRightExtent{}, maxDownExtent{}; ///< Max distances that characters can extend beyond the glyph origin

    std::array<CodepointData, codepoints.size()> codepointData{};
};

auto main(int const argc, char** argv) -> int
{
    //Print usage help
    if (argc < 2)
    {
        std::println("Usage: BitmapGenerator <file_path> [options]\n"
            "\toptions:\n"
            "\t\t--width <value>\tSpecify width of a single tile [2..1024]. Default: 32\n"
            "\t\t--height <value>\tSpecify height of a single tile [2..1024]. Default: 64\n"
            "\t\t--output <value>\tSpecify output file. Default: tiles.png\n"
            "\t\t--padding <value>\tSpecify glyph padding [0..256]. Default: 4\n"
            "\t\t--edge <value>\tSpecify sdf edge value [0..255]. Default: 51\n"
            "\t\t--debug\tRender debug boundaries on bitmap.\n"
            "\t\t--no-sdf\tRender regular bitmap");
        return 1;
    }

    //Parse command line args
    std::int32_t tileWidth{32}, tileHeight{64}, padding{4}, edge{51};
    std::string outputFile{"tiles.png"};
    bool debug{}, noSDF{};
    for (std::uint32_t i{2u}; i < argc; i++)
    {
        if (argv[i] == "--width"sv)
        {
            std::string_view valueStr{argv[i + 1]};
            if (!std::from_chars(valueStr.data(), valueStr.data() + valueStr.size(), tileWidth))
            {
                std::println("Invalid width argument: {}", valueStr);
                return 1;
            }
            if (tileWidth <= 2 || tileWidth > 1024)
            {
                std::println("Width out of range [2..1024]: {}", valueStr);
                return 1;
            }
            i++;
        }
        else if (argv[i] == "--height"sv)
        {
            std::string_view valueStr{argv[i + 1]};
            if (!std::from_chars(valueStr.data(), valueStr.data() + valueStr.size(), tileHeight))
            {
                std::println("Invalid height argument: {}", valueStr);
                return 1;
            }
            if (tileHeight <= 2 || tileHeight > 1024)
            {
                std::println("Height out of range [2..1024]: {}", valueStr);
                return 1;
            }
            i++;
        }
        else if (argv[i] == "--output"sv)
        {
            outputFile = argv[i + 1];
            i++;
        }
        else if (argv[i] == "--padding"sv)
        {
            std::string_view valueStr{argv[i + 1]};
            if (!std::from_chars(valueStr.data(), valueStr.data() + valueStr.size(), padding))
            {
                std::println("Invalid padding argument: {}", valueStr);
                return 1;
            }
            if (padding < 0 || padding > 256)
            {
                std::println("Padding out of range [0..256]: {}", valueStr);
                return 1;
            }
            i++;
        }
        else if (argv[i] == "--edge"sv)
        {
            std::string_view valueStr{argv[i + 1]};
            if (!std::from_chars(valueStr.data(), valueStr.data() + valueStr.size(), edge))
            {
                std::println("Invalid edge argument: {}", valueStr);
                return 1;
            }
            if (edge < 0 || edge > 255)
            {
                std::println("Edge out of range [0..255]: {}", valueStr);
                return 1;
            }
            i++;
        }
        else if (argv[i] == "--debug"sv)
        {
            debug = true;
        }
        else if (argv[i] == "--no-sdf"sv)
        {
            noSDF = true;
        }
        else
        {
            std::println("Unknown argument: {}", argv[i]);
        }
    }

    //Open and read font file
    auto file = std::fopen(argv[1], "rb");
    if (!file)
    {
        std::println("Failed to open file {}", argv[1]);
        return 1;
    }
    std::fseek(file, 0, SEEK_END);
    auto fileSize = std::ftell(file);
    std::rewind(file);
    std::vector<std::uint8_t> data(fileSize, 0);
    if (auto result = std::fread(data.data(), sizeof(std::uint8_t), fileSize, file); result < fileSize)
    {
        std::println("Failed to read file {}", argv[1]);
        return 1;
    }

    //Init stb font
    stbtt_fontinfo fontInfo;
    stbtt_InitFont(&fontInfo, data.data(), stbtt_GetFontOffsetForIndex(data.data(), 0));

    //Find the largest font that fits in tilemap and output to file
    auto generateTexture = [&fontInfo, outputFile, tileWidth, tileHeight, padding, edge]<bool noSDF, bool debug>(BoolSequence<noSDF, debug>) mutable
    {
        //Bitmaps don't need padding
        if constexpr (noSDF)
            padding = 0;

        auto fontData = FontData(fontInfo, tileWidth, tileHeight, padding, edge, BoolSequence<noSDF>{});
        fontData.fillTexture(outputFile, tileWidth, tileHeight, BoolSequence<debug>{});
    };
    BoolFlag::call(generateTexture, std::array{noSDF, debug});

    return 0;
}
