module;

#include <dlfcn.h>
#include <fcntl.h>
#include <unistd.h>

export module Helpers;

export import SDL3;
export import std;
export import :Flags;
export import :Concepts;
export import :Logger;
export import :Array;
export import :Color;
export import :FixedString;
export import :FixedVector;
export import :Random;
export import :Tags;
export import :Literals;

// Helper template aliases
export template <class T>
using optRef = std::optional<std::reference_wrapper<T>>;
export template <class T>
using optCRef = std::optional<std::reference_wrapper<T const>>;

template <class Enum, template <Enum> class T>
class EnumTupleTrait
{
    template <std::size_t... I>
    static consteval auto getType(std::index_sequence<I...>)
    {
        return std::tuple<T<static_cast<Enum>(I)>...>{};
    }

public:
    using type = decltype(getType(std::make_index_sequence<static_cast<std::size_t>(Enum::COUNT)>{}));
};
export template <class Enum, template <Enum> class T>
using EnumTupleType = EnumTupleTrait<Enum, T>::type;

export template <class... Args>
class GlobalHandler
{
public:
    GlobalHandler(Args&... args) : globals(args...) {}

    void cleanup() { cleanupIndex<std::tuple_size_v<decltype(globals)> - 1>(); }

private:
    template <std::size_t Index>
    void cleanupIndex()
    {
        destructGlobal(std::get<Index>(globals));
        if constexpr (Index > 0)
            cleanupIndex<Index - 1>();
    }

    template <class T>
    static void destructGlobal(T& obj)
    {
        obj.~T();
        new (&obj) T();
    }

    std::tuple<Args&...> globals;
};

export class LibraryLoader
{
public:
    LibraryLoader() = default;

    [[nodiscard]] bool init(char const* libraryName)
    {
        library = dlopen(libraryName, RTLD_LOCAL | RTLD_NOW);
        return library;
    }

    void cleanup() { dlclose(library); }

    template <class Func>
    [[nodiscard]] Func getFunction(char const* functionName)
    {
        return (Func)dlsym(library, functionName);
    }

private:
    void* library{};
};

export class DatReader
{
public:
    DatReader() = default;

    [[nodiscard]] bool init(char const* filePath)
    {
        descriptor = open(filePath, O_RDONLY);
        if (descriptor == -1)
        {
            cleanup();
            return false;
        }

        if (!readBytes())
        {
            cleanup();
            return false;
        }

        isValid = parseRow();
        return true;
    }

    void cleanup()
    {
        if (descriptor != -1)
            close(descriptor);
    }

    template <class Value>
    bool readValue(Value& value)
    {
        if (!isValid)
            return false;

        if constexpr (IsLikeStringView<Value>)
        {
            if (valueStr.getSize() > value.getCapacity())
                return false;

            value = valueStr;
        }
        else if constexpr (std::is_same_v<Value, Color>)
        {
            std::int64_t startIndex{};
            std::int64_t endIndex{};
            Array<std::uint8_t, 4> valArr;
            for (std::int64_t i = 0; i < valArr.getSize();)
            {
                if (endIndex == valueStr.getSize())
                {
                    if (i != valArr.getSize() - 1)
                        return false;

                    if (!std::from_chars(valueStr.getData() + startIndex, valueStr.getData() + endIndex, valArr[i]))
                        return false;
                    i++;
                }
                else if (valueStr[endIndex] == ',')
                {
                    if (!std::from_chars(valueStr.getData() + startIndex, valueStr.getData() + endIndex, valArr[i]))
                        return false;

                    startIndex = endIndex + 1;
                    i++;
                }

                endIndex++;
            }

            value.r = valArr[0];
            value.g = valArr[1];
            value.b = valArr[2];
            value.a = valArr[3];
        }
        else if constexpr (std::is_same_v<Value, bool>)
        {
            if (valueStr == "true"sv)
                value = true;
            else
            {
                if (valueStr != "false"sv)
                    return false;

                value = false;
            }
        }
        else if constexpr (std::is_same_v<Value, char>)
        {
            if (valueStr.getSize() != 1)
                return false;

            value = valueStr[0];
        }
        else if constexpr (std::is_integral_v<Value> || std::is_floating_point_v<Value>)
        {
            if (!std::from_chars(valueStr.getData(), valueStr.getData() + valueStr.getSize(), value))
                return false;
        }
        else if constexpr (std::is_enum_v<Value>)
        {
            std::uint64_t enumVal{};
            if (!std::from_chars(valueStr.getData(), valueStr.getData() + valueStr.getSize(), enumVal))
                return false;

            value = static_cast<Value>(enumVal);
        }
        else
        {
            static_assert(false, "Unsupported data type requested");
        }

        keyStr.clear();
        valueStr.clear();
        isValid = parseRow();
        return true;
    }

    [[nodiscard]] auto getIsValid() const { return isValid; }
    [[nodiscard]] auto const& getKey() const { return keyStr; }

private:
    [[nodiscard]] bool readBytes()
    {
        bytesRead = read(descriptor, buffer.getData(), buffer.getSize());
        bufferIndex = 0;
        return bytesRead > 0;
    }
    [[nodiscard]] bool parseRow()
    {
        while (true)
        {
            if (bufferIndex == bytesRead && !readBytes())
                return false;

            if (buffer[bufferIndex] != '\r' && buffer[bufferIndex] != '\n' && buffer[bufferIndex] != '\t' && buffer[bufferIndex] != ' ')
                break;

            bufferIndex++;
        }

        while (true)
        {
            if (bufferIndex == bytesRead && !readBytes())
                return false;

            if (buffer[bufferIndex] == '\r' || buffer[bufferIndex] == '\n')
                return false;

            if (buffer[bufferIndex] == '=')
            {
                bufferIndex++;
                break;
            }

            if (keyStr.getIsFull())
                return false;

            keyStr.append(buffer[bufferIndex]);
            bufferIndex++;
        }

        while (true)
        {
            if (bufferIndex >= bytesRead && !readBytes())
                break;

            if (buffer[bufferIndex] == '\r' || buffer[bufferIndex] == '\n')
            {
                bufferIndex++;
                break;
            }

            if (valueStr.getIsFull())
                return false;

            valueStr.append(buffer[bufferIndex]);
            bufferIndex++;
        }

        return true;
    }

    int descriptor{-1};
    Array<char, 4096> buffer;
    std::int64_t bytesRead{};
    std::int64_t bufferIndex{};

    bool isValid{};
    FixedString<256> keyStr;
    FixedString<256> valueStr;
};

export class DatWriter
{
public:
    DatWriter() = default;

    [[nodiscard]] bool init(char const* filePath)
    {
        descriptor = open(filePath, O_CREAT | O_TRUNC | O_WRONLY, 0644);
        if (descriptor == -1)
        {
            cleanup();
            return false;
        }

        return true;
    }

    void cleanup()
    {
        if (descriptor != -1)
            close(descriptor);
    }

    template <class Value>
    void writeValue(std::string_view key, Value& value)
    {
        FixedString<1024> rowStr;
        rowStr.append(key, '=');
        if constexpr (IsLikeStringView<Value>)
        {
            rowStr.append(value);
        }
        else if constexpr (std::is_same_v<Value, Color>)
        {
            Array<char, 4> valStr;
            auto result = std::to_chars(valStr.begin(), valStr.end(), value.r);
            rowStr.append(std::string_view{valStr.getData(), result.ptr}, ',');
            result = std::to_chars(valStr.begin(), valStr.end(), value.g);
            rowStr.append(std::string_view{valStr.getData(), result.ptr}, ',');
            result = std::to_chars(valStr.begin(), valStr.end(), value.b);
            rowStr.append(std::string_view{valStr.getData(), result.ptr}, ',');
            result = std::to_chars(valStr.begin(), valStr.end(), value.a);
            rowStr.append(std::string_view{valStr.getData(), result.ptr});
        }
        else if constexpr (std::is_same_v<Value, bool>)
        {
            rowStr.append(value ? "true"sv : "false"sv);
        }
        else if constexpr (std::is_integral_v<Value> || std::is_floating_point_v<Value>)
        {
            Array<char, 32> valStr;
            auto result = std::to_chars(valStr.begin(), valStr.end(), value);
            rowStr.append(std::string_view{valStr.begin(), result.ptr});
        }
        else if constexpr (std::is_enum_v<Value>)
        {
            Array<char, 32> valStr;
            auto result = std::to_chars(valStr.begin(), valStr.end(), (std::uint64_t)value);
            rowStr.append(std::string_view{valStr.begin(), result.ptr});
        }
        else
        {
            static_assert(false, "Unsupported data type requested");
        }

        rowStr.append('\n');
        write(descriptor, rowStr.getData(), rowStr.getSize());
    }

private:
    int descriptor{-1};
};

struct Huffman
{
    static constexpr std::uint16_t reverseBits(std::uint16_t v)
    {
        v = ((v & 0xAAAA) >> 1) | ((v & 0x5555) << 1);
        v = ((v & 0xCCCC) >> 2) | ((v & 0x3333) << 2);
        v = ((v & 0xF0F0) >> 4) | ((v & 0x0F0F) << 4);
        v = ((v & 0xFF00) >> 8) | ((v & 0x00FF) << 8);
        return v;
    }
    constexpr std::uint16_t decode(std::uint32_t& bitBuffer, std::int64_t& bufferSize) const
    {
        std::uint16_t b = fast[bitBuffer & 0x1FF];
        std::uint16_t s = b >> 9;
        bitBuffer >>= s;
        bufferSize -= s;
        return b & 0x1FF;
    }
    template<auto sizeCount>
    constexpr Huffman(Array<std::uint8_t, sizeCount> const& sizeList)
    {
        Array<std::int64_t, 17> nextSizes;
        for (std::int64_t i = 0; i < sizeCount; i++)
            ++nextSizes[sizeList[i]];
        nextSizes[0] = 0;

        Array<std::int64_t, 16> nextCode{};
        std::int64_t code{};
        std::int64_t symbol{};
        for (std::int64_t i = 1; i < 16; i++)
        {
            nextCode[i] = code;
            firstCodes[i] = code;
            firstSymbols[i] = symbol;
            code += nextSizes[i];
            maxCodes[i] = code << (16 - i);
            code <<= 1;
            symbol += nextSizes[i];
        }
        maxCodes[16] = 0x10000;
        for (std::int64_t i = 0; i < sizeCount; i++)
        {
            auto currentSize = sizeList[i];
            if (!currentSize)
                continue;

            auto c = nextCode[currentSize] - firstCodes[currentSize] + firstSymbols[currentSize];
            auto fastv = (std::uint16_t)((currentSize << 9) | i);
            sizes[c] = currentSize;
            values[c] = i;
            auto j = reverseBits(nextCode[currentSize]) >> (16 - currentSize);
            while (j < 512)
            {
                fast[j] = fastv;
                j += 1 << currentSize;
            }
            ++nextCode[currentSize];
        }
    }

    Array<std::uint16_t, 512> fast;
    Array<std::uint16_t, 16> firstCodes;
    Array<std::uint16_t, 16> firstSymbols;
    Array<std::int64_t, 17> maxCodes;
    Array<std::uint8_t, 288> sizes;
    Array<std::uint16_t, 288> values;
};
export class PngLoader
{
    static constexpr Array<std::int64_t, 31> baseLength{3,  4,  5,  6,  7,  8,  9,  10,  11,  13,  15,  17,  19,  23, 27, 31,
                                                        35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0,  0};
    static constexpr Array<std::int64_t, 31> extraLength{0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 0, 0};
    static constexpr Array<std::int64_t, 32> baseDistance{1,   2,   3,   4,   5,    7,    9,    13,   17,   25,   33,   49,    65,    97,    129, 193,
                                                          257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577, 0,   0};
    static constexpr Array<std::int64_t, 32> extraDistance{0, 0, 0, 0, 1, 1, 2,  2,  3,  3,  4,  4,  5,  5,  6, 6,
                                                           7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 0, 0};
    static constexpr Array<std::uint8_t, 288> defaultLength{
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
        9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
        9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8};
    static constexpr Array<std::uint8_t, 32> defaultDistance{5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5};
    static constexpr Huffman lengthHuffman{defaultLength};
    static constexpr Huffman distanceHuffman{defaultDistance};

public:
    struct Data
    {
        std::int64_t width{}, height{}, channelCount{};
        std::span<std::byte> data;
    };

    static bool load(char const* filePath, Data& data)
    {
        auto descriptor = open(filePath, O_RDONLY);
        if (descriptor == -1)
            return false;

        Array<std::byte, 8> header{};
        if (read(descriptor, header.getData(), header.getSize()) != header.getSize())
            return false;

        if (header != Array<std::byte, 8>{137, 80, 78, 71, 13, 10, 26, 10})
            return false;

        std::uint32_t length{};
        if (read(descriptor, &length, sizeof(length)) != sizeof(length))
            return false;
        length = std::byteswap(length);

        Array<std::byte, 4> type{};
        if (read(descriptor, type.getData(), type.getSize()) != type.getSize())
            return false;

        if (type != Array<std::byte, 4>{'I', 'H', 'D', 'R'})
            return false;

        if (length != 13)
            return false;

        std::uint32_t width{};
        if (read(descriptor, &width, sizeof(width)) != sizeof(width))
            return false;
        data.width = std::byteswap(width);

        if (data.width == 0)
            return false;

        std::uint32_t height{};
        if (read(descriptor, &height, sizeof(height)) != sizeof(height))
            return false;
        data.height = std::byteswap(height);

        if (data.height == 0)
            return false;

        if (data.data.size() < data.width * data.height)
            return false;

        std::uint8_t bitDepth{};
        if (read(descriptor, &bitDepth, sizeof(bitDepth)) != sizeof(bitDepth))
            return false;

        if (bitDepth != 8)
            return false;

        std::uint8_t color{};
        if (read(descriptor, &color, sizeof(color)) != sizeof(color))
            return false;

        if (color != 0)
            return false;
        data.channelCount = 1;

        std::uint8_t comp{};
        if (read(descriptor, &comp, sizeof(comp)) != sizeof(comp))
            return false;

        if (comp != 0)
            return false;

        std::uint8_t filter{};
        if (read(descriptor, &filter, sizeof(filter)) != sizeof(filter))
            return false;

        if (filter != 0)
            return false;

        std::uint8_t interlace{};
        if (read(descriptor, &interlace, sizeof(interlace)) != sizeof(interlace))
            return false;

        if (interlace != 0)
            return false;

        std::uint32_t crc{};
        if (read(descriptor, &crc, sizeof(crc)) != sizeof(crc))
            return false;

        if (read(descriptor, &length, sizeof(length)) != sizeof(length))
            return false;
        length = std::byteswap(length);

        if (read(descriptor, type.getData(), type.getSize()) != type.getSize())
            return false;

        if (type != Array<std::byte, 4>{'I', 'D', 'A', 'T'})
            return false;

        Array<std::byte, 131072> compressedData;
        if (length > compressedData.getSize())
            return false;

        if (read(descriptor, compressedData.getData(), length) != length)
            return false;

        if (!decompress(descriptor, std::span(compressedData.getData(), length), data.data))
            return false;

        std::int64_t decompressedIndex{};
        for (std::int64_t i = 0; i < data.height; i++)
        {
            std::uint8_t lineFilter = (std::uint8_t)data.data[decompressedIndex];
            decompressedIndex++;

            if (lineFilter == 0)
            {
                for (std::int64_t j = 0; j < data.width; j++)
                {
                    data.data[decompressedIndex - (i + 1)] = data.data[decompressedIndex];
                    decompressedIndex++;
                }
            }
            else if (lineFilter == 1)
            {
                data.data[decompressedIndex - (i + 1)] = data.data[decompressedIndex];
                decompressedIndex++;
                for (std::int64_t j = 1; j < data.width; j++)
                {
                    data.data[decompressedIndex - (i + 1)] = static_cast<std::byte>((std::uint8_t)data.data[decompressedIndex - (i + 1) - 1] + (std::uint8_t)data.data[decompressedIndex]);
                    decompressedIndex++;
                }
            }
            else if (lineFilter == 2)
            {
                for (std::int64_t j = 0; j < data.width; j++)
                {
                    auto top{i == 0 ? (std::uint8_t)0 : (std::uint8_t)data.data[decompressedIndex - (i + 1) - data.width]};
                    data.data[decompressedIndex - (i + 1)] = static_cast<std::byte>((std::uint8_t)data.data[decompressedIndex] + top);
                    decompressedIndex++;
                }
            }
            else if (lineFilter == 3)
            {
                auto top{i == 0 ? (std::uint8_t)0 : (std::uint8_t)data.data[decompressedIndex - (i + 1) - data.width]};
                data.data[decompressedIndex - (i + 1)] = static_cast<std::byte>((std::uint8_t)data.data[decompressedIndex] + top / 2);
                decompressedIndex++;
                for (std::int64_t j = 1; j < data.width; j++)
                {
                    top = i == 0 ? (std::uint8_t)0 : (std::uint8_t)data.data[decompressedIndex - (i + 1) - data.width];
                    data.data[decompressedIndex - (i + 1)] = static_cast<std::byte>((std::uint8_t)data.data[decompressedIndex] + (top + (std::uint8_t)data.data[decompressedIndex - (i + 1) - 1]) / 2);
                    decompressedIndex++;
                }
            }
            else if (lineFilter == 4)
            {
                auto top{i == 0 ? (std::uint8_t)0 : (std::uint8_t)data.data[decompressedIndex - (i + 1) - data.width]};
                data.data[decompressedIndex - (i + 1)] = static_cast<std::byte>((std::uint8_t)data.data[decompressedIndex] + (std::uint8_t)data.data[decompressedIndex - (i + 1) - data.width]);
                decompressedIndex++;
                for (std::int64_t j = 1; j < data.width; j++)
                {
                    auto left{(std::uint8_t)data.data[decompressedIndex - (i + 1) - 1]};
                    top = i == 0 ? (std::uint8_t)0 : (std::uint8_t)data.data[decompressedIndex - (i + 1) - data.width];
                    auto topLeft{i == 0 ? (std::uint8_t)0 : (std::uint8_t)data.data[decompressedIndex - (i + 1) - 1 - data.width]};
                    auto threshold{topLeft * 3 - (top + left)};
                    auto low{std::min(top, left)};
                    auto high{std::max(top, left)};
                    auto t0{high <= threshold ? low : topLeft};
                    auto t1{threshold <= low ? high : t0};
                    data.data[decompressedIndex - (i + 1)] = static_cast<std::byte>((std::uint8_t)data.data[decompressedIndex] + t1);
                    decompressedIndex++;
                }
            }
        }

        if (read(descriptor, &crc, sizeof(crc)) != sizeof(crc))
            return false;

        if (read(descriptor, &length, sizeof(length)) != sizeof(length))
            return false;
        length = std::byteswap(length);

        if (read(descriptor, type.getData(), type.getSize()) != type.getSize())
            return false;

        if (type != Array<std::byte, 4>{'I', 'E', 'N', 'D'})
            return false;

        if (read(descriptor, &crc, sizeof(crc)) != sizeof(crc))
            return false;

        close(descriptor);
        return true;
    }

private:
    static bool decompress(int descriptor, std::span<const std::byte> compressed, std::span<std::byte> decompressed)
    {
        auto cmf{static_cast<std::uint8_t>(compressed[0])};
        auto cm{static_cast<std::uint8_t>(cmf & 0xFu)};
        auto flag{static_cast<std::uint8_t>(compressed[1])};

        if (cm != 8 || flag & 32 || (cmf * 256 + flag) % 31)
            return false;

        std::uint32_t bitBuffer{};
        bitBuffer |= static_cast<std::uint8_t>(compressed[2]);
        std::int32_t final = bitBuffer & 1;
        bitBuffer >>= 1;
        std::int32_t type = bitBuffer & 3;
        bitBuffer >>= 2;
        std::int64_t bufferSize = 5;

        if (type != 1 || final != 1)
            return false;

        std::int64_t compressedIndex{3};
        std::int64_t decompressedIndex{};
        while (true)
        {
            while (bufferSize <= 24)
            {
                if (compressedIndex == compressed.size())
                {
                    if (bufferSize < 16)
                        return false;
                    break;
                }

                bitBuffer |= static_cast<std::uint32_t>(compressed[compressedIndex]) << bufferSize;
                bufferSize += 8;
                compressedIndex++;
            }

            auto z = lengthHuffman.decode(bitBuffer, bufferSize);
            if (z < 256)
            {
                decompressed[decompressedIndex] = static_cast<std::byte>(z);
                decompressedIndex++;
            }
            else if (z == 256)
                break;
            else
            {
                z -= 257;
                std::int64_t len = baseLength[z];
                if (extraLength[z])
                {
                    len += bitBuffer & (1 << extraLength[z]) - 1;
                    bitBuffer >>= extraLength[z];
                    bufferSize -= extraLength[z];
                }
                z = distanceHuffman.decode(bitBuffer, bufferSize);
                bitBuffer |= static_cast<std::uint32_t>(compressed[compressedIndex]) << bufferSize;
                bufferSize += 8;
                compressedIndex++;

                std::int64_t dist = baseDistance[z];
                if (extraDistance[z])
                {
                    dist += bitBuffer & (1 << extraDistance[z]) - 1;
                    bitBuffer >>= extraDistance[z];
                    bufferSize -= extraDistance[z];
                }

                std::int64_t startIndex{decompressedIndex - dist};
                for (std::int64_t i = 0; i < len; i++)
                {
                    decompressed[decompressedIndex] = decompressed[startIndex];
                    decompressedIndex++;
                    startIndex++;
                }
            }
        }

        return true;
    }
};

export struct Vec2
{
    float x{};
    float y{};
};
export struct Vec2i
{
    std::uint32_t x{};
    std::uint32_t y{};
};