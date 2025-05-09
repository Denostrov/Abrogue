export module Configuration;

export import std;

export enum class WeaponType
{
	eClaw,
	eClub,
	eDagger
};

export using PackedColor = std::uint32_t;
export struct Color
{
	constexpr Color() = default;
	constexpr Color(std::uint8_t red, std::uint8_t green, std::uint8_t blue, std::uint8_t alpha)
		:r(red), g(green), b(blue), a(alpha)
	{}

	constexpr PackedColor getPacked() const { return (std::uint32_t)r << 24 | (std::uint32_t)g << 16 | (std::uint32_t)b << 8 | (std::uint32_t)a; }
	constexpr PackedColor getTransparentPacked() const { return (std::uint32_t)r << 24 | (std::uint32_t)g << 16 | (std::uint32_t)b << 8; }

	std::uint8_t r{}, g{}, b{}, a{};
};

export struct EnemyData
{
	std::string name;
	std::uint8_t symbol{};
	Color color{};
	double speed{10.0};
	double mass{10.0};
	WeaponType weaponType{};
	Color weaponColor{};
	std::int64_t damage{};
	double attackTime{0.25};
};

export class Configuration
{
public:
	bool load();

	auto getWindowWidth() const { return windowWidth; }
	auto getWindowHeight() const { return windowHeight; }

	EnemyData const& getSuitableEnemy();

private:
	bool saveToFile();

	std::uint32_t windowWidth{800};
	std::uint32_t windowHeight{450};

	std::vector<EnemyData> enemyData;
};
