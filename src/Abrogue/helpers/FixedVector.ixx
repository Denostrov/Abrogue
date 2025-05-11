export module FixedVector;

export import std;

export template<class T, std::size_t N>
class FixedVector
{
public:
	constexpr FixedVector() = default;

	template<class Self>
	constexpr decltype(auto) operator[](this Self&& self, std::size_t index)
	{
		return std::forward<Self>(self).data[index];
	}

private:
	std::array<T, N> data{};
	std::size_t size{};
};