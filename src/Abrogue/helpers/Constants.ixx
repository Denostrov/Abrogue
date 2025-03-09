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

export class Constants
{
public:
	static constexpr std::string_view configFileName{"config"};
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
};