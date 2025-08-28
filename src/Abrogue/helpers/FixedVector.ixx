export module FixedVector;

export import std;

export template<class T>
concept IsCharArray = std::same_as<typename T::value_type, char>;

//Class for storing a dynamically sized array with fixed capacity on the stack
export template<class T, std::size_t N>
class FixedVector
{
	std::array<T, N> data{};
	std::size_t currentSize{};

public:
	using iterator = decltype(data)::iterator;
	using const_iterator = decltype(data)::const_iterator;
	using value_type = T;

	constexpr FixedVector() = default;

	constexpr void erase(const_iterator first)
	{
		std::size_t eraseIndex = first - data.begin();
		data[eraseIndex] = T();

		for(auto i = eraseIndex; i < currentSize - 1; i++)
			data[i] = std::move(data[i + 1]);

		currentSize--;
	}
	constexpr void erase(const_iterator first, const_iterator last)
	{
		std::size_t distance = last - first;
		std::size_t eraseBegin = first - data.begin();
		for(auto i = eraseBegin; i < eraseBegin + distance; i++)
			data[i] = T();

		for(auto i = eraseBegin; i < currentSize - distance; i++)
			data[i] = std::move(data[i + distance]);

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
	constexpr auto empty() const
	{
		return currentSize == 0;
	}
	constexpr auto isFull() const
	{
		return currentSize == data.size();
	}

	template<class Self>
	constexpr decltype(auto) front(this Self&& self)
	{
		return std::forward<Self>(self).data[0];
	}
	template<class Self>
	constexpr decltype(auto) back(this Self&& self)
	{
		return std::forward<Self>(self).data[self.currentSize - 1];
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

export template<std::size_t N>
constexpr bool operator==(FixedVector<char, N> const& lhs, std::string_view rhs)
{
	if(lhs.size() != rhs.size())
		return false;

	for(std::size_t i = 0; i < rhs.size(); i++)
	{
		if(lhs[i] != rhs[i])
			return false;
	}
	return true;
}

export template<std::size_t N>
constexpr bool operator!=(FixedVector<char, N> const& lhs, std::string_view rhs)
{
	return !(lhs == rhs);
}