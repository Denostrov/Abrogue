module Map;

import GameSystems;

void Map::init()
{
	tiles.reserve(Constants::mapWidth * Constants::mapHeight);
	for(size_t i = 0; i < Constants::mapWidth * Constants::mapHeight; i++)
		tiles.emplace_back();

	for(size_t i = 0; i < Constants::mapWidth; i++)
	{
		getTile(i, 0).type = TileType::eBedrock;
		getTile(i, Constants::mapHeight - 1).type = TileType::eBedrock;
	}

	for(size_t i = 0; i < Constants::mapHeight; i++)
	{
		getTile(0, i).type = TileType::eBedrock;
		getTile(Constants::mapWidth - 1, i).type = TileType::eBedrock;
	}

	auto middleX = Constants::mapWidth / 2;
	auto middleY = Constants::mapHeight / 2;
	getTile(middleX, middleY).type = TileType::eWall;
	getTile(middleX + 1, middleY).type = TileType::eWall;
	getTile(middleX, middleY + 1).type = TileType::eWall;
	getTile(middleX + 1, middleY + 1).type = TileType::eWall;

	getTile(middleX + 3, middleY).type = TileType::eWall;
	getTile(middleX + 4, middleY).type = TileType::eWall;
	getTile(middleX + 3, middleY + 1).type = TileType::eWall;
	getTile(middleX + 4, middleY + 1).type = TileType::eWall;

	getTile(middleX + 3, middleY + 3).type = TileType::eWall;
	getTile(middleX + 4, middleY + 3).type = TileType::eWall;
	getTile(middleX + 3, middleY + 4).type = TileType::eWall;
	getTile(middleX + 4, middleY + 4).type = TileType::eWall;

	getTile(middleX + 2, middleY + 4).type = TileType::eDoor;

	getTile(middleX, middleY + 3).type = TileType::eWall;
	getTile(middleX + 1, middleY + 3).type = TileType::eWall;
	getTile(middleX, middleY + 4).type = TileType::eWall;
	getTile(middleX + 1, middleY + 4).type = TileType::eWall;

	getTile(middleX + 6, middleY).type = TileType::eGrass;
	getTile(middleX + 7, middleY).type = TileType::eGrass;
	getTile(middleX + 6, middleY + 1).type = TileType::eGrass;
	getTile(middleX + 7, middleY + 1).type = TileType::eGrass;

	getTile(middleX + 6, middleY + 3).type = TileType::eBush;
	getTile(middleX + 7, middleY + 3).type = TileType::eBush;
	getTile(middleX + 6, middleY + 4).type = TileType::eBush;
	getTile(middleX + 7, middleY + 4).type = TileType::eBush;

	for(size_t i = 0; i < Constants::mapHeight; i++)
	{
		for(size_t j = 0; j < Constants::mapWidth; j++)
		{
			getTile(j, i).quadReference = quadPool.insert(QuadData{{(Constants::mapOffset + j + 0.5f) * QuadData::tileScale.x, (i + 0.5f) * QuadData::tileScale.y},
																 {Helpers::packColor(0, 0, 0, 255), Helpers::packColor(0,0,0,255)}, 32}, QuadPool::eMap);
		}
	}
}

void Map::updateDraw(double deltaTime)
{
	auto updateTileProperties = [](Tile const& tile, double brightness)
	{
		auto const& tileInfo = tilesInfo[(size_t)tile.type];
		auto [r, g, b, a] = Helpers::unpackColor(tileInfo.color);
		auto [bgR, bgG, bgB, bgA] = Helpers::unpackColor(tileInfo.backgroundColor);

		tile.quadReference.setGlyph(tileInfo.glyph);
		tile.quadReference.setColor(Helpers::packColor(r * brightness, g * brightness, b * brightness, a));
		tile.quadReference.setBackgroundColor(Helpers::packColor(bgR * brightness, bgG * brightness, bgB * brightness, bgA));
	};

	for(std::size_t i = 0; i < lastVisibleTilesSize; i++)
		updateTileProperties(tiles[lastVisibleTiles[i]], 0.5);

	auto [playerX, playerY] = player.getPosition();
	auto [playerVx, playerVy] = player.getVelocity();
	playerX += playerVx * deltaTime;
	playerY += playerVy * deltaTime;

	std::int64_t playerCellX = playerX;
	std::int64_t playerCellY = playerY;
	std::uint64_t playerCell = playerCellX + playerCellY * Constants::mapWidth;
	updateTileProperties(tiles[playerCell], 1.0);
	lastVisibleTiles[0] = playerCell;
	std::size_t visibleCount{1};

	//Octants are numbered clockwise from top left corner
	//Octant 1
	for(std::uint64_t i = 1; i <= 5; i++)
	{
		std::int64_t cellX = std::max((std::int64_t)(playerX - i), 0ll);
		std::int64_t cellY = playerY - i;
		if(cellY < 0)
			break;

		std::int64_t distanceY = cellY - playerCellY;
		for(std::int64_t j = cellX; j <= (int64_t)playerX; j++)
		{
			std::int64_t distanceX = j - playerCellX;
			double distance = std::sqrt(distanceX * distanceX + distanceY * distanceY);
			updateTileProperties(getTile(j, cellY), std::clamp(5.0 - distance, 0.0, 1.0));

			lastVisibleTiles[visibleCount] = j + cellY * Constants::mapWidth;
			visibleCount++;
		}
	}

	//Octant 2
	for(std::uint64_t i = 1; i <= 5; i++)
	{
		std::int64_t cellX = std::min((std::uint64_t)(playerX + i), Constants::mapWidth - 1);
		std::int64_t cellY = playerY - i;
		if(cellY < 0)
			break;

		for(std::int64_t j = cellX; j >= (int64_t)playerX; j--)
		{
			auto const& tile = getTile(j, cellY);
			auto const& tileInfo = tilesInfo[(size_t)tile.type];
			tile.quadReference.setGlyph(tileInfo.glyph);
			tile.quadReference.setColor(tileInfo.color);
			tile.quadReference.setBackgroundColor(tileInfo.backgroundColor);

			lastVisibleTiles[visibleCount] = j + cellY * Constants::mapWidth;
			visibleCount++;
		}
	}

	//Octant 3
	for(std::uint64_t i = 1; i <= 5; i++)
	{
		std::int64_t cellX = std::min((std::uint64_t)(playerX + i), Constants::mapWidth - 1);
		std::int64_t cellY = playerY - i;
		if(cellY < 0)
			break;

		for(std::int64_t j = cellY; j <= (int64_t)playerY; j++)
		{
			auto const& tile = getTile(cellX, j);
			auto const& tileInfo = tilesInfo[(size_t)tile.type];
			tile.quadReference.setGlyph(tileInfo.glyph);
			tile.quadReference.setColor(tileInfo.color);
			tile.quadReference.setBackgroundColor(tileInfo.backgroundColor);

			lastVisibleTiles[visibleCount] = cellX + j * Constants::mapWidth;
			visibleCount++;
		}
	}

	//Octant 4
	for(std::uint64_t i = 1; i <= 5; i++)
	{
		std::int64_t cellX = std::min((std::uint64_t)(playerX + i), Constants::mapWidth - 1);
		std::int64_t cellY = playerY + i;
		if(cellY >= Constants::mapHeight)
			break;

		for(std::int64_t j = cellY; j >= (int64_t)playerY; j--)
		{
			auto const& tile = getTile(cellX, j);
			auto const& tileInfo = tilesInfo[(size_t)tile.type];
			tile.quadReference.setGlyph(tileInfo.glyph);
			tile.quadReference.setColor(tileInfo.color);
			tile.quadReference.setBackgroundColor(tileInfo.backgroundColor);

			lastVisibleTiles[visibleCount] = cellX + j * Constants::mapWidth;
			visibleCount++;
		}
	}

	//Octant 5
	for(std::uint64_t i = 1; i <= 5; i++)
	{
		std::int64_t cellX = std::min((std::uint64_t)(playerX + i), Constants::mapWidth - 1);
		std::int64_t cellY = playerY + i;
		if(cellY >= Constants::mapHeight)
			break;

		for(std::int64_t j = cellX; j >= (int64_t)playerX; j--)
		{
			auto const& tile = getTile(j, cellY);
			auto const& tileInfo = tilesInfo[(size_t)tile.type];
			tile.quadReference.setGlyph(tileInfo.glyph);
			tile.quadReference.setColor(tileInfo.color);
			tile.quadReference.setBackgroundColor(tileInfo.backgroundColor);

			lastVisibleTiles[visibleCount] = j + cellY * Constants::mapWidth;
			visibleCount++;
		}
	}

	//Octant 6
	for(std::uint64_t i = 1; i <= 5; i++)
	{
		std::int64_t cellX = std::max((std::int64_t)(playerX - i), 0ll);
		std::int64_t cellY = playerY + i;
		if(cellY >= Constants::mapHeight)
			break;

		for(std::int64_t j = cellX; j <= (int64_t)playerX; j++)
		{
			auto const& tile = getTile(j, cellY);
			auto const& tileInfo = tilesInfo[(size_t)tile.type];
			tile.quadReference.setGlyph(tileInfo.glyph);
			tile.quadReference.setColor(tileInfo.color);
			tile.quadReference.setBackgroundColor(tileInfo.backgroundColor);

			lastVisibleTiles[visibleCount] = j + cellY * Constants::mapWidth;
			visibleCount++;
		}
	}

	//Octant 7
	for(std::uint64_t i = 1; i <= 5; i++)
	{
		std::int64_t cellX = std::max((std::int64_t)(playerX - i), 0ll);
		std::int64_t cellY = playerY + i;
		if(cellY >= Constants::mapHeight)
			break;

		for(std::int64_t j = cellY; j >= (int64_t)playerY; j--)
		{
			auto const& tile = getTile(cellX, j);
			auto const& tileInfo = tilesInfo[(size_t)tile.type];
			tile.quadReference.setGlyph(tileInfo.glyph);
			tile.quadReference.setColor(tileInfo.color);
			tile.quadReference.setBackgroundColor(tileInfo.backgroundColor);

			lastVisibleTiles[visibleCount] = cellX + j * Constants::mapWidth;
			visibleCount++;
		}
	}

	//Octant 8
	for(std::uint64_t i = 1; i <= 5; i++)
	{
		std::int64_t cellX = std::max((std::int64_t)(playerX - i), 0ll);
		std::int64_t cellY = playerY - i;
		if(cellY < 0)
			break;

		for(std::int64_t j = cellY; j <= (int64_t)playerY; j++)
		{
			auto const& tile = getTile(cellX, j);
			auto const& tileInfo = tilesInfo[(size_t)tile.type];
			tile.quadReference.setGlyph(tileInfo.glyph);
			tile.quadReference.setColor(tileInfo.color);
			tile.quadReference.setBackgroundColor(tileInfo.backgroundColor);

			lastVisibleTiles[visibleCount] = cellX + j * Constants::mapWidth;
			visibleCount++;
		}
	}

	lastVisibleTilesSize = visibleCount;
}

bool Map::getTileSolid(std::int32_t x, std::int32_t y) const
{
	logger.extraAssert(x >= 0 && x < Constants::mapWidth && y >= 0 && y < Constants::mapHeight, "Requested tile solidness out of bounds");

	auto const& type = getTile(x, y).type;
	return type == TileType::eBedrock || type == TileType::eWall;
}
