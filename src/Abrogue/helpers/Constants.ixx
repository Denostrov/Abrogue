export module Constants;

export import Helpers;

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

using namespace std::literals;

export class Constants
{
public:
	static constexpr std::string_view configFileName{"config.json"sv};
	static constexpr std::string_view dataFileName{"data.json"sv};
	static constexpr std::string_view infoLogFileName{"infoLog"sv};
	static constexpr std::string_view errorLogFileName{"errorLog"sv};

	static constexpr std::string_view appName{"Abrogue"sv};
	static constexpr std::string_view appVersion{"0.1"sv};
	static constexpr std::string_view appIdentifier{"org.strovstiksoft.abrogue"sv};
	static constexpr std::string_view appCreator{"Strovstik Software Inc."sv};
	static constexpr std::string_view appCopyright{"Copyright (c) 2025 Strovstik Software Inc."sv};
	static constexpr std::string_view appURL{"https://github.com/Denostrov"sv};
	static constexpr std::string_view appType{"game"sv};

	static constexpr std::uint32_t vkAppMajorVersion{0};
	static constexpr std::uint32_t vkAppMinorVersion{1};
	static constexpr std::uint32_t vkAppPatchVersion{0};

	static constexpr std::int64_t ticksPerSecond{32};
	static constexpr std::int64_t tickDurationNS{1000000000 / ticksPerSecond};
	static constexpr double tickDuration{1.0 / ticksPerSecond};

	static constexpr std::int64_t screenWidth{128};
	static constexpr std::int64_t screenHeight{36};

	static constexpr std::int64_t mapWidth{80};
	static constexpr std::int64_t mapHeight{36};
	static constexpr size_t mapTileCount{mapWidth * mapHeight};
	static constexpr std::int64_t mapOffset{screenWidth - mapWidth};

	static constexpr float tileAspectRatio{0.5f};
	static constexpr float tileScaleX{tileAspectRatio / Constants::screenHeight};
	static constexpr float tileScaleY{1.0f / Constants::screenHeight};

	static constexpr PackedColor labelBackgroundColor{Color::pack(255, 255, 255, 0)};
	static constexpr PackedColor labelHoveredColor{Color::pack(8, 8, 8, 255)};
	static constexpr PackedColor labelPressedColor{Color::pack(16, 16, 16, 255)};
	static constexpr PackedColor labelHoveredPressedColor{Color::pack(24, 24, 24, 255)};
	static constexpr PackedColor healthBackgroundColor{Color::pack(128, 0, 0, 255)};
	static constexpr PackedColor healthHoverColor{Color::pack(160, 0, 0, 255)};
	static constexpr PackedColor nutritionBackgroundColor{Color::pack(16, 16, 128, 255)};
	static constexpr PackedColor nutritionHoverColor{Color::pack(32, 32, 160, 255)};
};