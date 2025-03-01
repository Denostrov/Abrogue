export module Map;

export import ObjectPools;

export class Map
{
public:
	Map()
	{
		tiles.resize(Constants::mapWidth * Constants::mapHeight, Tile{});
		for(size_t i = 0; i < Constants::mapWidth; i++)
		{
			tiles[i].exists = true;
			tiles[i + Constants::mapWidth * (Constants::mapHeight - 1)].exists = true;
		}

		for(size_t i = 0; i < Constants::mapHeight; i++)
		{
			tiles[Constants::mapWidth * i].exists = true;
			tiles[Constants::mapWidth - 1 + Constants::mapWidth * i].exists = true;
		}

		for(size_t i = 0; i < tiles.size(); i++)
		{
			if(tiles[i].exists)
			{
				tiles[i].quadReference = QuadPool::insert(QuadData{{0.0f, 0.0f}, QuadData::tileScale,
														{QuadData::packColor(255, 255, 255, 255), QuadData::packColor(255, 255, 255, 255), QuadData::packColor(64, 64, 64, 64)}, 35});
			}
			else
			{
				tiles[i].quadReference = QuadPool::insert(QuadData{{0.0f, 0.0f}, QuadData::tileScale,
														{QuadData::packColor(255, 255, 255, 255), QuadData::packColor(255, 255, 255, 255), 0}, 46});
			}
		}
	}

	auto const& getTiles() const { return tiles; }

private:
	struct Tile
	{
		QuadPool::Reference quadReference;
		bool exists{};
	};

	std::vector<Tile> tiles;
};