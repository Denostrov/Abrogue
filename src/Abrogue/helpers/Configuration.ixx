export module Configuration;

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

export class Configuration
{
public:
	auto getWindowWidth() { return windowWidth; }
	auto getWindowHeight() { return windowHeight; }

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

private:
	bool load();
	bool saveToFile();

	std::uint32_t windowWidth{800};
	std::uint32_t windowHeight{450};

	friend class Game;
};
