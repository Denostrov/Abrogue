export module Map;

export import ObjectPools;

export class Map
{
public:
	Map()
	{
		tiles.resize((size_t)60 * 30, Tile{});
		for(size_t i = 0; i < 60; i++)
		{
			tiles[i].exists = true;
			tiles[i + (size_t)60 * 29].exists = true;
		}	

		for(size_t i = 0; i < 30; i++)
		{
			tiles[60 * i].exists = true;
			tiles[59 + 60 * i].exists = true;
		}

		for(size_t i = 0; i < tiles.size(); i++)
		{
			if(tiles[i].exists)
			{
				size_t row = i / 60;
				size_t column = i % 60;
				QuadPool::insert(QuadData{{7.0f / 9.0f + column / 60.0f, row / 30.0f}, {0.01666f, 0.03333f},
								 {QuadData::packColor(255, 255, 255, 255), QuadData::packColor(255, 255, 255, 255), QuadData::packColor(64, 64, 64, 64)}, 35});
			}
		}
	}

private:
	struct Tile
	{
		QuadPool::Reference quadReference;
		bool exists{};
	};

	std::vector<Tile> tiles;
};