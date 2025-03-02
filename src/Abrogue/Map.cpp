module Map;

Map::Map(size_t width, size_t height)
{
	tiles.reserve(width * height);
	for(size_t i = 0; i < width * height; i++)
		tiles.emplace_back();

	for(size_t i = 0; i < width; i++)
	{
		tiles[i].exists = true;
		tiles[i + width * (height - 1)].exists = true;
	}

	for(size_t i = 0; i < height; i++)
	{
		tiles[width * i].exists = true;
		tiles[width - 1 + width * i].exists = true;
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