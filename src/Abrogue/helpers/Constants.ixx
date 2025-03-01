export module Constants;

export import std;

export class Constants
{
public:
	static constexpr std::uint64_t tickDurationNS{62500000};
	static constexpr double tickDuration{0.0625};

	static constexpr size_t mapWidth{70};
	static constexpr size_t mapHeight{30};
};