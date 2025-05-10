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
		std::int64_t originX{}, originY{};
		std::int64_t width{}, height{};
	};

	struct LevelData
	{
		std::array<Room, 64> rooms{};
		std::uint64_t roomCount{};

		std::array<bool, Constants::mapTileCount> tilesOccupiedMask{};
	};

public:
	Map() = default;
	void init();

	void updateDraw(double deltaTime);

	bool getTileSolid(std::int64_t x, std::int64_t y) const;
	bool getTileOpaque(std::int64_t x, std::int64_t y) const;
	bool getTileFloor(std::int64_t x, std::int64_t y) const;
	double getTileBrightness(std::int64_t x, std::int64_t y) const;

private:
	decltype(auto) getTile(this auto& self, std::int64_t x, std::int64_t y) { return self.tiles[x + y * Constants::mapWidth]; }

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

	std::array<Tile, Constants::mapTileCount> tiles;
	LevelData levelData;

	std::array<std::uint64_t, Constants::mapTileCount> lastVisibleTiles{};
	std::uint64_t lastVisibleTilesSize{};

	std::array<double, Constants::mapTileCount> tileBrightnessMask{};
};