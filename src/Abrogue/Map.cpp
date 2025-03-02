module Map;

Map::Map()
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
			tiles[i].quadReference = QuadPool::insert(QuadData{{0.0f, 0.0f}, 1.0f,
													{QuadData::packColor(255, 255, 255, 255), QuadData::packColor(32, 32, 32, 255)}, 35});
		}
		else
		{
			tiles[i].quadReference = QuadPool::insert(QuadData{{0.0f, 0.0f}, 1.0f,
													{QuadData::packColor(255, 255, 255, 255), QuadData::packColor(8, 8, 8, 255)}, 46});
		}
	}
}