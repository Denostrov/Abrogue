export module Map;

export import ObjectPools;

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

	auto const& getTiles() const { return tiles; }
	bool getTileExists(std::uint32_t x, std::uint32_t y) const;

private:
	size_t width{}, height{};
	std::vector<Tile> tiles;
};