export module Configuration;

export import Helpers;

export enum class WeaponType
{
	eClaw,
	eClub,
	eDagger
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
