module Map;

import GameSystems;

Map::Map(size_t width, size_t height)
	:width(width), height(height)
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

	tiles[width / 2 + height / 2 * width].exists = true;
	tiles[width / 2 + 1 + height / 2 * width].exists = true;
	tiles[width / 2 + (height / 2 + 1) * width].exists = true;
	tiles[width / 2 + 1 + (height / 2 + 1) * width].exists = true;

	tiles[width / 2 + 3 + height / 2 * width].exists = true;
	tiles[width / 2 + 4 + height / 2 * width].exists = true;
	tiles[width / 2 + 3 + (height / 2 + 1) * width].exists = true;
	tiles[width / 2 + 4 + (height / 2 + 1) * width].exists = true;

	tiles[width / 2 + 3 + (height / 2 + 3) * width].exists = true;
	tiles[width / 2 + 4 + (height / 2 + 3) * width].exists = true;
	tiles[width / 2 + 3 + (height / 2 + 4) * width].exists = true;
	tiles[width / 2 + 4 + (height / 2 + 4) * width].exists = true;

	tiles[width / 2 + (height / 2 + 3) * width].exists = true;
	tiles[width / 2 + 1 + (height / 2 + 3) * width].exists = true;
	tiles[width / 2 + (height / 2 + 4) * width].exists = true;
	tiles[width / 2 + 1 + (height / 2 + 4) * width].exists = true;

	for(size_t i = 0; i < tiles.size(); i++)
	{
		if(tiles[i].exists)
		{
			tiles[i].quadReference = quadPool.insert(QuadData{{0.0f, 0.0f}, {Helpers::packColor(255, 255, 255, 255), Helpers::packColor(32, 32, 32, 255)}, 35},
													 QuadPool::eMap);
		}
		else
		{
			tiles[i].quadReference = quadPool.insert(QuadData{{0.0f, 0.0f}, {Helpers::packColor(255, 255, 255, 255), Helpers::packColor(8, 8, 8, 255)}, 46},
													 QuadPool::eMap);
		}
	}
}

bool Map::getTileExists(std::uint32_t x, std::uint32_t y) const
{
	if(x >= width || y >= height)
		return false;

	return tiles[x + y * width].exists;
}

void Map::onMousePressed(std::uint32_t x, std::uint32_t y)
{}
