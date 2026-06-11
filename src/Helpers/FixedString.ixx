export module Helpers:FixedString;

import std;
import :Logger;

using namespace std::literals;

//Class for storing fixed size strings without a null terminator
//Can be constructed from char array, std::string_view and another FixedString
//Can be converted to std::string_view
//Has support for std::format
export template <std::size_t N>
class FixedString
{
public:
    constexpr FixedString() = default;

    //Constructing from another FixedString
    //Assert if the size of other FixedString exceeds capacity
    template <std::size_t StrSize>
    explicit constexpr FixedString(FixedString<StrSize> const& str)
    {
        *this = str;
    }

    template <std::size_t StrSize>
    constexpr auto& operator=(FixedString<StrSize> const& str)
    {
        static_assert(str.getSize() <= N, "FixedString assignment: source FixedString exceeds capacity");

        currentSize = str.getSize();
        for (std::size_t i = 0; i < currentSize; i++)
            data[i] = str[i];

        return *this;
    }

    //Constructing from a char array
    //Copy contents without the null terminator
    template <std::size_t StrSize>
    explicit constexpr FixedString(char const (&str)[StrSize])
    {
        *this = str;
    }

    template <std::size_t StrSize>
    constexpr auto& operator=(char const (&str)[StrSize])
    {
        static_assert(StrSize - 1 <= N, "FixedString assignment: source char array exceeds capacity");

        currentSize = StrSize - 1;
        for (std::size_t i = 0; i < currentSize; i++)
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

    //Constructing from and converting to std::string_view
    //Assert if the size of std::string_view exceeds capacity
    explicit constexpr FixedString(std::string_view str)
    {
        *this = str;
    }

    constexpr auto& operator=(std::string_view str)
    {
        if !consteval
        {
            logger.extraAssert(str.size() <= N, "FixedString assignment: source string_view exceeds capacity"sv);
        }

        currentSize = str.size();
        for (std::size_t i = 0; i < currentSize; i++)
            data[i] = str[i];

        return *this;
    }

    constexpr operator std::string_view() const
    {
        return std::string_view(data, currentSize);
    }

    template <std::same_as<std::string_view>... Args>
    constexpr void append(std::string_view val, Args... args)
    {
        if !consteval
        {
            logger.extraAssert(currentSize + val.size() <= N, "FixedString append: source string_view exceeds capacity"sv);
        }

        for (std::size_t i = currentSize, j = 0; j < val.size(); i++, j++)
            data[i] = val[j];
        currentSize += val.size();

        if constexpr (sizeof...(Args) > 0)
        {
            append(args...);
        }
    }

    //Clear storage and copy multiple std::string_view into it
    //Assert if total size of std::string_view arguments exceeds capacity
    template <std::same_as<std::string_view>... Args>
    constexpr auto& fill(std::string_view val, Args... args)
    {
        currentSize = 0;
        append(val, args...);
        return *this;
    }

    //Format arguments using std::format and store them
    //Assert if size of formatted string exceeds capacity
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
    [[nodiscard]] static constexpr auto getCapacity() { return N; }

    template <class Self>
    [[nodiscard]] constexpr auto&& operator[](this Self&& self, std::size_t index)
    {
        if !consteval
        {
            logger.extraAssert(index < std::forward<Self>(self).currentSize, "FixedString operator[]: index was out of bounds"sv);
        }
        return std::forward<Self>(self).data[index];
    }

private:
    char data[N]{};
    std::size_t currentSize{};
};

//Deduction guides for char array and FixedString constructors
template <std::size_t StrSize>
FixedString(char const (&str)[StrSize]) -> FixedString<StrSize - 1>;
template <std::size_t StrSize>
FixedString(FixedString<StrSize> const& str) -> FixedString<StrSize>;

//Specialization for std::format support
//FixedString is equivalent to std::string_view
export template <std::size_t N>
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