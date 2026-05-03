export module Helpers:Array;

import std;
import :Logger;
import :Tags;

using namespace std::literals;

export template <class T, auto N>
class Array
{
    static constexpr std::uint64_t size = static_cast<std::uint64_t>(N);

public:
    constexpr Array() = default;
    template<class FillVal>
    constexpr Array(FillVal&& value, FillTag) { fill(std::forward<FillVal>(value)); }

    template<std::size_t OtherSize>
    constexpr Array(T const (&other)[OtherSize])
    {
        static_assert(OtherSize <= (std::size_t)N);
        for (std::size_t i{}; i < OtherSize; i++)
            arr[i] = other[i];
    }

    template <class... Args>
    constexpr Array(Args&&... args) : arr{T{std::forward<Args>(args)}...}
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
    [[nodiscard]] constexpr auto getSize() { return size;}

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

    template <class Self>
    constexpr void fill(this Self&& self, T const& value)
    {
        for (std::uint64_t i{}; i < size; ++i)
            std::forward<Self>(self).arr[i] = value;
    }

private:
    T arr[size]{};
};

template<class... Args>
Array(Args&&... args) -> Array<std::remove_cvref_t<decltype(args...[0])>, sizeof...(args)>;