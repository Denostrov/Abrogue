export module Constants;

export import std;

#ifdef NDEBUG
export inline constexpr bool isDebugBuild{false};
#else
export inline constexpr bool isDebugBuild{true};
#endif

#ifdef EXTRA_ASSERTS
export inline constexpr bool useExtraAsserts{true};
#else
export inline constexpr bool useExtraAsserts{false};
#endif

export class Helpers
{
public:
	static constexpr std::uint32_t packColor(std::uint8_t red, std::uint8_t green, std::uint8_t blue, std::uint8_t alpha)
	{
		return (std::uint32_t)red << 24 | (std::uint32_t)green << 16 | (std::uint32_t)blue << 8 | (std::uint32_t)alpha;
	}
	static constexpr std::tuple<std::uint8_t, std::uint8_t, std::uint8_t, std::uint8_t> unpackColor(std::uint32_t color)
	{
		return {(color >> 24) & 0xFF, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF};
	}
};

export class Constants
{
public:
	static constexpr std::string_view configFileName{"config"};
	static constexpr std::string_view dataFileName{"data"};
	static constexpr std::string_view infoLogFileName{"infoLog"};
	static constexpr std::string_view errorLogFileName{"errorLog"};

	static constexpr std::string_view appName{"Abrogue"};
	static constexpr std::string_view appVersion{"0.1"};
	static constexpr std::string_view appIdentifier{"org.strovstiksoft.abrogue"};
	static constexpr std::string_view appCreator{"Strovstik Software Inc."};
	static constexpr std::string_view appCopyright{"Copyright (c) 2025 Strovstik Software Inc."};
	static constexpr std::string_view appURL{"https://github.com/Denostrov"};
	static constexpr std::string_view appType{"game"};

	static constexpr std::uint32_t vkAppMajorVersion{0};
	static constexpr std::uint32_t vkAppMinorVersion{1};
	static constexpr std::uint32_t vkAppPatchVersion{0};

	static constexpr std::uint64_t ticksPerSecond{32};
	static constexpr std::uint64_t tickDurationNS{1000000000 / ticksPerSecond};
	static constexpr double tickDuration{1.0 / ticksPerSecond};

	static constexpr size_t screenWidth{128};
	static constexpr size_t screenHeight{36};

	static constexpr size_t mapWidth{80};
	static constexpr size_t mapHeight{36};

	static constexpr double tileAspectRatio{0.5};

	static constexpr std::uint32_t labelBackgroundColor{Helpers::packColor(255, 255, 255, 0)};
	static constexpr std::uint32_t labelHoveredColor{Helpers::packColor(8, 8, 8, 255)};
	static constexpr std::uint32_t labelPressedColor{Helpers::packColor(16, 16, 16, 255)};
	static constexpr std::uint32_t labelHoveredPressedColor{Helpers::packColor(24, 24, 24, 255)};
	static constexpr std::uint32_t healthBackgroundColor{Helpers::packColor(128, 0, 0, 255)};
	static constexpr std::uint32_t healthHoverColor{Helpers::packColor(160, 0, 0, 255)};
	static constexpr std::uint32_t nutritionBackgroundColor{Helpers::packColor(16, 16, 128, 255)};
	static constexpr std::uint32_t nutritionHoverColor{Helpers::packColor(32, 32, 160, 255)};
};