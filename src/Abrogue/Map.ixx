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
		COUNT
	};

	struct Tile
	{
		QuadPool::Reference quadReference;
		TileType type{TileType::eFloor};
	};

	struct TileInfo
	{
		std::uint32_t color{};
		std::uint32_t backgroundColor{};
		std::uint32_t glyph{};
	};
public:
	Map() = default;
	void init();

	void updateDraw(double deltaTime);

	bool getTileSolid(std::int32_t x, std::int32_t y) const;

private:
	decltype(auto) getTile(this auto& self, std::int32_t x, std::int32_t y) { return self.tiles[x + Constants::mapWidth * y]; }

	static constexpr std::array<TileInfo, (size_t)TileType::COUNT> tilesInfo{
		TileInfo{Helpers::packColor(96, 96, 96, 255), Helpers::packColor(4, 4, 4, 255), 250},
		TileInfo{Helpers::packColor(0, 255, 0, 255), Helpers::packColor(4, 4, 4, 255), 34},
		TileInfo{Helpers::packColor(0, 255, 0, 255), Helpers::packColor(4, 4, 4, 255), 237},
		TileInfo{Helpers::packColor(255, 255, 255, 255), Helpers::packColor(32, 32, 32, 255), 35},
		TileInfo{Helpers::packColor(255, 255, 255, 255), Helpers::packColor(16, 16, 16, 255), 35},
		TileInfo{Helpers::packColor(255, 128, 0, 255), Helpers::packColor(192, 64, 0, 255), 43}
	};

	std::vector<Tile> tiles;

	std::array<std::uint64_t, 256> lastVisibleTiles{};
	std::uint64_t lastVisibleTilesSize{};
};