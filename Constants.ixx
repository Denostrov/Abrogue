export module Constants;

export import std;

export class Constants
{
public:
	static constexpr std::uint64_t tickDurationNS{31250000};
	static constexpr double tickDuration{0.03125};
};