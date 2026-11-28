module;

#include <dlfcn.h>
#include <fcntl.h>
#include <unistd.h>

export module Helpers;

export import std;

using namespace std::literals;

// Convert defines into constexpr flags
#ifdef NDEBUG
export constexpr bool isDebugBuild{false};
#else
export constexpr bool isDebugBuild{true};
#endif

#ifdef EXTRA_ASSERTS
export constexpr bool useExtraAsserts{true};
#else
export constexpr bool useExtraAsserts{false};
#endif

// Helper literals
export constexpr std::int64_t operator""_i64(unsigned long long int val) { return val; }
export constexpr std::uint64_t operator""_u64(unsigned long long int val) { return val; }

// Helper concepts
export template <class T, class U>
concept IsSameType = std::same_as<std::remove_cvref_t<T>, std::remove_cvref_t<U>>;
export template <class T>
concept IsFixedString = requires { T::FixedStringType; };

// Helper template aliases
export template <class T>
using optRef = std::optional<std::reference_wrapper<T>>;
export template <class T>
using optCRef = std::optional<std::reference_wrapper<T const>>;

export template<class... Ts>
class Tuple
{
    struct Storage;
    consteval
    {
        std::vector<std::meta::info> member_specs;
        template for (std::int64_t i{}; auto type : {^^Ts...})
        {
            member_specs.push_back(data_member_spec(type, {.name = std::string(1, 'a' + i)}));
            i++;
        }

        std::meta::define_aggregate(^^Storage, member_specs);
    }
    Storage data;

public:
    constexpr Tuple() = default;
    template <class... Args>
    constexpr Tuple(Args&&... args) :data{std::forward<Args>(args)...}
    {}

    template <class Self, auto I>
    constexpr auto&& get(this Self&& self)
    {
        static constexpr auto field = std::meta::nonstatic_data_members_of(^^Storage, std::meta::access_context::current())[static_cast<std::int64_t>(I)];
        return std::forward<Self>(self).data.[:field:];
    }
};
export template <class Enum, template <Enum> class T>
class EnumTuple
{
    struct Storage;
    consteval
    {
        std::vector<std::meta::info> member_specs;
        static constexpr auto enums = std::define_static_array(std::meta::enumerators_of(^^Enum));
        template for (constexpr auto e : enums)
        {
            constexpr auto eVal = [:e:];
            if (eVal < Enum::COUNT)
                member_specs.push_back(data_member_spec(^^T<eVal>, {.name = std::meta::identifier_of(e)}));
        }

        std::meta::define_aggregate(^^Storage, member_specs);
    }
    Storage data;

public:
    constexpr EnumTuple() = default;
    template <class... Args>
    constexpr EnumTuple(Args&&... args) :data{std::forward<Args>(args)...}
    {}

    template <Enum e, class Self>
    constexpr auto&& get(this Self&& self)
    {
        static constexpr auto field = std::meta::nonstatic_data_members_of(^^Storage, std::meta::access_context::current())[static_cast<std::int64_t>(e)];
        return std::forward<Self>(self).data.[:field:];
    }
};

export struct FillTag
{};

// Class for logging errors and messages to files and console
export class Logger
{
public:
    Logger() = default;

    [[nodiscard]] bool init(void (*callback)(char const*))
    {
        if (!callback)
            return false;
        errorCallback = callback;

        infoLog.open(infoLogFileName.data(), std::ios::binary | std::ios::out | std::ios::trunc);
        errorLog.open(errorLogFileName.data(), std::ios::binary | std::ios::out | std::ios::trunc);
        if (!infoLog || !errorLog)
        {
            logError("Couldn't create log file, check if game folder needs admin permissions"sv);
            return false;
        }

        return true;
    }

    template <class... Args>
    void logError(std::format_string<Args...> fmt, Args&&... args)
    {
        std::array<char, 2048> errorMessageArr{};
        auto stackTrace = std::stacktrace::current();

        std::format_to_n(errorMessageArr.data(), errorMessageArr.size() - 1, fmt, std::forward<Args>(args)...);
        std::string_view errorMessage = errorMessageArr.data();

        std::println(errorLog, "Error: {}\nStacktrace:\n{}", errorMessage, stackTrace);
        std::println(std::cerr, "Error: {}\nStacktrace:\n{}", errorMessage, stackTrace);

        std::array<char, 2048 + 128> popupMessage{};
        std::format_to_n(popupMessage.data(), popupMessage.size() - 1, "{}\nCheck the error log for details. Esc to exit", errorMessage);
        errorCallback(popupMessage.data());
    }

    template <class... Args>
    void logInfo(std::format_string<Args...> fmt, Args&&... args)
    {
        std::println(infoLog, fmt, std::forward<Args>(args)...);
        std::println(std::cout, fmt, std::forward<Args>(args)...);
    }

    void extraAssert(bool condition, std::string_view message)
    {
        if constexpr (useExtraAsserts)
        {
            if (condition)
                return;

            logError("{}", message);
            std::abort();
        }
    }

private:
    std::ofstream infoLog;
    std::ofstream errorLog;
    void (*errorCallback)(char const*){};

    static constexpr auto infoLogFileName{"infoLog.txt"sv};
    static constexpr auto errorLogFileName{"errorLog.txt"sv};
};
export Logger logger;

// Classes for storing and manipulating RGBA colors
export using PackedColor = std::uint32_t;
export struct Color
{
    constexpr Color() = default;

    constexpr Color(std::uint8_t red, std::uint8_t green, std::uint8_t blue, std::uint8_t alpha) : r(red), g(green), b(blue), a(alpha) {}

    constexpr Color(PackedColor packedColor) : r((packedColor >> 24) & 0xFF), g((packedColor >> 16) & 0xFF), b((packedColor >> 8) & 0xFF), a(packedColor & 0xFF)
    {}

    static constexpr PackedColor pack(std::uint8_t red, std::uint8_t green, std::uint8_t blue, std::uint8_t alpha)
    {
        return static_cast<std::uint32_t>(red) << 24 | static_cast<std::uint32_t>(green) << 16 | static_cast<std::uint32_t>(blue) << 8 |
            static_cast<std::uint32_t>(alpha);
    }

    constexpr void multiplyRGB(double coefficient)
    {
        r = static_cast<std::uint8_t>(std::clamp(r * coefficient, 0.0, 255.0));
        g = static_cast<std::uint8_t>(std::clamp(g * coefficient, 0.0, 255.0));
        b = static_cast<std::uint8_t>(std::clamp(b * coefficient, 0.0, 255.0));
    }

    constexpr void multiplyRGBA(double coefficient)
    {
        multiplyRGB(coefficient);
        a = static_cast<std::uint8_t>(std::clamp(a * coefficient, 0.0, 255.0));
    }

    [[nodiscard]] constexpr PackedColor getPacked() const { return pack(r, g, b, a); }
    [[nodiscard]] constexpr PackedColor getTransparentPacked() const { return pack(r, g, b, 0); }

    std::uint8_t r{}, g{}, b{}, a{};
};

export template <class T, auto N>
class Array
{
    static constexpr std::uint64_t size = static_cast<std::uint64_t>(N);

public:
    constexpr Array() = default;
    template <class FillVal>
    constexpr Array(FillVal&& value, FillTag)
    {
        fill(std::forward<FillVal>(value));
    }

    template <std::size_t OtherSize>
    constexpr Array(T const (&other)[OtherSize])
    {
        static_assert(OtherSize <= (std::size_t)N);
        for (std::size_t i{}; i < OtherSize; i++)
            arr[i] = other[i];
    }

    template <class... Args>
    constexpr Array(Args&&... args) : arr{static_cast<T>(std::forward<Args>(args))...}
    {
        static_assert(sizeof...(args) >= size, "Array variadic initializer has too few elements");
        static_assert(sizeof...(args) <= size, "Array variadic initializer has too many elements");
    }

    template <class Self>
    [[nodiscard]] constexpr auto begin(this Self&& self)
    {
        return std::forward<Self>(self).arr;
    }

    template <class Self>
    [[nodiscard]] constexpr auto end(this Self&& self)
    {
        return std::forward<Self>(self).arr + size;
    }

    [[nodiscard]] constexpr auto getData() { return arr; }
    [[nodiscard]] constexpr auto getSize() { return size; }

    template <class Self, class Index>
    [[nodiscard]] constexpr auto&& operator[](this Self&& self, Index index)
    {
        auto castIndex = static_cast<std::size_t>(index);
        if !consteval
        {
            logger.extraAssert(castIndex < size, "Array index out of bounds"sv);
        }

        return std::forward<Self>(self).arr[castIndex];
    }

    [[nodiscard]] constexpr auto operator==(Array const& other) const
    {
        for (std::int64_t i{}; i < size; i++)
        {
            if (arr[i] != other.arr[i])
                return false;
        }

        return true;
    }

    template <class Self>
    constexpr void fill(this Self&& self, T const& value)
    {
        for (std::uint64_t i{}; i < size; i++)
            std::forward<Self>(self).arr[i] = value;
    }

private:
    T arr[size]{};
};

template <class... Args>
Array(Args&&... args) -> Array<std::remove_cvref_t<decltype(args...[0])>, sizeof...(args)>;

// Class for storing a dynamically sized array with fixed capacity on the stack
export template <class T, auto N>
class FixedVector
{
public:
    template <class Self>
    constexpr auto begin(this Self&& self)
    {
        return std::forward<Self>(self).data;
    }
    template <class Self>
    constexpr auto end(this Self&& self)
    {
        return std::forward<Self>(self).data + std::forward<Self>(self).currentSize;
    }

    constexpr auto isEmpty() const { return currentSize == 0; }
    constexpr auto isFull() const { return currentSize == capacity; }
    constexpr auto getSize() const { return currentSize; }
    template <class Self>
    constexpr auto getData(this Self&& self)
    {
        return std::forward<Self>(self).data;
    }
    template <class Self>
    constexpr auto&& getFront(this Self&& self)
    {
        if !consteval
        {
            logger.extraAssert(!isEmpty(), "FixedVector getFront(): container was empty"sv);
        }

        return std::forward<Self>(self).data[0];
    }
    template <class Self>
    constexpr auto&& getBack(this Self&& self)
    {
        if !consteval
        {
            logger.extraAssert(!std::forward<Self>(self).isEmpty(), "FixedVector getBack(): container was empty"sv);
        }

        return std::forward<Self>(self).data[std::forward<Self>(self).currentSize - 1];
    }
    constexpr auto getSpan() const { return std::span<T const>(data, currentSize); }

    constexpr void erase(std::int64_t index)
    {
        if !consteval
        {
            logger.extraAssert(index >= 0 && index < currentSize, "FixedVector erase(): index was out of bounds"sv);
        }

        data[index] = T();
        for (auto i = index; i < currentSize - 1; i++)
            data[i] = std::move(data[i + 1]);
        currentSize--;
    }
    constexpr void popBack() { erase(currentSize - 1); }
    constexpr void clear()
    {
        for (std::int64_t i = 0; i < currentSize; i++)
            data[i] = T();
        currentSize = 0;
    }
    constexpr void resize(std::int64_t newSize)
    {
        for (auto i = newSize; i < currentSize; i++)
            data[i] = T();

        currentSize = newSize;
    }
    template <class... Args>
    constexpr void emplaceBack(Args&&... args)
    {
        if !consteval
        {
            logger.extraAssert(!isFull(), "FixedVector emplaceBack(): container was full"sv);
        }
        data[currentSize] = T(std::forward<Args>(args)...);

        currentSize++;
    }
    constexpr std::int64_t find(T const& key) const
    {
        std::int64_t result{-1};
        for (std::int64_t i = 0; i < currentSize; i++)
        {
            if (data[i] != key)
                continue;

            result = i;
            break;
        }
        return result;
    }

    template <class Self>
    constexpr auto&& operator[](this Self&& self, std::int64_t index)
    {
        if !consteval
        {
            logger.extraAssert(index >= 0 && index < std::forward<Self>(self).currentSize, "FixedVector operator[]: index was out of bounds"sv);
        }

        return std::forward<Self>(self).data[index];
    }

    static constexpr auto capacity{static_cast<std::int64_t>(N)};

private:
    T data[capacity]{};
    std::int64_t currentSize{};
};

/*
 * Class for storing fixed size strings without a null terminator
 * Can be constructed from char array, std::string_view and another FixedString
 * Can be converted to std::string_view
 * Has support for std::format
 */
export template <auto N>
class FixedString
{
public:
    using FixedStringType = void;

    constexpr FixedString() = default;

    template <auto StrSize>
    explicit constexpr FixedString(FixedString<StrSize> const& str) : FixedString(std::string_view{str})
    {}
    template <auto StrSize>
    constexpr auto& operator=(FixedString<StrSize> const& str)
    {
        return *this = std::string_view{str};
    }

    template <auto StrSize>
    explicit constexpr FixedString(char const (&str)[StrSize])
    {
        *this = str;
    }
    template <auto StrSize>
    constexpr auto& operator=(char const (&str)[StrSize])
    {
        static_assert(StrSize - 1 <= capacity, "FixedString operator=(): buffer must have enough capacity");

        currentSize = StrSize - 1;
        for (std::int64_t i = 0; i < currentSize; i++)
        {
            if (str[i] == 0)
            {
                currentSize = i;
                break;
            }
            data[i] = str[i];
        }

        return *this;
    }

    explicit constexpr FixedString(std::string_view str) { *this = str; }
    constexpr auto& operator=(std::string_view str)
    {
        if !consteval
        {
            logger.extraAssert(str.size() <= capacity, "FixedString operator=(): buffer must have enough capacity"sv);
        }

        currentSize = str.size();
        for (std::int64_t i = 0; i < currentSize; i++)
            data[i] = str[i];

        return *this;
    }

    constexpr operator std::string_view() const { return std::string_view(data, currentSize); }

    template <class T, class... Args>
    constexpr void append(T val, Args... args)
        requires(std::same_as<T, std::string_view> || std::same_as<T, char>) && ((std::same_as<Args, std::string_view> || std::same_as<Args, char>) && ...)
    {
        if constexpr (std::same_as<T, std::string_view>)
        {
            if !consteval
            {
                logger.extraAssert(currentSize + val.size() <= N, "FixedString append(): buffer must have enough capacity"sv);
            }

            for (std::int64_t i = currentSize, j = 0; j < val.size(); i++, j++)
                data[i] = val[j];
            currentSize += val.size();
        }
        else
        {
            if !consteval
            {
                logger.extraAssert(currentSize + 1 <= N, "FixedString append(): buffer must have enough capacity"sv);
            }

            data[currentSize] = val;
            currentSize++;
        }

        if constexpr (sizeof...(Args) > 0)
        {
            append(args...);
        }
    }
    template <class T, class... Args>
    constexpr auto& fill(T val, Args... args)
    {
        currentSize = 0;
        append(val, args...);
        return *this;
    }

    constexpr void clear() { currentSize = 0; }

    // Format arguments using std::format and store them
    // Assert if size of formatted string exceeds capacity
    template <class... Args>
    auto& format(std::format_string<Args...> fmt, Args&&... args)
    {
        auto result = std::format_to_n(data, N, fmt, std::forward<Args>(args)...);
        logger.extraAssert(result.size <= N, "FixedString format: formatted string exceeded size"sv);

        currentSize = result.size;

        return *this;
    }

    [[nodiscard]] constexpr auto getData() const { return data; }
    [[nodiscard]] constexpr auto getSize() const { return currentSize; }
    [[nodiscard]] constexpr auto getIsFull() const { return currentSize == capacity; }

    template <class Self>
    [[nodiscard]] constexpr auto&& operator[](this Self&& self, std::size_t index)
    {
        if !consteval
        {
            logger.extraAssert(index < std::forward<Self>(self).currentSize, "FixedString operator[]: index was out of bounds"sv);
        }
        return std::forward<Self>(self).data[index];
    }

    static constexpr auto capacity{static_cast<std::int64_t>(N)};

private:
    char data[capacity]{};
    std::int64_t currentSize{};
};

// Deduction guides for char array and FixedString constructors
template <auto StrSize>
FixedString(char const (&str)[StrSize]) -> FixedString<StrSize - 1>;
template <auto StrSize>
FixedString(FixedString<StrSize> const& str) -> FixedString<StrSize>;

// Specialization for std::format support
// FixedString is equivalent to std::string_view
export template <auto N>
struct std::formatter<FixedString<N>, char>
{
    template <class ParseContext>
    static constexpr ParseContext::iterator parse(ParseContext& context)
    {
        return std::formatter<std::string_view>().parse(context);
    }

    template <class FormatContext>
    static FormatContext::iterator format(FixedString<N> const& str, FormatContext& context)
    {
        return std::formatter<std::string_view>().format(str, context);
    }
};

// xoshiro256** random number generator
export class Random
{
    class SplitMix64
    {
    public:
        SplitMix64(std::uint64_t seed) : state(seed) {}

        std::uint64_t generate()
        {
            std::uint64_t result = (state += 0x9E3779B97f4A7C15);
            result = (result ^ (result >> 30)) * 0xBF58476D1CE4E5B9;
            result = (result ^ (result >> 27)) * 0x94D049BB133111EB;
            return result ^ (result >> 31);
        }

    private:
        std::uint64_t state{};
    };

public:
    Random() = default;
    void seed(std::uint64_t newSeed)
    {
        SplitMix64 startMixer(newSeed);
        state[0] = startMixer.generate();
        state[1] = startMixer.generate();
        state[2] = startMixer.generate();
        state[3] = startMixer.generate();
    }

    std::uint64_t generate()
    {
        std::uint64_t result = rotl(state[1] * 5, 7) * 9;

        std::uint64_t t = state[1] << 17;

        state[2] ^= state[0];
        state[3] ^= state[1];
        state[1] ^= state[2];
        state[0] ^= state[3];

        state[2] ^= t;

        state[3] = rotl(state[3], 45);

        return result;
    }

private:
    static std::uint64_t rotl(std::uint64_t x, std::int32_t k) { return (x << k) | (x >> (64 - k)); }

    std::uint64_t state[4]{};
};
export Random mapRandom;
export Random visualRandom;

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

        if constexpr (std::convertible_to<Value, std::string_view>)
        {
            if (valueStr.getSize() > value.capacity)
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
        if constexpr (IsFixedString<Value>)
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
    template <auto sizeCount>
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
                    data.data[decompressedIndex - (i + 1)] =
                        static_cast<std::byte>((std::uint8_t)data.data[decompressedIndex - (i + 1) - 1] + (std::uint8_t)data.data[decompressedIndex]);
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
                    data.data[decompressedIndex - (i + 1)] = static_cast<std::byte>((std::uint8_t)data.data[decompressedIndex] +
                                                                                    (top + (std::uint8_t)data.data[decompressedIndex - (i + 1) - 1]) / 2);
                    decompressedIndex++;
                }
            }
            else if (lineFilter == 4)
            {
                auto top{i == 0 ? (std::uint8_t)0 : (std::uint8_t)data.data[decompressedIndex - (i + 1) - data.width]};
                data.data[decompressedIndex - (i + 1)] =
                    static_cast<std::byte>((std::uint8_t)data.data[decompressedIndex] + (std::uint8_t)data.data[decompressedIndex - (i + 1) - data.width]);
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

export template <auto bufferSize, auto allocationCount>
class BumpAllocator
{
public:
    void* allocate(std::int64_t newSize)
    {
        logger.extraAssert(newSize > 0, "BumpAllocator allocate(): allocation size must be > 0"sv);
        logger.extraAssert(currentSize + newSize <= bufferSize, "BumpAllocator allocate(): not enough space for allocation"sv);

        void* result = &buffer[currentSize];
        currentSize += newSize;

        while (currentSize <= bufferSize && currentSize % alignof(std::max_align_t) != 0)
            currentSize++;

        blocks.emplaceBack(result, newSize);

        return result;
    }

    void* reallocate(void* old, std::int64_t newSize)
    {
        std::int64_t oldSize{};
        for (std::int64_t i = blocks.getSize() - 1; i >= 0; i--)
        {
            if (blocks[i].ptr != old)
                continue;

            oldSize = blocks[i].allocationSize;
        }
        logger.extraAssert(oldSize > 0, "BumpAllocator reallocate(): old pointer must be a valid allocation"sv);

        auto result = allocate(newSize);
        std::memcpy(result, old, std::min(oldSize, newSize));

        blocks.emplaceBack(result, newSize);

        return result;
    }

    void free(void* ptr)
    {
        std::int64_t index{-1};
        for (std::int64_t i = blocks.getSize() - 1; i >= 0; i--)
        {
            if (ptr != blocks[i].ptr)
                continue;

            index = i;
            break;
        }
        logger.extraAssert(index >= 0, "BumpAllocator free(): pointer must be a valid allocation"sv);

        if (index == blocks.getSize() - 1)
        {
            if (index == 0)
                currentSize = 0;
            else
            {
                currentSize = static_cast<std::byte*>(blocks[index - 1].ptr) - buffer + blocks[index - 1].allocationSize;
                while (currentSize <= bufferSize && currentSize % alignof(std::max_align_t) != 0)
                    currentSize++;
            }
        }
        blocks.erase(index);
    }

private:
    alignas(std::max_align_t) std::byte buffer[bufferSize]{};
    std::int64_t currentSize{};

    struct Block
    {
        void* ptr{};
        std::int64_t allocationSize{};
    };
    FixedVector<Block, allocationCount> blocks;
};
