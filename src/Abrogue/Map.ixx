export module Map;

export import ObjectPools;

export class Map
{
public:
	Map() = default;
	Map(size_t width, size_t height);

	auto const& getTiles() const { return tiles; }

private:
	struct Tile
	{
		QuadPool::Reference quadReference;
		bool exists{};
	};

	std::vector<Tile> tiles;
};