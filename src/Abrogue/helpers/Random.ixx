export module Random;

export import std;

export class Random
{
	class SplitMix64
	{
	public:
		SplitMix64(std::uint64_t seed)
			:state(seed)
		{}

		std::uint64_t generate()
		{
			std::uint64_t result = (state += 0x9E3779B97f4A7C15);
			result = (result ^ (result >> 30)) * 0xBF58476D1CE4E5B9;
			result = (result ^ (result >> 27)) * 0x94D049BB133111EB;
			return result ^ (result >> 31);
		}

	private:
		std::uint64_t state{};
	};

public:
	Random() = default;
	void seed(std::uint64_t newSeed)
	{
		SplitMix64 startMixer(newSeed);
		state[0] = startMixer.generate();
		state[1] = startMixer.generate();
		state[2] = startMixer.generate();
		state[3] = startMixer.generate();
	}

	std::uint64_t generate()
	{
		std::uint64_t result = rotl(state[1] * 5, 7) * 9;

		std::uint64_t t = state[1] << 17;

		state[2] ^= state[0];
		state[3] ^= state[1];
		state[1] ^= state[2];
		state[0] ^= state[3];

		state[2] ^= t;

		state[3] = rotl(state[3], 45);

		return result;
	}

private:
	std::uint64_t rotl(std::uint64_t x, std::int32_t k)
	{
		return (x << k) | (x >> (64 - k));
	}

	std::uint64_t state[4]{};
};