export module Map;

export import ObjectPools;

export class Map
{
public:
	Map();

	auto const& getTiles() const { return tiles; }

private:
	struct Tile
	{
		QuadPool::Reference quadReference;
		bool exists{};
	};

	std::vector<Tile> tiles;
};