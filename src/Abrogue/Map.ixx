export module Map;

export import QuadPool;

export class Map
{
public:
	struct Tile
	{
		QuadPool::Reference quadReference;
		bool exists{};
	};

	Map() = default;
	Map(size_t width, size_t height);

	void updateDraw(double deltaTime);

	auto const& getTiles() const { return tiles; }
	bool getTileExists(std::uint32_t x, std::uint32_t y) const;

private:
	size_t width{}, height{};
	std::vector<Tile> tiles;

	std::array<std::uint64_t, 256> lastVisibleTiles{};
	std::uint64_t lastVisibleTilesSize{};
};