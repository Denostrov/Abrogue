export module Configuration;

export import Helpers;
export import FixedVector;

//Enum for visual types of weapons
export enum class WeaponType
{
	eClaw,
	eClub,
	eDagger
};

//Struct for defining an enemy type
export struct EnemyData
{
	FixedVector<char, 16> name;
	std::uint8_t symbol{};
	Color color{};
	double speed{10.0};
	double mass{10.0};
	WeaponType weaponType{};
	Color weaponColor{};
	std::int64_t damage{};
	double attackTime{0.25};
};

//Class for saving and loading configuration options
export class Configuration
{
public:
	[[nodiscard]] bool load();

	[[nodiscard]] auto getWindowWidth() const { return windowWidth; }
	[[nodiscard]] auto getWindowHeight() const { return windowHeight; }

	[[nodiscard]] optCRef<EnemyData> getSuitableEnemy();

private:
	bool saveOptionsToFile();

	std::int64_t windowWidth{800};
	std::int64_t windowHeight{450};

	FixedVector<EnemyData, 128> enemyData;
};

export inline Configuration configuration;