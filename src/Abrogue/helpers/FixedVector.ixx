export module FixedVector;

export import std;

export template<class T, std::size_t N>
class FixedVector
{
	std::array<T, N> data{};
	std::size_t currentSize{};

public:
	using iterator = decltype(data)::iterator;
	using const_iterator = decltype(data)::const_iterator;

	constexpr FixedVector() = default;

	constexpr void erase(const_iterator first, const_iterator last)
	{
		std::size_t distance = last - first;
		for(auto i = currentSize - distance; i < currentSize; i++)
			data[i] = T();

		currentSize -= distance;
	}
	constexpr void clear()
	{
		for(std::size_t i = 0; i < currentSize; i++)
			data[i] = T();

		currentSize = 0;
	}

	constexpr auto begin()
	{
		return data.begin();
	}
	constexpr auto end()
	{
		return data.begin() + currentSize;
	}
	constexpr auto size() const
	{
		return currentSize;
	}
	constexpr auto capacity() const
	{
		return N;
	}

	template<class... Args>
	constexpr void emplace_back(Args&&... args)
	{
		data[currentSize] = T(std::forward<Args>(args)...);

		currentSize++;
	}

	template<class Self>
	constexpr decltype(auto) operator[](this Self&& self, std::size_t index)
	{
		return std::forward<Self>(self).data[index];
	}
};