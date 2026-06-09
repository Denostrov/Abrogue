export module Helpers:FixedVector;

import std;
import :Logger;

using namespace std::literals;

// Class for storing a dynamically sized array with fixed capacity on the stack
export template <class T, std::size_t N>
class FixedVector
{
public:
    constexpr FixedVector() = default;

    constexpr void erase(std::size_t index)
    {
        if !consteval
        {
            logger.extraAssert(index < N, "FixedVector erase(): index was out of bounds"sv);
        }

        data[index] = T();
        for (auto i = index; i < currentSize - 1; ++i)
            data[i] = std::move(data[i + 1]);

        currentSize--;
    }

    constexpr void popBack() { erase(currentSize - 1); }

    constexpr void clear()
    {
        for (std::size_t i = 0; i < currentSize; i++)
            data[i] = T();

        currentSize = 0;
    }

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

    constexpr auto getSize() const { return currentSize; }

    static constexpr auto getCapacity() { return N; }

    constexpr auto isEmpty() const { return currentSize == 0; }

    constexpr auto isFull() const { return currentSize == N; }

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

    constexpr void resize(std::size_t newSize)
    {
        for (std::size_t i = currentSize; i < newSize; i++)
            data[i] = T();

        for (std::size_t i = currentSize; i > newSize; i--)
            data[i - 1] = T();

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

    template <class Self>
    constexpr auto&& operator[](this Self&& self, std::size_t index)
    {
        if !consteval
        {
            logger.extraAssert(index < std::forward<Self>(self).currentSize, "FixedVector operator[]: index was out of bounds"sv);
        }

        return std::forward<Self>(self).data[index];
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

private:
    T data[N]{};
    std::size_t currentSize{};
};
