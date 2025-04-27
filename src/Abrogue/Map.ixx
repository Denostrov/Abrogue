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

	auto const& getTiles() const { return tiles; }
	bool getTileExists(std::uint32_t x, std::uint32_t y) const;

	void onMousePressed(std::uint32_t x, std::uint32_t y);

private:
	size_t width{}, height{};
	std::vector<Tile> tiles;
};