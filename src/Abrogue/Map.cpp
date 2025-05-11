module Map;

import GameSystems;

void Map::init()
{
	generateLevel();

	for(std::int64_t y = 0; y < Constants::mapHeight; y++)
	{
		for(std::int64_t x = 0; x < Constants::mapWidth; x++)
		{
			getTile(x, y).quadReference = quadPool.insert(QuadData{{Constants::mapOffset + x + 0.5f, y + 0.5f},
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
	{
		updateTileProperties(tiles[lastVisibleTiles[i]], 0.25);
		tileBrightnessMask[lastVisibleTiles[i]] = 0.0;
	}

	auto [playerX, playerY] = player.getPosition();
	auto [playerVx, playerVy] = player.getVelocity();
	playerX += playerVx * deltaTime;
	playerY += playerVy * deltaTime;

	std::int64_t playerCellX = playerX;
	std::int64_t playerCellY = playerY;
	std::uint64_t playerCell = playerCellX + playerCellY * Constants::mapWidth;
	updateTileProperties(tiles[playerCell], 1.0);
	lastVisibleTiles[0] = playerCell;
	lastVisibleTilesSize = 1;
	tileBrightnessMask[playerCell] = 1.0;

	double visionRange = 8.0;
	std::int64_t lookupRange = std::ceil(visionRange);

	auto updateVisibleTile = [this, &updateTileProperties, visionRange](std::int64_t x, std::int64_t y, double distanceX, double distanceY)
	{
		double distance = std::sqrt(distanceX * distanceX / 4.0 + distanceY * distanceY);
		if(distance > visionRange)
			return;

		double lightStrength = std::clamp((visionRange - distance) / 2.0 + 0.5, 0.5, 1.0);
		updateTileProperties(getTile(x, y), lightStrength);

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
					double newEndSlope = (j - playerY) / (cellX + 1 - playerX);
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

bool Map::getTileSolid(std::int64_t x, std::int64_t y) const
{
	logger.extraAssert(x >= 0 && x < Constants::mapWidth && y >= 0 && y < Constants::mapHeight, "Requested is tile solid out of bounds");

	auto type = getTile(x, y).type;
	return type == TileType::eBedrock || type == TileType::eWall;
}

bool Map::getTileOpaque(std::int64_t x, std::int64_t y) const
{
	logger.extraAssert(x >= 0 && x < Constants::mapWidth && y >= 0 && y < Constants::mapHeight, "Requested is tile opaque out of bounds");

	auto type = getTile(x, y).type;
	return type == TileType::eBedrock || type == TileType::eWall || type == TileType::eBush || type == TileType::eDoor || type == TileType::eExit;
}

bool Map::getTileFloor(std::int64_t x, std::int64_t y) const
{
	logger.extraAssert(x >= 0 && x < Constants::mapWidth && y >= 0 && y < Constants::mapHeight, "Requested is tile a floor out of bounds");

	auto type = getTile(x, y).type;
	return type == TileType::eFloor || type == TileType::eGrass || type == TileType::eBush;
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
		getTile(x, 0).type = TileType::eBedrock;
		levelData.tilesOccupiedMask[x] = true;

		getTile(x, Constants::mapHeight - 1).type = TileType::eBedrock;
		levelData.tilesOccupiedMask[x + (Constants::mapHeight - 1) * Constants::mapWidth] = true;
	}

	for(std::int64_t y = 0; y < Constants::mapHeight; y++)
	{
		getTile(0, y).type = TileType::eBedrock;
		levelData.tilesOccupiedMask[y * Constants::mapWidth] = true;

		getTile(Constants::mapWidth - 1, y).type = TileType::eBedrock;
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
				getTile(x, y).type = TileType::eFloor;
		}

		for(auto y = originY - 1; y < originY + height + 1; y++)
		{
			for(auto x = originX - 1; x < originX + width + 1; x++)
				levelData.tilesOccupiedMask[x + y * Constants::mapWidth] = true;
		}

		std::uint64_t value = std::random_device()() % 2;
		getTile(doorX, doorY).type = value == 0 ? TileType::eDoor : TileType::eFloor;

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
					getTile(x, y).type = value == 0 ? TileType::eDoor : TileType::eFloor;
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
					getTile(x, y).type = value == 0 ? TileType::eDoor : TileType::eFloor;
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
					getTile(x, y).type = value == 0 ? TileType::eDoor : TileType::eFloor;
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
					getTile(x, y).type = value == 0 ? TileType::eDoor : TileType::eFloor;
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
			getTile(x, y).type = TileType::eFloor;
	}
	for(auto y = startingRoom.originY - 1; y < startingRoom.originY + startingRoom.height + 1; y++)
	{
		for(auto x = startingRoom.originX - 1; x < startingRoom.originX + startingRoom.width + 1; x++)
			levelData.tilesOccupiedMask[x + y * Constants::mapWidth] = true;
	}
	getTile(40, 34).type = TileType::eExit;

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
}
