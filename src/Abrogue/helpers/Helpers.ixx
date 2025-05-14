export module Helpers;

export import std;

export template<class T>
using optRef = std::optional<std::reference_wrapper<T>>;
export template<class T>
using optCRef = std::optional<std::reference_wrapper<T const>>;

//Classes for storing and manipulating RGBA colors
export using PackedColor = std::uint32_t;
export struct Color
{
	constexpr Color() = default;
	constexpr Color(std::uint8_t red, std::uint8_t green, std::uint8_t blue, std::uint8_t alpha)
		: r(red), g(green), b(blue), a(alpha)
	{}
	constexpr Color(PackedColor packedColor)
		: r((packedColor >> 24) & 0xFF), g((packedColor >> 16) & 0xFF), b((packedColor >> 8) & 0xFF), a(packedColor & 0xFF)
	{}

	static constexpr PackedColor pack(std::uint8_t red, std::uint8_t green, std::uint8_t blue, std::uint8_t alpha)
	{
		return (std::uint32_t)red << 24 | (std::uint32_t)green << 16 | (std::uint32_t)blue << 8 | (std::uint32_t)alpha;
	}

	constexpr void multiplyRGB(double coefficient)
	{
		r = std::clamp(r * coefficient, 0.0, 255.0);
		g = std::clamp(g * coefficient, 0.0, 255.0);
		b = std::clamp(b * coefficient, 0.0, 255.0);
	}
	constexpr void multiplyRGBA(double coefficient)
	{
		multiplyRGB(coefficient);
		a = std::clamp(a * coefficient, 0.0, 255.0);
	}

	constexpr PackedColor getPacked() const { return pack(r, g, b, a); }
	constexpr PackedColor getTransparentPacked() const { return pack(r, g, b, 0); }

	std::uint8_t r{}, g{}, b{}, a{};
};