export module Constants;

export import std;

export class Constants
{
public:
	static constexpr std::uint64_t ticksPerSecond{32};
	static constexpr std::uint64_t tickDurationNS{1000000000 / ticksPerSecond};
	static constexpr double tickDuration{1.0 / ticksPerSecond};

	static constexpr size_t screenWidth{128};
	static constexpr size_t screenHeight{36};

	static constexpr size_t mapWidth{80};
	static constexpr size_t mapHeight{36};
};