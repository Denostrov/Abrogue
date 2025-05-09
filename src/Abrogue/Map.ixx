export module Map;

export import QuadPool;

export class Map
{
	enum class TileType
	{
		eFloor,
		eGrass,
		eBush,
		eWall,
		eBedrock,
		eDoor,
		eExit,
		COUNT
	};

	struct Tile
	{
		QuadPool::Reference quadReference;
		TileType type{TileType::eWall};
	};

	struct TileInfo
	{
		std::uint32_t color{};
		std::uint32_t backgroundColor{};
		std::uint32_t glyph{};
	};

	struct Room
	{
		std::int32_t originX{}, originY{};
		std::uint64_t width{}, height{};
	};

	struct LevelData
	{
		std::array<Room, 64> rooms{};
		std::uint64_t roomCount{};

		std::array<bool, Constants::mapWidth* Constants::mapHeight> tilesOccupiedMask{};
	};

public:
	Map() = default;
	void init();

	void updateDraw(double deltaTime);

	bool getTileSolid(std::int32_t x, std::int32_t y) const;
	bool getTileOpaque(std::int32_t x, std::int32_t y) const;
	double getTileBrightness(std::int32_t x, std::int32_t y) const;

private:
	decltype(auto) getTile(this auto& self, std::int32_t x, std::int32_t y) { return self.tiles[x + Constants::mapWidth * y]; }

	void generateLevel();

	static constexpr std::array<TileInfo, (size_t)TileType::COUNT> tilesInfo{
		TileInfo{Helpers::packColor(96, 96, 96, 255), Helpers::packColor(8, 8, 8, 255), 250},
		TileInfo{Helpers::packColor(0, 255, 0, 255), Helpers::packColor(8, 8, 8, 255), 34},
		TileInfo{Helpers::packColor(0, 255, 0, 255), Helpers::packColor(8, 8, 8, 255), 237},
		TileInfo{Helpers::packColor(255, 255, 255, 255), Helpers::packColor(32, 32, 32, 255), 35},
		TileInfo{Helpers::packColor(255, 255, 255, 255), Helpers::packColor(16, 16, 16, 255), 35},
		TileInfo{Helpers::packColor(255, 128, 0, 255), Helpers::packColor(192, 64, 0, 255), 43},
		TileInfo{Helpers::packColor(255, 255, 255, 255), Helpers::packColor(16, 16, 192, 255), 234}
	};

	std::array<Tile, Constants::mapWidth* Constants::mapHeight> tiles;
	LevelData levelData;

	std::array<std::uint64_t, Constants::mapWidth* Constants::mapHeight> lastVisibleTiles{};
	std::uint64_t lastVisibleTilesSize{};

	std::array<double, Constants::mapWidth* Constants::mapHeight> tileBrightnessMask{};
};