module Map;

import Player;
import Logger;
import Random;
import Enemy;

Map::Tile::Tile(std::int64_t x, std::int64_t y, Type type)
	:type(type)
{
	quad = quadPool.insert(QuadData{{Constants::mapOffset + x + 0.5f, y + 0.5f},
									{Color::pack(0, 0, 0, 255), Color::pack(0, 0, 0, 255)}, info[(size_t)type].glyph}, QuadPool::eMap);
}

void Map::init()
{
	for(std::int64_t y = 0; y < Constants::mapHeight; y++)
	{
		for(std::int64_t x = 0; x < Constants::mapWidth; x++)
		{
			getTile(x, y) = Tile(x, y, Tile::Type::eWall);
		}
	}

	generateLevel();

	updateVisibilityFunc = &Map::updateVisibility;
	(this->*updateVisibilityFunc)(0.0);
	enemyHandler.populateLevel();
}

void Map::update()
{
	(this->*updateVisibilityFunc)(0.0);

	for(auto& item : items)
		item.update();
}

void Map::updateDraw(double deltaTime)
{
	debugLines.clear();
	(this->*updateVisibilityFunc)(deltaTime);

	for(auto& item : items)
		item.updateDraw(deltaTime);
}

std::optional<Item> Map::pickupItem(std::int64_t x, std::int64_t y, bool onlyGold)
{
	for(std::size_t i = 0; i < items.size(); i++)
	{
		auto [itemX, itemY] = items[i].getPosition();
		if((std::int64_t)itemX != x || (std::int64_t)itemY != y)
			continue;

		if(items[i].getType() != Item::Type::eGold && onlyGold)
			continue;

		items[i].setVisible(false);
		std::optional<Item> result(std::move(items[i]));
		items.erase(items.begin() + i);
		return result;
	}

	return std::optional<Item>();
}

Map::Room const& Map::getRandomRoom() const
{
	return levelData.rooms[mapRandom.generate() % levelData.roomCount];
}

bool Map::getTileSolid(std::int64_t x, std::int64_t y) const
{
	logger.extraAssert(x >= 0 && x < Constants::mapWidth && y >= 0 && y < Constants::mapHeight, "Requested is tile solid out of bounds");

	return getTile(x, y).getIsSolid();
}

bool Map::getTileOpaque(std::int64_t x, std::int64_t y) const
{
	logger.extraAssert(x >= 0 && x < Constants::mapWidth && y >= 0 && y < Constants::mapHeight, "Requested is tile opaque out of bounds");

	return getTile(x, y).getIsOpaque();
}

bool Map::getTileFloor(std::int64_t x, std::int64_t y) const
{
	logger.extraAssert(x >= 0 && x < Constants::mapWidth && y >= 0 && y < Constants::mapHeight, "Requested is tile a floor out of bounds");

	return getTile(x, y).getIsFloor();
}

double Map::getTileBrightness(std::int64_t x, std::int64_t y) const
{
	logger.extraAssert(x >= 0 && x < Constants::mapWidth && y >= 0 && y < Constants::mapHeight, "Requested tile brightness out of bounds");

	return tileBrightnessMask[x + Constants::mapWidth * y];
}

void Map::generateLevel()
{
	for(std::int64_t x = 0; x < Constants::mapWidth; x++)
	{
		getTile(x, 0).setType(Tile::Type::eBedrock);
		levelData.tilesOccupiedMask[x] = true;

		getTile(x, Constants::mapHeight - 1).setType(Tile::Type::eBedrock);
		levelData.tilesOccupiedMask[x + (Constants::mapHeight - 1) * Constants::mapWidth] = true;
	}

	for(std::int64_t y = 0; y < Constants::mapHeight; y++)
	{
		getTile(0, y).setType(Tile::Type::eBedrock);
		levelData.tilesOccupiedMask[y * Constants::mapWidth] = true;

		getTile(Constants::mapWidth - 1, y).setType(Tile::Type::eBedrock);
		levelData.tilesOccupiedMask[Constants::mapWidth - 1 + y * Constants::mapWidth] = true;
	}

	auto tryPlacingRoom = [this](std::int64_t doorX, std::int64_t doorY, std::int64_t originX, std::int64_t originY, std::int64_t width, std::int64_t height, std::uint64_t originDirection)
	{
		for(auto y = originY; y < originY + height; y++)
		{
			for(auto x = originX; x < originX + width; x++)
			{
				if(x < 0 || x >= Constants::mapWidth || y < 0 || y >= Constants::mapHeight
				   || levelData.tilesOccupiedMask[x + y * Constants::mapWidth])
					return;
			}
		}

		for(auto y = originY; y < originY + height; y++)
		{
			for(auto x = originX; x < originX + width; x++)
				getTile(x, y).setType(Tile::Type::eFloor);
		}

		for(auto y = originY - 1; y < originY + height + 1; y++)
		{
			for(auto x = originX - 1; x < originX + width + 1; x++)
				levelData.tilesOccupiedMask[x + y * Constants::mapWidth] = true;
		}

		std::uint64_t value = std::random_device()() % 2;
		getTile(doorX, doorY).setType(value == 0 ? Tile::Type::eDoor : Tile::Type::eFloor);

		levelData.rooms[levelData.roomCount].originX = originX;
		levelData.rooms[levelData.roomCount].originY = originY;
		levelData.rooms[levelData.roomCount].width = width;
		levelData.rooms[levelData.roomCount].height = height;
		levelData.roomCount++;

		if(originDirection != 1)
		{
			for(auto y = originY; y < originY + height; y++)
			{
				int64_t x = originX - 1;
				if(x < 2)
					break;

				if(getTileFloor(x - 1, y) && getTileFloor(x + 1, y))
				{
					std::uint64_t value = std::random_device()() % 2;
					getTile(x, y).setType(value == 0 ? Tile::Type::eDoor : Tile::Type::eFloor);
					break;
				}
			}
		}

		if(originDirection != 3)
		{
			for(auto y = originY; y < originY + height; y++)
			{
				int64_t x = originX + width;
				if(x >= Constants::mapWidth - 2)
					break;

				if(getTileFloor(x - 1, y) && getTileFloor(x + 1, y))
				{
					std::uint64_t value = std::random_device()() % 2;
					getTile(x, y).setType(value == 0 ? Tile::Type::eDoor : Tile::Type::eFloor);
					break;
				}
			}
		}

		if(originDirection != 2)
		{
			for(auto x = originX; x < originX + width; x++)
			{
				int64_t y = originY - 1;
				if(y < 2)
					break;

				if(getTileFloor(x, y - 1) && getTileFloor(x, y + 1))
				{
					std::uint64_t value = std::random_device()() % 2;
					getTile(x, y).setType(value == 0 ? Tile::Type::eDoor : Tile::Type::eFloor);
					break;
				}
			}
		}

		if(originDirection != 0)
		{
			for(auto x = originX; x < originX + width; x++)
			{
				int64_t y = originY + height;
				if(y >= Constants::mapHeight - 2)
					break;

				if(getTileFloor(x, y - 1) && getTileFloor(x, y + 1))
				{
					std::uint64_t value = std::random_device()() % 2;
					getTile(x, y).setType(value == 0 ? Tile::Type::eDoor : Tile::Type::eFloor);
					break;
				}
			}
		}
	};

	auto& startingRoom = levelData.rooms[0];
	startingRoom.originX = 30;
	startingRoom.originY = 29;
	startingRoom.width = 20;
	startingRoom.height = 5;
	levelData.roomCount = 1;
	for(auto y = startingRoom.originY; y < startingRoom.originY + startingRoom.height; y++)
	{
		for(auto x = startingRoom.originX; x < startingRoom.originX + startingRoom.width; x++)
			getTile(x, y).setType(Tile::Type::eFloor);
	}
	for(auto y = startingRoom.originY - 1; y < startingRoom.originY + startingRoom.height + 1; y++)
	{
		for(auto x = startingRoom.originX - 1; x < startingRoom.originX + startingRoom.width + 1; x++)
			levelData.tilesOccupiedMask[x + y * Constants::mapWidth] = true;
	}
	getTile(40, 34).setType(Tile::Type::eExit);

	for(std::int64_t i = 0; i < 200; i++)
	{
	outerLoop:
		std::int64_t currentWidthSpread = std::llround(18 * (1.0 - i / 200.0)) + 1;
		std::int64_t currentHeightSpread = std::llround(8 * (1.0 - i / 200.0)) + 1;

		auto randomness = std::random_device();
		auto const& originRoom = levelData.rooms[randomness() % levelData.roomCount];

		std::int64_t newRoomWidth = 2ll + randomness() % currentWidthSpread;
		std::int64_t newRoomHeight = 2ll + randomness() % currentHeightSpread;

		std::uint64_t newDirection = randomness() % 4;
		if(newDirection == 0)
		{
			std::int64_t doorX = originRoom.originX + randomness() % originRoom.width;
			std::int64_t doorY = originRoom.originY - 1;

			std::int64_t newRoomOriginX = doorX - randomness() % newRoomWidth;
			std::int64_t newRoomOriginY = doorY - newRoomHeight;

			tryPlacingRoom(doorX, doorY, newRoomOriginX, newRoomOriginY, newRoomWidth, newRoomHeight, newDirection);
		}
		else if(newDirection == 1)
		{
			std::int64_t doorX = originRoom.originX + originRoom.width;
			std::int64_t doorY = originRoom.originY + randomness() % originRoom.height;

			std::int64_t newRoomOriginX = doorX + 1;
			std::int64_t newRoomOriginY = doorY - randomness() % newRoomHeight;

			tryPlacingRoom(doorX, doorY, newRoomOriginX, newRoomOriginY, newRoomWidth, newRoomHeight, newDirection);
		}
		else if(newDirection == 2)
		{
			std::int64_t doorX = originRoom.originX + randomness() % originRoom.width;
			std::int64_t doorY = originRoom.originY + originRoom.height;

			std::int64_t newRoomOriginX = doorX - randomness() % newRoomWidth;
			std::int64_t newRoomOriginY = doorY + 1;

			tryPlacingRoom(doorX, doorY, newRoomOriginX, newRoomOriginY, newRoomWidth, newRoomHeight, newDirection);
		}
		else if(newDirection == 3)
		{
			std::int64_t doorX = originRoom.originX - 1;
			std::int64_t doorY = originRoom.originY + randomness() % originRoom.height;

			std::int64_t newRoomOriginX = doorX - newRoomWidth;
			std::int64_t newRoomOriginY = doorY - randomness() % newRoomHeight;

			tryPlacingRoom(doorX, doorY, newRoomOriginX, newRoomOriginY, newRoomWidth, newRoomHeight, newDirection);
		}
	}

	for(std::int64_t i = 0; i < 20; i++)
	{
		auto const& room = getRandomRoom();
		std::int64_t spawnX = room.originX + mapRandom.generate() % room.width;
		std::int64_t spawnY = room.originY + mapRandom.generate() % room.height;

		std::int64_t itemTypeVal = mapRandom.generate() % 10;
		Item::Type itemType = itemTypeVal == 0 ? Item::Type::eFood : Item::Type::eGold;
		items.emplace_back(itemType, spawnX + 0.5, spawnY + 0.5);
	}

	auto const& room = getRandomRoom();
	std::int64_t spawnX = room.originX + mapRandom.generate() % room.width;
	std::int64_t spawnY = room.originY + mapRandom.generate() % room.height;
	items.emplace_back(Item::Type::eAmulet, spawnX + 0.5, spawnY + 0.5);
}

void Map::updateVisibility(double deltaTime)
{
	for(std::size_t i = 0; i < lastVisibleTilesSize; i++)
	{
		tiles[lastVisibleTiles[i]].updateDraw(0.25);
		tileBrightnessMask[lastVisibleTiles[i]] = 0.0;
	}

	auto [playerX, playerY] = player.getPosition();
	auto [playerVx, playerVy] = player.getVelocity();
	playerX += playerVx * deltaTime;
	playerY += playerVy * deltaTime;

	std::int64_t playerCellX = playerX;
	std::int64_t playerCellY = playerY;
	std::uint64_t playerCell = playerCellX + playerCellY * Constants::mapWidth;
	tiles[playerCell].updateDraw(1.0);
	lastVisibleTiles[0] = playerCell;
	lastVisibleTilesSize = 1;
	tileBrightnessMask[playerCell] = 1.0;

	double visionRange = 40.0;
	std::int64_t lookupRange = std::ceil(visionRange);

	auto updateVisibleTile = [this, visionRange](std::int64_t x, std::int64_t y, double distanceX, double distanceY)
	{
		double distance = std::sqrt(distanceX * distanceX / 4.0 + distanceY * distanceY);
		if(distance > visionRange)
			return;

		double lightStrength = std::clamp((visionRange - distance) / 2.0 + 0.5, 0.5, 1.0);
		getTile(x, y).updateDraw(lightStrength);

		lastVisibleTiles[lastVisibleTilesSize] = x + y * Constants::mapWidth;
		lastVisibleTilesSize++;

		tileBrightnessMask[x + y * Constants::mapWidth] = lightStrength;
	};

	//Octants are numbered clockwise from top left corner
	//Octant 1
	auto calculateOctant1 = [this, lookupRange, playerX, playerY, &updateVisibleTile](this auto self, std::int64_t startOffset, double startSlope, double endSlope) -> void
	{
		for(auto i = startOffset; i <= lookupRange; i++)
		{
			std::int64_t startCellX = std::max((std::int64_t)(playerX - i * startSlope), 0ll);
			std::int64_t endCellX = std::max((std::int64_t)(playerX - i * endSlope), 0ll);

			std::int64_t cellY = playerY - i;
			if(cellY < 0)
				break;

			double distanceY = cellY - playerY;
			for(auto j = startCellX; j <= endCellX; j++)
			{
				if(getTileOpaque(j, cellY))
				{
					double newEndSlope = (j - playerX) / (cellY + 1.0 - playerY);
					if(startSlope > newEndSlope)
						self(i + 1, startSlope, newEndSlope);

					do
					{
						double distanceX = j - playerX;
						updateVisibleTile(j, cellY, distanceX, distanceY);
						if(j == endCellX)
							return;

						j++;
					} while(j <= endCellX && getTileOpaque(j, cellY));

					startSlope = (j - playerX) / (cellY - playerY);
					if(startSlope <= endSlope)
					{
						double distanceX = j - playerX;
						updateVisibleTile(j, cellY, distanceX, distanceY);
						return;
					}
				}

				double distanceX = j - playerX;
				updateVisibleTile(j, cellY, distanceX, distanceY);
			}
		}
	};
	calculateOctant1(1, 1.0, 0.0);

	//Octant 2
	auto calculateOctant2 = [this, lookupRange, playerX, playerY, &updateVisibleTile](this auto self, std::int64_t startOffset, double startSlope, double endSlope) -> void
	{
		for(auto i = startOffset; i <= lookupRange; i++)
		{
			std::int64_t startCellX = std::min((std::int64_t)(playerX - i * startSlope), Constants::mapWidth - 1);
			std::int64_t endCellX = std::min((std::int64_t)(playerX - i * endSlope), Constants::mapWidth - 1);

			std::int64_t cellY = playerY - i;
			if(cellY < 0)
				break;

			double distanceY = cellY - playerY;
			for(auto j = startCellX; j >= endCellX; j--)
			{
				if(getTileOpaque(j, cellY))
				{
					double newEndSlope = (j + 1 - playerX) / (cellY + 1 - playerY);
					if(startSlope < newEndSlope)
						self(i + 1, startSlope, newEndSlope);

					do
					{
						double distanceX = j - playerX;
						updateVisibleTile(j, cellY, distanceX, distanceY);
						if(j == endCellX)
							return;

						j--;
					} while(j >= endCellX && getTileOpaque(j, cellY));

					startSlope = (j + 1 - playerX) / (cellY - playerY);
					if(startSlope >= endSlope)
					{
						double distanceX = j - playerX;
						updateVisibleTile(j, cellY, distanceX, distanceY);
						return;
					}
				}

				double distanceX = j - playerX;
				updateVisibleTile(j, cellY, distanceX, distanceY);
			}
		}
	};
	calculateOctant2(1, -1.0, 0.0);

	//Octant 3
	auto calculateOctant3 = [this, lookupRange, playerX, playerY, &updateVisibleTile](this auto self, std::int64_t startOffset, double startSlope, double endSlope) -> void
	{
		for(auto i = startOffset; i <= lookupRange * 2; i++)
		{
			std::int64_t cellX = playerX + i;
			if(cellX >= Constants::mapWidth)
				break;

			std::int64_t startCellY = std::max((std::int64_t)(playerY + i * startSlope), 0ll);
			std::int64_t endCellY = std::max((std::int64_t)(playerY + i * endSlope), 0ll);

			double distanceX = cellX - playerX;
			for(auto j = startCellY; j <= endCellY; j++)
			{
				if(getTileOpaque(cellX, j))
				{
					double newEndSlope = (j - playerY) / (cellX - playerX);
					if(startSlope < newEndSlope)
						self(i + 1, startSlope, newEndSlope);

					do
					{
						double distanceY = j - playerY;
						updateVisibleTile(cellX, j, distanceX, distanceY);
						if(j == endCellY)
							return;

						j++;
					} while(j <= endCellY && getTileOpaque(cellX, j));

					startSlope = (j - playerY) / (cellX + 1 - playerX);
					if(startSlope >= endSlope)
					{
						double distanceY = j - playerY;
						updateVisibleTile(cellX, j, distanceX, distanceY);
						return;
					}
				}

				double distanceY = j - playerY;
				updateVisibleTile(cellX, j, distanceX, distanceY);
			}
		}
	};
	calculateOctant3(1, -1.0, 0.0);

	//Octant 4
	auto calculateOctant4 = [this, lookupRange, playerX, playerY, &updateVisibleTile](this auto self, std::int64_t startOffset, double startSlope, double endSlope) -> void
	{
		for(auto i = startOffset; i <= lookupRange * 2; i++)
		{
			std::int64_t cellX = playerX + i;
			if(cellX >= Constants::mapWidth)
				break;

			std::int64_t startCellY = std::min((std::int64_t)(playerY + i * startSlope), Constants::mapHeight - 1);
			std::int64_t endCellY = std::min((std::int64_t)(playerY + i * endSlope), Constants::mapHeight - 1);

			double distanceX = cellX - playerX;
			for(auto j = startCellY; j >= endCellY; j--)
			{
				if(getTileOpaque(cellX, j))
				{
					double newEndSlope = (j + 1.0 - playerY) / (cellX - playerX);
					if(startSlope > newEndSlope)
						self(i + 1, startSlope, newEndSlope);

					do
					{
						double distanceY = j - playerY;
						updateVisibleTile(cellX, j, distanceX, distanceY);
						if(j == endCellY)
							return;

						j--;
					} while(j >= endCellY && getTileOpaque(cellX, j));

					startSlope = (j + 1.0 - playerY) / (cellX + 1.0 - playerX);
					if(startSlope <= endSlope)
					{
						double distanceY = j - playerY;
						updateVisibleTile(cellX, j, distanceX, distanceY);
						return;
					}
				}

				double distanceY = j - playerY;
				updateVisibleTile(cellX, j, distanceX, distanceY);
			}
		}
	};
	calculateOctant4(1, 1.0, 0.0);

	//Octant 5
	auto calculateOctant5 = [this, lookupRange, playerX, playerY, &updateVisibleTile](this auto self, std::int64_t startOffset, double startSlope, double endSlope) -> void
	{
		for(auto i = startOffset; i <= lookupRange; i++)
		{
			std::int64_t startCellX = std::min((std::int64_t)(playerX + i * startSlope), Constants::mapWidth - 1);
			std::int64_t endCellX = std::min((std::int64_t)(playerX + i * endSlope), Constants::mapWidth - 1);

			std::int64_t cellY = playerY + i;
			if(cellY >= Constants::mapHeight)
				break;

			double distanceY = cellY - playerY;
			for(auto j = startCellX; j >= endCellX; j--)
			{
				if(getTileOpaque(j, cellY))
				{
					double newEndSlope = (j + 1 - playerX) / (cellY - playerY);
					if(startSlope > newEndSlope)
						self(i + 1, startSlope, newEndSlope);

					do
					{
						double distanceX = j - playerX;
						updateVisibleTile(j, cellY, distanceX, distanceY);
						if(j == endCellX)
							return;

						j--;
					} while(j >= endCellX && getTileOpaque(j, cellY));

					startSlope = (j + 1 - playerX) / (cellY + 1 - playerY);
					if(startSlope <= endSlope)
					{
						double distanceX = j - playerX;
						updateVisibleTile(j, cellY, distanceX, distanceY);
						return;
					}
				}

				double distanceX = j - playerX;
				updateVisibleTile(j, cellY, distanceX, distanceY);
			}
		}
	};
	calculateOctant5(1, 1.0, 0.0);

	//Octant 6
	auto calculateOctant6 = [this, lookupRange, playerX, playerY, &updateVisibleTile](this auto self, std::int64_t startOffset, double startSlope, double endSlope) -> void
	{
		for(auto i = startOffset; i <= lookupRange; i++)
		{
			std::int64_t startCellX = std::max((std::int64_t)(playerX + i * startSlope), 0ll);
			std::int64_t endCellX = std::max((std::int64_t)(playerX + i * endSlope), 0ll);

			std::int64_t cellY = playerY + i;
			if(cellY >= Constants::mapHeight)
				break;

			double distanceY = cellY - playerY;
			for(auto j = startCellX; j <= endCellX; j++)
			{
				if(getTileOpaque(j, cellY))
				{
					double newEndSlope = (j - playerX) / (cellY - playerY);
					if(startSlope < newEndSlope)
						self(i + 1, startSlope, newEndSlope);

					do
					{
						double distanceX = j - playerX;
						updateVisibleTile(j, cellY, distanceX, distanceY);
						if(j == endCellX)
							return;

						j++;
					} while(j <= endCellX && getTileOpaque(j, cellY));

					startSlope = (j - playerX) / (cellY + 1.0 - playerY);
					if(startSlope >= endSlope)
					{
						double distanceX = j - playerX;
						updateVisibleTile(j, cellY, distanceX, distanceY);
						return;
					}
				}

				double distanceX = j - playerX;
				updateVisibleTile(j, cellY, distanceX, distanceY);
			}
		}
	};
	calculateOctant6(1, -1.0, 0.0);

	//Octant 7
	auto calculateOctant7 = [this, lookupRange, playerX, playerY, &updateVisibleTile](this auto self, std::int64_t startOffset, double startSlope, double endSlope) -> void
	{
		for(auto i = startOffset; i <= lookupRange * 2; i++)
		{
			std::int64_t cellX = playerX - i;
			if(cellX < 0)
				break;

			std::int64_t startCellY = std::min((std::int64_t)(playerY - i * startSlope), Constants::mapHeight - 1);
			std::int64_t endCellY = std::min((std::int64_t)(playerY - i * endSlope), Constants::mapHeight - 1);

			double distanceX = cellX - playerX;
			for(auto j = startCellY; j >= endCellY; j--)
			{
				if(getTileOpaque(cellX, j))
				{
					double newEndSlope = (j + 1.0 - playerY) / (cellX + 1.0 - playerX);
					if(startSlope < newEndSlope)
						self(i + 1, startSlope, newEndSlope);

					do
					{
						double distanceY = j - playerY;
						updateVisibleTile(cellX, j, distanceX, distanceY);
						if(j == endCellY)
							return;

						j--;
					} while(j >= endCellY && getTileOpaque(cellX, j));

					startSlope = (j + 1.0 - playerY) / (cellX - playerX);
					if(startSlope >= endSlope)
					{
						double distanceY = j - playerY;
						updateVisibleTile(cellX, j, distanceX, distanceY);
						return;
					}
				}

				double distanceY = j - playerY;
				updateVisibleTile(cellX, j, distanceX, distanceY);
			}
		}
	};
	calculateOctant7(1, -1.0, 0.0);

	//Octant 8
	auto calculateOctant8 = [this, lookupRange, playerX, playerY, &updateVisibleTile](this auto self, std::int64_t startOffset, double startSlope, double endSlope) -> void
	{
		for(auto i = startOffset; i <= lookupRange * 2; i++)
		{
			std::int64_t cellX = playerX - i;
			if(cellX < 0)
				break;

			std::int64_t startCellY = std::max((std::int64_t)(playerY - i * startSlope), 0ll);
			std::int64_t endCellY = std::max((std::int64_t)(playerY - i * endSlope), 0ll);

			double distanceX = cellX - playerX;
			for(auto j = startCellY; j <= endCellY; j++)
			{
				if(getTileOpaque(cellX, j))
				{
					double newEndSlope = (j - playerY) / (cellX + 1.0 - playerX);
					if(startSlope > newEndSlope)
						self(i + 1, startSlope, newEndSlope);

					do
					{
						double distanceY = j - playerY;
						updateVisibleTile(cellX, j, distanceX, distanceY);
						if(j == endCellY)
							return;

						j++;
					} while(j <= endCellY && getTileOpaque(cellX, j));

					startSlope = (j - playerY) / (cellX - playerX);
					if(startSlope <= endSlope)
					{
						double distanceY = j - playerY;
						updateVisibleTile(cellX, j, distanceX, distanceY);
						return;
					}
				}

				double distanceY = j - playerY;
				updateVisibleTile(cellX, j, distanceX, distanceY);
			}
		}
	};
	calculateOctant8(1, 1.0, 0.0);
}

void Map::updateVisibilityDebug(double deltaTime)
{
	for(std::size_t i = 0; i < lastVisibleTilesSize; i++)
	{
		tiles[lastVisibleTiles[i]].updateDraw(0.25);
		tileBrightnessMask[lastVisibleTiles[i]] = 0.0;
	}
	lastVisibleTilesSize = 0;

	auto [playerX, playerY] = player.getPosition();
	auto [playerVx, playerVy] = player.getVelocity();
	playerX += playerVx * deltaTime;
	playerY += playerVy * deltaTime;

	double visionRange = 40.0;
	std::int64_t lookupRange = std::ceil(visionRange);

	auto updateVisibleTile = [this, visionRange](std::int64_t x, std::int64_t y, double distanceX, double distanceY)
	{
		double distance = std::sqrt(distanceX * distanceX / 4.0 + distanceY * distanceY);
		if(distance > visionRange)
			return;

		double lightStrength = std::clamp((visionRange - distance) / 2.0 + 0.5, 0.5, 1.0);
		getTile(x, y).updateDraw(lightStrength);

		lastVisibleTiles[lastVisibleTilesSize] = x + y * Constants::mapWidth;
		lastVisibleTilesSize++;

		tileBrightnessMask[x + y * Constants::mapWidth] = lightStrength;
	};

	//Get initial left slope
	double startSlope{};
	std::int64_t startCellX{(std::int64_t)playerX}, startCellY{(std::int64_t)playerY};
	while(true)
	{
		double distanceX = startCellX + 0.5 - playerX;
		double distanceY = startCellY + 0.5 - playerY;
		updateVisibleTile(startCellX, startCellY, distanceX, distanceY);

		if(getTileOpaque(startCellX, startCellY))
		{
			startCellY--;
			startSlope = (startCellY + 1 - playerY) / (startCellX + 1 - playerX);
			debugLines.emplace_back(Constants::mapOffset + playerX, playerY, Constants::mapOffset + startCellX + 1, playerY);
			break;
		}
		startCellX--;
	}

	//Get initial right slope
	double endSlope{};
	std::int64_t endCellX{(std::int64_t)playerX}, endCellY{(std::int64_t)playerY};
	while(true)
	{
		double distanceX = endCellX + 0.5 - playerX;
		double distanceY = endCellY + 0.5 - playerY;
		updateVisibleTile(endCellX, endCellY, distanceX, distanceY);

		if(getTileOpaque(endCellX, endCellY))
		{
			endCellY--;
			endSlope = (endCellY + 1 - playerY) / (endCellX - playerX);
			debugLines.emplace_back(Constants::mapOffset + playerX, playerY, Constants::mapOffset + endCellX, playerY);
			break;
		}
		endCellX++;
	}

	auto calculateVisibilitySector = [this, playerX, playerY, &updateVisibleTile](this auto&& self,
																				  double startSlope, double startEnterX, std::int64_t startCellX, std::int64_t startCellY,
																				  double endSlope, double endEnterX, std::int64_t endCellX, std::int64_t endCellY)
	{
		while(true)
		{
			std::int64_t currentStartCellX{};
			if(getTileOpaque(startCellX, startCellY))
			{
				debugLines.emplace_back(Constants::mapOffset + playerX, playerY, Constants::mapOffset + startEnterX, startCellY + 1);

				do
				{
					double distanceX = startCellX + 0.5 - playerX;
					double distanceY = startCellY + 0.5 - playerY;
					updateVisibleTile(startCellX, startCellY, distanceX, distanceY);

					if(startCellX == endCellX)
						return;
					startCellX++;

				} while(getTileOpaque(startCellX, startCellY));

				updateVisibleTile(startCellX, startCellY, startCellX + 0.5 - playerX, startCellY + 0.5 - playerY);
				currentStartCellX = startCellX + 1;

				startSlope = (startCellY - playerY) / (startCellX - playerX);
				startEnterX = startCellX;
				startCellX--;
				startCellY--;
			}
			else
			{
				currentStartCellX = startCellX + 1;
				updateVisibleTile(startCellX, startCellY, startCellX + 0.5 - playerX, startCellY + 0.5 - playerY);

				auto startCoefficientX = std::sqrt(1.0 + startSlope * startSlope);
				auto startCoefficientY = std::sqrt(1.0 + 1.0 / (startSlope * startSlope));
				while(true)
				{
					double leftExtentX{startEnterX - startCellX};
					double distanceX{startCoefficientX * leftExtentX}, distanceY{startCoefficientY * 1};
					if(distanceX <= distanceY)
					{
						startCellX--;
						updateVisibleTile(startCellX, startCellY, startCellX + 0.5 - playerX, startCellY + 0.5 - playerY);
						if(getTileOpaque(startCellX, startCellY))
						{
							debugLines.emplace_back(Constants::mapOffset + playerX, playerY, Constants::mapOffset + startCellX + 1, startCellY + 1 - std::sqrt(distanceX * distanceX - leftExtentX * leftExtentX));
							startCellY--;
							startSlope = (startCellY + 1 - playerY) / (startCellX + 1 - playerX);
							startEnterX = startCellX + 1;
							break;
						}
					}
					else
					{
						startEnterX = currentStartCellX + std::sqrt(distanceY * distanceY - 1);
						startCellY--;
						break;
					}
				}
			}

			std::int64_t currentEndCellX{};
			if(getTileOpaque(endCellX, endCellY))
			{
				debugLines.emplace_back(Constants::mapOffset + playerX, playerY, Constants::mapOffset + endEnterX, endCellY + 1);

				do
				{
					double distanceX = endCellX + 0.5 - playerX;
					double distanceY = endCellY + 0.5 - playerY;
					updateVisibleTile(endCellX, endCellY, distanceX, distanceY);

					if(endCellX == startCellX)
						return;
					endCellX--;

				} while(getTileOpaque(endCellX, endCellY));

				updateVisibleTile(endCellX, endCellY, endCellX + 0.5 - playerX, endCellY + 0.5 - playerY);
				currentEndCellX = endCellX - 1;

				endSlope = (endCellY - playerY) / (endCellX + 1 - playerX);
				endEnterX = endCellX + 1;
				endCellX++;
				endCellY--;
			}
			else
			{
				currentEndCellX = endCellX - 1;
				updateVisibleTile(endCellX, endCellY, endCellX + 0.5 - playerX, endCellY + 0.5 - playerY);

				auto endCoefficientX = std::sqrt(1.0 + endSlope * endSlope);
				auto endCoefficientY = std::sqrt(1.0 + 1.0 / (endSlope * endSlope));
				while(true)
				{
					double rightExtentX{endCellX + 1 - endEnterX};
					double distanceX{endCoefficientX * rightExtentX}, distanceY{endCoefficientY * 1};
					if(distanceX <= distanceY)
					{
						endCellX++;
						updateVisibleTile(endCellX, endCellY, endCellX + 0.5 - playerX, endCellY + 0.5 - playerY);
						if(getTileOpaque(endCellX, endCellY))
						{
							debugLines.emplace_back(Constants::mapOffset + playerX, playerY, Constants::mapOffset + endCellX, endCellY + 1 - std::sqrt(distanceX * distanceX - rightExtentX * rightExtentX));
							endCellY--;
							endSlope = (endCellY + 1 - playerY) / (endCellX - playerX);
							endEnterX = endCellX;
							break;
						}
					}
					else
					{
						endEnterX = currentEndCellX + std::sqrt(distanceY * distanceY - 1);
						endCellY--;
						break;
					}
				}
			}

			std::int64_t currentCellY{startCellY + 1};
			for(auto i = currentStartCellX; i <= currentEndCellX; i++)
			{
				if(getTileOpaque(i, currentCellY))
				{
					double newEndSlope = (currentCellY + 1 - playerY) / (i - playerX);
					if(newEndSlope > startSlope)
					{
						if(newEndSlope > 0.0)
						{
							std::int64_t newEndCellX = i - 1;
							std::int64_t newEndEnterX = i;

							auto endCoefficientX = std::sqrt(1.0 + newEndSlope * newEndSlope);
							auto endCoefficientY = std::sqrt(1.0 + 1.0 / (newEndSlope * newEndSlope));
							while(true)
							{
								double leftExtentX{endCellX - endEnterX};
								double distanceX{endCoefficientX * leftExtentX}, distanceY{endCoefficientY * 1};
								if(distanceX <= distanceY)
								{
									newEndCellX--;
									if(newEndCellX == startCellX)
										break;
								}
								else
								{
									newEndEnterX = newEndCellX - std::sqrt(distanceY * distanceY - 1);
									self(startSlope, startEnterX, startCellX, startCellY, newEndSlope, newEndEnterX, newEndCellX, startCellY);
									break;
								}
							}
						}
						else
							self(startSlope, startEnterX, startCellX, startCellY, newEndSlope, i, i - 1, startCellY);
					}

					do
					{
						updateVisibleTile(i, currentCellY, i + 0.5 - playerX, currentCellY + 0.5 - playerY);

						if(i == endCellX)
							return;
						i++;

					} while(getTileOpaque(i, currentCellY));

					startSlope = (currentCellY - playerY) / (i - playerX);
					startEnterX = i;
					startCellX = i - 1;
				}

				updateVisibleTile(i, currentCellY, i + 0.5 - playerX, currentCellY + 0.5 - playerY);
			}
		}
	};
	calculateVisibilitySector(startSlope, startCellX + 1.0, startCellX, startCellY, endSlope, endCellX, endCellX, endCellY);

	//auto calculateTopHalf = [](double startX, double startY)
	//{
	//	double startSlopePosX{}, startSlopePosY{};
	//	double startSlope{-0.01};

	//	auto startCoefficientX = std::sqrt(1.0 + startSlope * startSlope);
	//	auto startCoefficientY = std::sqrt(1.0 + 1.0 / (startSlope * startSlope));

	//	std::int64_t currentStartTileX{startX}, currentStartTileY{startY};

	//	double startInitialX{std::abs(startX - currentStartTileX) * startCoefficientX};
	//	double startInitialY{std::abs(startY - currentStartTileY) * startCoefficientY};

	//	std::int64_t startCellX{}, startCellY{};
	//	std::int64_t startStepsX{}, startStepsY{};
	//	while(true)
	//	{
	//		double distanceX{startInitialX + startCoefficientX * startStepsX}, distanceY{startInitialY + startCoefficientY * startStepsY};
	//		if(distanceX <= distanceY)
	//		{
	//			startStepsX++;
	//			currentStartTileX--;
	//			if(getTileOpaque(currentStartTileX, currentStartTileY))
	//			{
	//				startCellX = currentStartTileX;
	//				startCellY = currentStartTileY;
	//				break;
	//			}
	//		}
	//		else
	//		{
	//			startCellX = currentStartTileX;
	//			startCellY = currentStartTileY;
	//			break;
	//		}
	//	}

	//	double endSlopePosX{}, endSlopePosY{};
	//	double endSlope{0.01};

	//	auto endCoefficientX = std::sqrt(1.0 + endSlope * endSlope);
	//	auto endCoefficientY = std::sqrt(1.0 + 1.0 / (endSlope * endSlope));

	//	std::int64_t currentEndTileX{startX}, currentEndTileY{startY};

	//	double endInitialX{std::abs(1.0 - (startX - currentEndTileX)) * endCoefficientX};
	//	double endInitialY{std::abs(startY - currentStartTileY) * endCoefficientY};

	//	std::int64_t endCellX{}, endCellY{};
	//	std::int64_t endStepsX{}, endStepsY{};
	//	while(true)
	//	{
	//		double distanceX{endInitialX + endCoefficientX * endStepsX}, distanceY{endInitialY + endCoefficientY * endStepsY};
	//		if(distanceX <= distanceY)
	//		{
	//			endStepsX++;
	//			currentEndTileX++;
	//			if(getTileOpaque(currentEndTileX, currentEndTileY))
	//			{
	//				endCellX = currentEndTileX;
	//				endCellY = currentEndTileY;
	//				break;
	//			}
	//		}
	//		else
	//		{
	//			endCellX = currentEndTileX;
	//			endCellY = currentEndTileY;
	//			break;
	//		}
	//	}
	//};

	////Octants are numbered clockwise from top left corner
	////Octant 1
	//auto calculateOctant1 = [this, lookupRange, playerX, playerY, &updateVisibleTile](this auto self, std::int64_t startOffset, double startSlope, double endSlope) -> void
	//{


	//	for(auto i = startOffset; i <= lookupRange; i++)
	//	{
	//		std::int64_t startCellX = std::max((std::int64_t)(playerX - i * startSlope), 0ll);
	//		std::int64_t endCellX = std::max((std::int64_t)(playerX - i * endSlope), 0ll);

	//		std::int64_t cellY = playerY - i;
	//		if(cellY < 0)
	//			break;

	//		double distanceY = cellY - playerY;
	//		for(auto j = startCellX; j <= endCellX; j++)
	//		{
	//			if(getTileOpaque(j, cellY))
	//			{
	//				double newEndSlope = (j - playerX) / (cellY + 1.0 - playerY);
	//				if(startSlope > newEndSlope)
	//					self(i + 1, startSlope, newEndSlope);

	//				do
	//				{
	//					double distanceX = j - playerX;
	//					updateVisibleTile(j, cellY, distanceX, distanceY);
	//					if(j == endCellX)
	//					{
	//						debugLines.emplace_back(Constants::mapOffset + playerX, playerY, Constants::mapOffset + playerX - i * startSlope, playerY - i);
	//						debugLines.emplace_back(Constants::mapOffset + playerX, playerY, Constants::mapOffset + playerX - i * endSlope, playerY - i);
	//						return;
	//					}

	//					j++;
	//				} while(j <= endCellX && getTileOpaque(j, cellY));

	//				debugLines.emplace_back(Constants::mapOffset + playerX, playerY, Constants::mapOffset + playerX - i * startSlope, playerY - i);
	//				startSlope = (j - playerX) / (cellY - playerY);
	//				if(startSlope <= endSlope)
	//				{
	//					double distanceX = j - playerX;
	//					updateVisibleTile(j, cellY, distanceX, distanceY);
	//					return;
	//				}
	//			}

	//			double distanceX = j - playerX;
	//			updateVisibleTile(j, cellY, distanceX, distanceY);
	//		}
	//	}
	//};
	//calculateOctant1(1, 1.0, 0.0);

	////Octant 2
	//auto calculateOctant2 = [this, lookupRange, playerX, playerY, &updateVisibleTile](this auto self, std::int64_t startOffset, double startSlope, double endSlope) -> void
	//{
	//	for(auto i = startOffset; i <= lookupRange; i++)
	//	{
	//		std::int64_t startCellX = std::min((std::int64_t)(playerX - i * startSlope), Constants::mapWidth - 1);
	//		std::int64_t endCellX = std::min((std::int64_t)(playerX - i * endSlope), Constants::mapWidth - 1);

	//		std::int64_t cellY = playerY - i;
	//		if(cellY < 0)
	//			break;

	//		double distanceY = cellY - playerY;
	//		for(auto j = startCellX; j >= endCellX; j--)
	//		{
	//			if(getTileOpaque(j, cellY))
	//			{
	//				double newEndSlope = (j + 1 - playerX) / (cellY + 1 - playerY);
	//				if(startSlope < newEndSlope)
	//					self(i + 1, startSlope, newEndSlope);

	//				do
	//				{
	//					double distanceX = j - playerX;
	//					updateVisibleTile(j, cellY, distanceX, distanceY);
	//					if(j == endCellX)
	//					{
	//						debugLines.emplace_back(Constants::mapOffset + playerX, playerY, Constants::mapOffset + playerX - i * startSlope, playerY - i);
	//						debugLines.emplace_back(Constants::mapOffset + playerX, playerY, Constants::mapOffset + playerX - i * endSlope, playerY - i);
	//						return;
	//					}

	//					j--;
	//				} while(j >= endCellX && getTileOpaque(j, cellY));

	//				debugLines.emplace_back(Constants::mapOffset + playerX, playerY, Constants::mapOffset + playerX - i * startSlope, playerY - i);
	//				startSlope = (j + 1 - playerX) / (cellY - playerY);
	//				if(startSlope >= endSlope)
	//				{
	//					double distanceX = j - playerX;
	//					updateVisibleTile(j, cellY, distanceX, distanceY);
	//					return;
	//				}
	//			}

	//			double distanceX = j - playerX;
	//			updateVisibleTile(j, cellY, distanceX, distanceY);
	//		}
	//	}
	//};
	//calculateOctant2(1, -1.0, 0.0);

	////Octant 3
	//auto calculateOctant3 = [this, lookupRange, playerX, playerY, &updateVisibleTile](this auto self, std::int64_t startOffset, double startSlope, double endSlope) -> void
	//{
	//	for(auto i = startOffset; i <= lookupRange * 2; i++)
	//	{
	//		std::int64_t cellX = playerX + i;
	//		if(cellX >= Constants::mapWidth)
	//			break;

	//		std::int64_t startCellY = std::max((std::int64_t)(playerY + i * startSlope), 0ll);
	//		std::int64_t endCellY = std::max((std::int64_t)(playerY + i * endSlope), 0ll);

	//		double distanceX = cellX - playerX;
	//		for(auto j = startCellY; j <= endCellY; j++)
	//		{
	//			if(getTileOpaque(cellX, j))
	//			{
	//				double newEndSlope = (j - playerY) / (cellX - playerX);
	//				if(startSlope < newEndSlope)
	//					self(i + 1, startSlope, newEndSlope);

	//				do
	//				{
	//					double distanceY = j - playerY;
	//					updateVisibleTile(cellX, j, distanceX, distanceY);
	//					if(j == endCellY)
	//					{
	//						debugLines.emplace_back(Constants::mapOffset + playerX, playerY, Constants::mapOffset + playerX + i, playerY + i * startSlope);
	//						debugLines.emplace_back(Constants::mapOffset + playerX, playerY, Constants::mapOffset + playerX + i, playerY + i * endSlope);
	//						return;
	//					}

	//					j++;
	//				} while(j <= endCellY && getTileOpaque(cellX, j));

	//				debugLines.emplace_back(Constants::mapOffset + playerX, playerY, Constants::mapOffset + playerX + i, playerY + i * startSlope);
	//				startSlope = (j - playerY) / (cellX + 1 - playerX);
	//				if(startSlope >= endSlope)
	//				{
	//					double distanceY = j - playerY;
	//					updateVisibleTile(cellX, j, distanceX, distanceY);
	//					return;
	//				}
	//			}

	//			double distanceY = j - playerY;
	//			updateVisibleTile(cellX, j, distanceX, distanceY);
	//		}
	//	}
	//};
	//calculateOctant3(1, -1.0, 0.0);

	////Octant 4
	//auto calculateOctant4 = [this, lookupRange, playerX, playerY, &updateVisibleTile](this auto self, std::int64_t startOffset, double startSlope, double endSlope) -> void
	//{
	//	for(auto i = startOffset; i <= lookupRange * 2; i++)
	//	{
	//		std::int64_t cellX = playerX + i;
	//		if(cellX >= Constants::mapWidth)
	//			break;

	//		std::int64_t startCellY = std::min((std::int64_t)(playerY + i * startSlope), Constants::mapHeight - 1);
	//		std::int64_t endCellY = std::min((std::int64_t)(playerY + i * endSlope), Constants::mapHeight - 1);

	//		double distanceX = cellX - playerX;
	//		for(auto j = startCellY; j >= endCellY; j--)
	//		{
	//			if(getTileOpaque(cellX, j))
	//			{
	//				double newEndSlope = (j + 1.0 - playerY) / (cellX - playerX);
	//				if(startSlope > newEndSlope)
	//					self(i + 1, startSlope, newEndSlope);

	//				do
	//				{
	//					double distanceY = j - playerY;
	//					updateVisibleTile(cellX, j, distanceX, distanceY);
	//					if(j == endCellY)
	//					{
	//						debugLines.emplace_back(Constants::mapOffset + playerX, playerY, Constants::mapOffset + playerX + i, playerY + i * startSlope);
	//						debugLines.emplace_back(Constants::mapOffset + playerX, playerY, Constants::mapOffset + playerX + i, playerY + i * endSlope);
	//						return;
	//					}

	//					j--;
	//				} while(j >= endCellY && getTileOpaque(cellX, j));

	//				debugLines.emplace_back(Constants::mapOffset + playerX, playerY, Constants::mapOffset + playerX + i, playerY + i * startSlope);
	//				startSlope = (j + 1.0 - playerY) / (cellX + 1.0 - playerX);
	//				if(startSlope <= endSlope)
	//				{
	//					double distanceY = j - playerY;
	//					updateVisibleTile(cellX, j, distanceX, distanceY);
	//					return;
	//				}
	//			}

	//			double distanceY = j - playerY;
	//			updateVisibleTile(cellX, j, distanceX, distanceY);
	//		}
	//	}
	//};
	//calculateOctant4(1, 1.0, 0.0);

	////Octant 5
	//auto calculateOctant5 = [this, lookupRange, playerX, playerY, &updateVisibleTile](this auto self, std::int64_t startOffset, double startSlope, double endSlope) -> void
	//{
	//	for(auto i = startOffset; i <= lookupRange; i++)
	//	{
	//		std::int64_t startCellX = std::min((std::int64_t)(playerX + i * startSlope), Constants::mapWidth - 1);
	//		std::int64_t endCellX = std::min((std::int64_t)(playerX + i * endSlope), Constants::mapWidth - 1);

	//		std::int64_t cellY = playerY + i;
	//		if(cellY >= Constants::mapHeight)
	//			break;

	//		double distanceY = cellY - playerY;
	//		for(auto j = startCellX; j >= endCellX; j--)
	//		{
	//			if(getTileOpaque(j, cellY))
	//			{
	//				double newEndSlope = (j + 1 - playerX) / (cellY - playerY);
	//				if(startSlope > newEndSlope)
	//					self(i + 1, startSlope, newEndSlope);

	//				do
	//				{
	//					double distanceX = j - playerX;
	//					updateVisibleTile(j, cellY, distanceX, distanceY);
	//					if(j == endCellX)
	//					{
	//						debugLines.emplace_back(Constants::mapOffset + playerX, playerY, Constants::mapOffset + playerX + i * startSlope, playerY + i);
	//						debugLines.emplace_back(Constants::mapOffset + playerX, playerY, Constants::mapOffset + playerX + i * endSlope, playerY + i);
	//						return;
	//					}

	//					j--;
	//				} while(j >= endCellX && getTileOpaque(j, cellY));

	//				debugLines.emplace_back(Constants::mapOffset + playerX, playerY, Constants::mapOffset + playerX + i * startSlope, playerY + i);
	//				startSlope = (j + 1 - playerX) / (cellY + 1 - playerY);
	//				if(startSlope <= endSlope)
	//				{
	//					double distanceX = j - playerX;
	//					updateVisibleTile(j, cellY, distanceX, distanceY);
	//					return;
	//				}
	//			}

	//			double distanceX = j - playerX;
	//			updateVisibleTile(j, cellY, distanceX, distanceY);
	//		}
	//	}
	//};
	//calculateOctant5(1, 1.0, 0.0);

	////Octant 6
	//auto calculateOctant6 = [this, lookupRange, playerX, playerY, &updateVisibleTile](this auto self, std::int64_t startOffset, double startSlope, double endSlope) -> void
	//{
	//	for(auto i = startOffset; i <= lookupRange; i++)
	//	{
	//		std::int64_t startCellX = std::max((std::int64_t)(playerX + i * startSlope), 0ll);
	//		std::int64_t endCellX = std::max((std::int64_t)(playerX + i * endSlope), 0ll);

	//		std::int64_t cellY = playerY + i;
	//		if(cellY >= Constants::mapHeight)
	//			break;

	//		double distanceY = cellY - playerY;
	//		for(auto j = startCellX; j <= endCellX; j++)
	//		{
	//			if(getTileOpaque(j, cellY))
	//			{
	//				double newEndSlope = (j - playerX) / (cellY - playerY);
	//				if(startSlope < newEndSlope)
	//					self(i + 1, startSlope, newEndSlope);

	//				do
	//				{
	//					double distanceX = j - playerX;
	//					updateVisibleTile(j, cellY, distanceX, distanceY);
	//					if(j == endCellX)
	//					{
	//						debugLines.emplace_back(Constants::mapOffset + playerX, playerY, Constants::mapOffset + playerX + i * startSlope, playerY + i);
	//						debugLines.emplace_back(Constants::mapOffset + playerX, playerY, Constants::mapOffset + playerX + i * endSlope, playerY + i);
	//						return;
	//					}

	//					j++;
	//				} while(j <= endCellX && getTileOpaque(j, cellY));

	//				debugLines.emplace_back(Constants::mapOffset + playerX, playerY, Constants::mapOffset + playerX + i * startSlope, playerY + i);
	//				startSlope = (j - playerX) / (cellY + 1.0 - playerY);
	//				if(startSlope >= endSlope)
	//				{
	//					double distanceX = j - playerX;
	//					updateVisibleTile(j, cellY, distanceX, distanceY);
	//					return;
	//				}
	//			}

	//			double distanceX = j - playerX;
	//			updateVisibleTile(j, cellY, distanceX, distanceY);
	//		}
	//	}
	//};
	//calculateOctant6(1, -1.0, 0.0);

	////Octant 7
	//auto calculateOctant7 = [this, lookupRange, playerX, playerY, &updateVisibleTile](this auto self, std::int64_t startOffset, double startSlope, double endSlope) -> void
	//{
	//	for(auto i = startOffset; i <= lookupRange * 2; i++)
	//	{
	//		std::int64_t cellX = playerX - i;
	//		if(cellX < 0)
	//			break;

	//		std::int64_t startCellY = std::min((std::int64_t)(playerY - i * startSlope), Constants::mapHeight - 1);
	//		std::int64_t endCellY = std::min((std::int64_t)(playerY - i * endSlope), Constants::mapHeight - 1);

	//		double distanceX = cellX - playerX;
	//		for(auto j = startCellY; j >= endCellY; j--)
	//		{
	//			if(getTileOpaque(cellX, j))
	//			{
	//				double newEndSlope = (j + 1.0 - playerY) / (cellX + 1.0 - playerX);
	//				if(startSlope < newEndSlope)
	//					self(i + 1, startSlope, newEndSlope);

	//				do
	//				{
	//					double distanceY = j - playerY;
	//					updateVisibleTile(cellX, j, distanceX, distanceY);
	//					if(j == endCellY)
	//					{
	//						debugLines.emplace_back(Constants::mapOffset + playerX, playerY, Constants::mapOffset + playerX - i, playerY - i * startSlope);
	//						debugLines.emplace_back(Constants::mapOffset + playerX, playerY, Constants::mapOffset + playerX - i, playerY - i * endSlope);
	//						return;
	//					}

	//					j--;
	//				} while(j >= endCellY && getTileOpaque(cellX, j));

	//				debugLines.emplace_back(Constants::mapOffset + playerX, playerY, Constants::mapOffset + playerX - i, playerY - i * startSlope);
	//				startSlope = (j + 1.0 - playerY) / (cellX - playerX);
	//				if(startSlope >= endSlope)
	//				{
	//					double distanceY = j - playerY;
	//					updateVisibleTile(cellX, j, distanceX, distanceY);
	//					return;
	//				}
	//			}

	//			double distanceY = j - playerY;
	//			updateVisibleTile(cellX, j, distanceX, distanceY);
	//		}
	//	}
	//};
	//calculateOctant7(1, -1.0, 0.0);

	////Octant 8
	//auto calculateOctant8 = [this, lookupRange, playerX, playerY, &updateVisibleTile](this auto self, std::int64_t startOffset, double startSlope, double endSlope) -> void
	//{
	//	for(auto i = startOffset; i <= lookupRange * 2; i++)
	//	{
	//		std::int64_t cellX = playerX - i;
	//		if(cellX < 0)
	//			break;

	//		std::int64_t startCellY = std::max((std::int64_t)(playerY - i * startSlope), 0ll);
	//		std::int64_t endCellY = std::max((std::int64_t)(playerY - i * endSlope), 0ll);

	//		double distanceX = cellX - playerX;
	//		for(auto j = startCellY; j <= endCellY; j++)
	//		{
	//			if(getTileOpaque(cellX, j))
	//			{
	//				double newEndSlope = (j - playerY) / (cellX + 1.0 - playerX);
	//				if(startSlope > newEndSlope)
	//					self(i + 1, startSlope, newEndSlope);

	//				do
	//				{
	//					double distanceY = j - playerY;
	//					updateVisibleTile(cellX, j, distanceX, distanceY);
	//					if(j == endCellY)
	//					{
	//						debugLines.emplace_back(Constants::mapOffset + playerX, playerY, Constants::mapOffset + playerX - i, playerY - i * startSlope);
	//						debugLines.emplace_back(Constants::mapOffset + playerX, playerY, Constants::mapOffset + playerX - i, playerY - i * endSlope);
	//						return;
	//					}

	//					j++;
	//				} while(j <= endCellY && getTileOpaque(cellX, j));

	//				debugLines.emplace_back(Constants::mapOffset + playerX, playerY, Constants::mapOffset + playerX - i, playerY - i * startSlope);
	//				startSlope = (j - playerY) / (cellX - playerX);
	//				if(startSlope <= endSlope)
	//				{
	//					double distanceY = j - playerY;
	//					updateVisibleTile(cellX, j, distanceX, distanceY);
	//					return;
	//				}
	//			}

	//			double distanceY = j - playerY;
	//			updateVisibleTile(cellX, j, distanceX, distanceY);
	//		}
	//	}
	//};
	//calculateOctant8(1, 1.0, 0.0);
}
