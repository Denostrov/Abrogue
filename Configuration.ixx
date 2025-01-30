export module Configuration;

export import std;

export class Configuration
{
public:
	static bool init();
	static void release();

	static auto const& getAppName() { return appName; }
	static auto const& getAppVersion() { return appVersion; }
	static auto const& getAppIdentifier() { return appIdentifier; }
	static auto const& getAppCreator() { return appCreator; }
	static auto const& getAppCopyright() { return appCopyright; }
	static auto const& getAppURL() { return appURL; }
	static auto const& getAppType() { return appType; }

	static auto getWindowWidth() { return windowWidth; }
	static auto getWindowHeight() { return windowHeight; }

private:
	inline static std::string appName{"Abrogue"};
	inline static std::string appVersion{"0.1"};
	inline static std::string appIdentifier{"org.strovstiksoft.abrogue"};
	inline static std::string appCreator{"Strovstik Software Inc."};
	inline static std::string appCopyright{"Copyright (c) 2025 Strovstik Software Inc."};
	inline static std::string appURL{"https://github.com/Denostrov"};
	inline static std::string appType{"game"};

	inline static std::uint32_t windowWidth{800};
	inline static std::uint32_t windowHeight{450};
};
