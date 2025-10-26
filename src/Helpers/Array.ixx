export module Helpers:Array;

import std;
import :Logger;
import :Tags;

using namespace std::literals;

export template <class T, auto N>
class Array
{
public:
    static constexpr std::size_t size = static_cast<std::size_t>(N);

    constexpr Array() = default;
    template<class FillVal>
    constexpr Array(FillVal&& value, FillTag) { fill(std::forward<FillVal>(value)); }

    template<std::size_t OtherSize>
    constexpr Array(T const (&other)[OtherSize])
    {
        static_assert(OtherSize <= (std::size_t)N);
        for (std::size_t i = 0; i < OtherSize; i++)
            data[i] = other[i];
    }

    template <class... Args>
    constexpr Array(Args&&... args) : data{T{std::forward<Args>(args)}...}
    {
        static_assert(sizeof...(args) >= size, "Array variadic initializer has too few elements");
        static_assert(sizeof...(args) <= size, "Array variadic initializer has too many elements");
    }

    template<IsLikePair... Args>
    constexpr Array(Args&&... args)
    {
        static_assert(sizeof...(args) >= size, "Array variadic pair initializer has too few elements");
        static_assert(sizeof...(args) <= size, "Array variadic pair initializer has too many elements");

        fillWithPairs(std::forward<Args>(args)...);
    }

    template <class Self>
    [[nodiscard]] constexpr auto begin(this Self&& self)
    {
        return std::forward<Self>(self).data;
    }

    template <class Self>
    [[nodiscard]] constexpr auto end(this Self&& self)
    {
        return std::forward<Self>(self).data + size;
    }

    template <class Self, class Index>
    [[nodiscard]] constexpr auto&& operator[](this Self&& self, Index index)
    {
        auto castIndex = static_cast<std::size_t>(index);
        if !consteval
        {
            logger.extraAssert(castIndex < size, "Array index out of bounds"sv);
        }

        return std::forward<Self>(self).data[castIndex];
    }

    template <class Self>
    constexpr void fill(this Self&& self, T const& value)
    {
        for (std::size_t i = 0; i < size; ++i)
            std::forward<Self>(self).data[i] = value;
    }

    T data[size]{};

private:
    template<class Pair, class... Args>
    constexpr void fillWithPairs(Pair&& pair, Args&&... args)
    {
        data[static_cast<std::size_t>(std::forward<Pair>(pair).first)] = std::forward<Pair>(pair).second;
        if constexpr (sizeof...(args) > 0)
            fillWithPairs(std::forward<Args>(args)...);
    }
};

template<class... Args>
Array(Args&&... args) -> Array<std::remove_cvref_t<decltype(args...[0])>, sizeof...(args)>;