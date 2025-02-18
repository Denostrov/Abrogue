export module Constants;

export import std;

export class Constants
{
public:
	static constexpr std::uint64_t tickDurationNS{62500000};
	static constexpr double tickDuration{0.0625};
};