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
			tiles[i].quadReference = quadPool.insert(QuadData{{0.0f, 0.0f}, {Helpers::packColor(96, 96, 96, 255), Helpers::packColor(4, 4, 4, 255)}, 250},
													 QuadPool::eMap);
		}
	}
}

void Map::updateDraw(double deltaTime)
{
	for(std::size_t i = 0; i < lastVisibleTilesSize; i++)
		tiles[lastVisibleTiles[i]].quadReference.setBackgroundColor(Helpers::packColor(4, 4, 4, 255));

	auto [playerX, playerY] = player.getPosition();
	auto [playerVx, playerVy] = player.getVelocity();
	playerX += playerVx * deltaTime;
	playerY += playerVy * deltaTime;

	std::uint64_t playerCell = playerX + (std::uint64_t)playerY * width;
	tiles[playerCell].quadReference.setBackgroundColor(Helpers::packColor(128, 128, 128, 255));
	lastVisibleTiles[0] = playerCell;
	std::size_t visibleCount{1};

	auto shadowcast = [this, playerX, playerY, &visibleCount]()
	{
		for(std::uint64_t i = 1; i <= 5; i++)
		{
			std::int64_t cellX = playerX - i;
			std::int64_t cellY = playerY - i;
			for(std::uint64_t j = cellX; j <= (uint64_t)playerX; j++)
			{
				tiles[j + cellY * width].quadReference.setBackgroundColor(Helpers::packColor(128, 128, 128, 255));
				lastVisibleTiles[visibleCount] = j + cellY * width;
				visibleCount++;
			}
		}
	};

	shadowcast();

	lastVisibleTilesSize = visibleCount;
}

bool Map::getTileExists(std::uint32_t x, std::uint32_t y) const
{
	if(x >= width || y >= height)
		return false;

	return tiles[x + y * width].exists;
}
