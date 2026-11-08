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

    template<class Value>
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
        else if constexpr(std::is_same_v<Value, bool>)
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
        else if constexpr(std::is_same_v<Value, char>)
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

    template<class Value>
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
        else if constexpr(std::is_same_v<Value, bool>)
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