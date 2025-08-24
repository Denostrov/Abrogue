export module FixedString;

export import Logger;

using namespace std::literals;

//Class for storing fixed size strings without a null terminator
//Can be constructed from char array, std::string_view and another FixedString
//Can be converted to std::string_view
//Has support for std::format
export template<std::size_t N>
class FixedString
{
public:
	constexpr FixedString() = default;

	//Constructing from another FixedString
	//Assert if the size of other FixedString exceeds capacity
	template<std::size_t StrSize>
	constexpr FixedString(FixedString<StrSize> const& str)
	{
		*this = str;
	}
	template<std::size_t StrSize>
	constexpr auto& operator=(FixedString<StrSize> const& str)
	{
		if !consteval
		{
			logger.extraAssert(str.getSize() <= N, "FixedString assignment: source FixedString exceeds size"sv);
		}

		currentSize = StrSize;
		for(std::size_t i = 0; i < StrSize; i++)
			data[i] = str[i];

		return *this;
	}

	//Constructing from a char array
	//Copy contents without the null terminator
	template<std::size_t StrSize>
	constexpr FixedString(char const(&str)[StrSize])
	{
		*this = str;
	}
	template<std::size_t StrSize>
	constexpr auto& operator=(char const(&str)[StrSize])
	{
		static_assert(StrSize - 1 <= N, "FixedString assignment: source char array exceeds size");

		currentSize = StrSize - 1;
		for(std::size_t i = 0; i < StrSize - 1; i++)
			data[i] = str[i];

		return *this;
	}

	//Constructing from and converting to std::string_view
	//Assert if the size of std::string_view exceeds capacity
	constexpr FixedString(std::string_view str)
	{
		*this = str;
	}
	constexpr auto& operator=(std::string_view str)
	{
		if !consteval
		{
			logger.extraAssert(str.size() <= N, "FixedString assignment: source string_view exceeds size"sv);
		}

		currentSize = str.size();
		for(std::size_t i = 0; i < currentSize; i++)
			data[i] = str[i];

		return *this;
	}
	constexpr operator std::string_view() const
	{
		return std::string_view(data.data(), currentSize);
	}

	//Clear storage and copy multiple std::string_view into it
	//Assert if total size of std::string_view arguments exceeds capacity
	template<std::same_as<std::string_view>... Args>
	constexpr auto& fill(Args... args)
	{
		currentSize = 0;

		auto appendStr = [this](this auto&& self, std::string_view str, auto... args) constexpr
		{
			if !consteval
			{
				logger.extraAssert(currentSize + str.size() <= N, "FixedString fill: arguments exceed size"sv);
			}

			for(std::size_t i = currentSize, j = 0; j < str.size(); i++, j++)
				data[i] = str[j];
			currentSize += str.size();

			if constexpr(sizeof...(args) > 0)
				std::forward<decltype(self)>(self)(args...);
		};
		appendStr(args...);

		return *this;
	}

	//Format arguments using std::format and store them
	//Assert if size of formatted string exceeds capacity
	template<class... Args>
	auto& format(std::format_string<Args...> fmt, Args&&... args)
	{
		auto result = std::format_to_n(data.begin(), data.end() - data.begin(), fmt, std::forward<Args>(args)...);
		logger.extraAssert(result.size <= N, "FixedString format: formatted string exceeded size"sv);
		currentSize = result.size;
		return *this;
	}

	constexpr std::size_t getSize() const { return currentSize; }
	template<class Self>
	constexpr char operator[](this Self&& self, std::size_t index) {
		if !consteval
		{
			logger.extraAssert(index < std::forward<Self>(self).currentSize, "FixedString operator[]: index was out of bounds"sv);
		}
		return std::forward<Self>(self).data[index]; }

private:
	std::array<char, N> data{};
	std::size_t currentSize{};
};

//Deduction guides for char array and FixedString constructors
template<std::size_t StrSize>
FixedString(char const(&str)[StrSize]) -> FixedString<StrSize - 1>;
template<std::size_t StrSize>
FixedString(FixedString<StrSize> const& str) -> FixedString<StrSize>;

//Specialization for std::format support
//FixedString is equivalent to std::string_view
export template<std::size_t N>
struct std::formatter<FixedString<N>, char>
{
	template<class ParseContext>
	constexpr ParseContext::iterator parse(ParseContext& context)
	{
		return std::formatter<std::string_view, char>().parse(context);
	}
	template<class FormatContext>
	FormatContext::iterator format(FixedString<N> const& str, FormatContext& context) const
	{
		return std::formatter<std::string_view, char>().format(str, context);
	}
};