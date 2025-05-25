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

	std::int64_t playerCellX = playerX;
	std::int64_t playerCellY = playerY;
	updateVisibleTile(playerX, playerY, playerCellX + 0.5 - playerX, playerCellY + 0.5 - playerY);

	//Get initial left slopes
	double topStartSlope{}, bottomStartSlope{};
	std::int64_t startCellX{playerCellX - 1};
	while(true)
	{
		updateVisibleTile(startCellX, playerCellY, startCellX + 0.5 - playerX, playerCellY + 0.5 - playerY);

		if(getTileOpaque(startCellX, playerCellY))
		{
			topStartSlope = (playerCellY - playerY) / (startCellX + 1 - playerX);
			bottomStartSlope = (playerCellY + 1 - playerY) / (startCellX + 1 - playerX);

			break;
		}
		startCellX--;
	}

	//Get initial right slopes
	double topEndSlope{}, bottomEndSlope{};
	std::int64_t endCellX{playerCellX + 1};
	while(true)
	{
		updateVisibleTile(endCellX, playerCellY, endCellX + 0.5 - playerX, playerCellY + 0.5 - playerY);

		if(getTileOpaque(endCellX, playerCellY))
		{
			topEndSlope = (playerCellY - playerY) / (endCellX - playerX);
			bottomEndSlope = (playerCellY + 1 - playerY) / (endCellX - playerX);

			break;
		}
		endCellX++;
	}

	auto getSlopesCorrect = [](double startSlope, double endSlope)
	{
		return (startSlope < 0.0 && endSlope > 0.0) || (endSlope < startSlope && (endSlope > 0.0 || startSlope < 0.0));
	};

	auto calculateVisibilitySector = [this, playerX, playerY, &updateVisibleTile, &getSlopesCorrect](this auto&& self, std::int64_t currentCellY, std::int64_t directionY,
																									 double startSlope, double startEnterX, std::int64_t startCellX,
																									 double endSlope, double endEnterX, std::int64_t endCellX)
	{
		while(true)
		{
			//Check if left entry cell is opaque
			std::int64_t currentStartCellX{};
			if(getTileOpaque(startCellX, currentCellY))
			{
				//Move right until first non opaque cell found
				do
				{
					updateVisibleTile(startCellX, currentCellY, startCellX + 0.5 - playerX, currentCellY + 0.5 - playerY);

					//Terminate if no more empty cells
					if(startCellX == endCellX)
						return;

					startCellX++;

				} while(getTileOpaque(startCellX, currentCellY));
				updateVisibleTile(startCellX, currentCellY, startCellX + 0.5 - playerX, currentCellY + 0.5 - playerY);

				currentStartCellX = startCellX + 1;

				if(startCellX < playerX)
				{
					//New slope is towards top right corner of last blocking cell in top half and towards bottom right corner in bottom half
					startSlope = (currentCellY + (directionY == 1) - playerY) / (startCellX - playerX);
					startEnterX = startCellX;
					startCellX--;
				}
				else
				{
					//New slope is towards bottom right corner of last blocking cell in top half and towards top right corner in bottom half
					startSlope = (currentCellY + (directionY == -1) - playerY) / (startCellX - playerX);
					startEnterX = startCellX + 1.0 / std::abs(startSlope);
					startCellX = (std::int64_t)startEnterX;
				}
			}
			else
			{
				currentStartCellX = startCellX + 1;
				updateVisibleTile(startCellX, currentCellY, startCellX + 0.5 - playerX, currentCellY + 0.5 - playerY);

				//Move left until first opaque cell or move right until next row
				double directedStartSlope = startSlope * directionY;
				if(directedStartSlope < 0.0)
				{
					double initialEnterX = startEnterX;
					startEnterX += 1.0 / directedStartSlope;
					startCellX = startEnterX;
					for(auto i = currentStartCellX - 2; i >= startCellX; i--)
					{
						updateVisibleTile(i, currentCellY, i + 0.5 - playerX, currentCellY + 0.5 - playerY);
						if(getTileOpaque(i, currentCellY))
						{
							//New slope is towards top right corner of first opaque cell in top half and towards bottom right corner in bottom half
							startSlope = (currentCellY + (directionY == 1) - playerY) / (i + 1 - playerX);
							startEnterX = i + 1;
							startCellX = i;
							break;
						}
					}
				}
				else
				{
					startEnterX = startEnterX + 1.0 / directedStartSlope;
					startCellX = startEnterX;
				}
			}

			std::int64_t currentEndCellX{endCellX};
			double directedEndSlope = endSlope * directionY;
			if(directedEndSlope > 0.0)
			{
				if(getTileOpaque(endCellX, currentCellY))
				{
					do
					{
						updateVisibleTile(endCellX, currentCellY, endCellX + 0.5 - playerX, currentCellY + 0.5 - playerY);
						endCellX--;

					} while(getTileOpaque(endCellX, currentCellY));

					currentEndCellX = endCellX;
					endCellX++;

					if(endCellX > playerX)
					{
						endSlope = (currentCellY + (directionY == 1) - playerY) / (endCellX - playerX);
						endEnterX = endCellX;
					}
					else
					{
						endSlope = (currentCellY + (directionY == -1) - playerY) / (endCellX - playerX);
						endEnterX = endCellX + 1.0 / endSlope * directionY;
						endCellX = (std::int64_t)endEnterX;
					}
				}
				else
				{
					updateVisibleTile(currentEndCellX, currentCellY, currentEndCellX + 0.5 - playerX, currentCellY + 0.5 - playerY);
					double initialEndEnterX = endEnterX;
					endEnterX = endEnterX + 1.0 / directedEndSlope;
					endCellX = (std::int64_t)(endEnterX);
					for(auto i = currentEndCellX + 1; i <= endCellX; i++)
					{
						updateVisibleTile(i, currentCellY, i + 0.5 - playerX, currentCellY + 0.5 - playerY);
						if(getTileOpaque(i, currentCellY))
						{
							endSlope = (currentCellY + (directionY == 1) - playerY) / (i - playerX);
							endEnterX = i;
							endCellX = i;
						}
					}
				}
			}
			else
			{
				endEnterX = endEnterX + 1.0 / directedEndSlope;
				endCellX = (std::int64_t)(endEnterX);
			}

			for(auto i = currentStartCellX; i <= currentEndCellX; i++)
			{
				if(getTileOpaque(i, currentCellY))
				{
					double newEndSlope = i < playerX ? (currentCellY + (directionY == -1) - playerY) / (i - playerX) : (currentCellY + (directionY == 1) - playerY) / (i - playerX);
					if(getSlopesCorrect(startSlope * directionY, newEndSlope * directionY))
					{
						if(newEndSlope * directionY < 0.0)
						{
							double newEndEnterX = i + 1.0 / newEndSlope * directionY;
							std::int64_t newEndCellX = newEndEnterX;

							self(currentCellY + directionY, directionY, startSlope, startEnterX, startCellX, newEndSlope, newEndEnterX, newEndCellX);
						}
						else
							self(currentCellY + directionY, directionY, startSlope, startEnterX, startCellX, newEndSlope, i, i);
					}

					do
					{
						updateVisibleTile(i, currentCellY, i + 0.5 - playerX, currentCellY + 0.5 - playerY);

						if(i == currentEndCellX)
							return;
						i++;

					} while(getTileOpaque(i, currentCellY));

					startSlope = i < playerX ? (currentCellY + (directionY == 1) - playerY) / (i - playerX) : (currentCellY + (directionY == -1) - playerY) / (i - playerX);
					double directedStartSlope = startSlope * directionY;
					if(directedStartSlope < 0.0)
					{
						startEnterX = i;
						startCellX = i - 1;
					}
					else
					{
						startEnterX = i + 1.0 / directedStartSlope;
						startCellX = startEnterX;
					}
				}

				updateVisibleTile(i, currentCellY, i + 0.5 - playerX, currentCellY + 0.5 - playerY);
			}

			if(!getSlopesCorrect(startSlope * directionY, endSlope * directionY))
				return;

			currentCellY += directionY;
		}
	};
	calculateVisibilitySector(playerCellY - 1, -1, topStartSlope, startCellX + 1.0, startCellX, topEndSlope, endCellX, endCellX);
	calculateVisibilitySector(playerCellY + 1, 1, bottomStartSlope, startCellX + 1.0, startCellX, bottomEndSlope, endCellX, endCellX);
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

	std::int64_t playerCellX = playerX;
	std::int64_t playerCellY = playerY;
	updateVisibleTile(playerX, playerY, playerCellX + 0.5 - playerX, playerCellY + 0.5 - playerY);

	//Get initial left slopes
	double topStartSlope{}, bottomStartSlope{};
	std::int64_t startCellX{playerCellX - 1};
	while(true)
	{
		updateVisibleTile(startCellX, playerCellY, startCellX + 0.5 - playerX, playerCellY + 0.5 - playerY);

		if(getTileOpaque(startCellX, playerCellY))
		{
			topStartSlope = (playerCellY - playerY) / (startCellX + 1 - playerX);
			bottomStartSlope = (playerCellY + 1 - playerY) / (startCellX + 1 - playerX);

			debugLines.emplace_back(Constants::mapOffset + playerX, playerY, Constants::mapOffset + startCellX + 1, playerY);
			break;
		}
		startCellX--;
	}

	//Get initial right slopes
	double topEndSlope{}, bottomEndSlope{};
	std::int64_t endCellX{playerCellX + 1};
	while(true)
	{
		updateVisibleTile(endCellX, playerCellY, endCellX + 0.5 - playerX, playerCellY + 0.5 - playerY);

		if(getTileOpaque(endCellX, playerCellY))
		{
			topEndSlope = (playerCellY - playerY) / (endCellX - playerX);
			bottomEndSlope = (playerCellY + 1 - playerY) / (endCellX - playerX);

			debugLines.emplace_back(Constants::mapOffset + playerX, playerY, Constants::mapOffset + endCellX, playerY);
			break;
		}
		endCellX++;
	}

	auto getSlopesCorrect = [](double startSlope, double endSlope)
	{
		return (startSlope < 0.0 && endSlope > 0.0) || (endSlope < startSlope && (endSlope > 0.0 || startSlope < 0.0));
	};

	auto calculateVisibilitySector = [this, playerX, playerY, &updateVisibleTile, &getSlopesCorrect](this auto&& self, std::int64_t currentCellY, std::int64_t directionY,
																									 double startSlope, double startEnterX, std::int64_t startCellX,
																									 double endSlope, double endEnterX, std::int64_t endCellX)
	{
		while(true)
		{
			//Check if left entry cell is opaque
			std::int64_t currentStartCellX{};
			if(getTileOpaque(startCellX, currentCellY))
			{
				debugLines.emplace_back(Constants::mapOffset + playerX, playerY, Constants::mapOffset + startEnterX, currentCellY + (directionY == -1));

				//Move right until first non opaque cell found
				do
				{
					updateVisibleTile(startCellX, currentCellY, startCellX + 0.5 - playerX, currentCellY + 0.5 - playerY);

					//Terminate if no more empty cells
					if(startCellX == endCellX)
					{
						debugLines.emplace_back(Constants::mapOffset + playerX, playerY, Constants::mapOffset + endEnterX, currentCellY + (directionY == -1));
						return;
					}
					startCellX++;

				} while(getTileOpaque(startCellX, currentCellY));
				updateVisibleTile(startCellX, currentCellY, startCellX + 0.5 - playerX, currentCellY + 0.5 - playerY);

				currentStartCellX = startCellX + 1;

				if(startCellX < playerX)
				{
					//New slope is towards top right corner of last blocking cell in top half and towards bottom right corner in bottom half
					startSlope = (currentCellY + (directionY == 1) - playerY) / (startCellX - playerX);
					startEnterX = startCellX;
					startCellX--;
				}
				else
				{
					//New slope is towards bottom right corner of last blocking cell in top half and towards top right corner in bottom half
					startSlope =  (currentCellY + (directionY == -1) - playerY) / (startCellX - playerX);
					startEnterX = startCellX + 1.0 / std::abs(startSlope);
					startCellX = (std::int64_t)startEnterX;
				}
			}
			else
			{
				currentStartCellX = startCellX + 1;
				updateVisibleTile(startCellX, currentCellY, startCellX + 0.5 - playerX, currentCellY + 0.5 - playerY);

				//Move left until first opaque cell or move right until next row
				double directedStartSlope = startSlope * directionY;
				if(directedStartSlope < 0.0)
				{
					double initialEnterX = startEnterX;
					startEnterX += 1.0 / directedStartSlope;
					startCellX = startEnterX;
					for(auto i = currentStartCellX - 2; i >= startCellX; i--)
					{
						updateVisibleTile(i, currentCellY, i + 0.5 - playerX, currentCellY + 0.5 - playerY);
						if(getTileOpaque(i, currentCellY))
						{
							debugLines.emplace_back(Constants::mapOffset + playerX, playerY, Constants::mapOffset + i + 1, currentCellY + (directionY == -1) - (initialEnterX - i - 1) * startSlope);

							//New slope is towards top right corner of first opaque cell in top half and towards bottom right corner in bottom half
							startSlope = (currentCellY + (directionY == 1) - playerY) / (i + 1 - playerX);
							startEnterX = i + 1;
							startCellX = i;
							break;
						}
					}
				}
				else
				{
					startEnterX = startEnterX + 1.0 / directedStartSlope;
					startCellX = startEnterX;
				}
			}

			std::int64_t currentEndCellX{endCellX};
			double directedEndSlope = endSlope * directionY;
			if(directedEndSlope > 0.0)
			{
				if(getTileOpaque(endCellX, currentCellY))
				{
					debugLines.emplace_back(Constants::mapOffset + playerX, playerY, Constants::mapOffset + endEnterX, currentCellY + (directionY == -1));

					do
					{
						updateVisibleTile(endCellX, currentCellY, endCellX + 0.5 - playerX, currentCellY + 0.5 - playerY);
						endCellX--;

					} while(getTileOpaque(endCellX, currentCellY));

					currentEndCellX = endCellX;
					endCellX++;

					if(endCellX > playerX)
					{
						endSlope = (currentCellY + (directionY == 1) - playerY) / (endCellX - playerX);
						endEnterX = endCellX;
					}
					else
					{
						endSlope = (currentCellY + (directionY == -1) - playerY) / (endCellX - playerX);
						endEnterX = endCellX + 1.0 / endSlope * directionY;
						endCellX = (std::int64_t)endEnterX;
					}
				}
				else
				{
					updateVisibleTile(currentEndCellX, currentCellY, currentEndCellX + 0.5 - playerX, currentCellY + 0.5 - playerY);
					double initialEndEnterX = endEnterX;
					endEnterX = endEnterX + 1.0 / directedEndSlope;
					endCellX = (std::int64_t)(endEnterX);
					for(auto i = currentEndCellX + 1; i <= endCellX; i++)
					{
						updateVisibleTile(i, currentCellY, i + 0.5 - playerX, currentCellY + 0.5 - playerY);
						if(getTileOpaque(i, currentCellY))
						{
							debugLines.emplace_back(Constants::mapOffset + playerX, playerY, Constants::mapOffset + i, currentCellY + (directionY == -1) + (i - initialEndEnterX) * endSlope);

							endSlope = (currentCellY + (directionY == 1) - playerY) / (i - playerX);
							endEnterX = i;
							endCellX = i;
						}
					}
				}
			}
			else
			{
				endEnterX = endEnterX + 1.0 / directedEndSlope;
				endCellX = (std::int64_t)(endEnterX);
			}

			for(auto i = currentStartCellX; i <= currentEndCellX; i++)
			{
				if(getTileOpaque(i, currentCellY))
				{
					double newEndSlope = i < playerX ? (currentCellY + (directionY == -1) - playerY) / (i - playerX) : (currentCellY + (directionY == 1) - playerY) / (i - playerX);
					if(getSlopesCorrect(startSlope * directionY, newEndSlope * directionY))
					{
						if(newEndSlope * directionY < 0.0)
						{
							double newEndEnterX = i + 1.0 / newEndSlope * directionY;
							std::int64_t newEndCellX = newEndEnterX;

							self(currentCellY + directionY, directionY, startSlope, startEnterX, startCellX, newEndSlope, newEndEnterX, newEndCellX);
						}
						else
							self(currentCellY + directionY, directionY, startSlope, startEnterX, startCellX, newEndSlope, i, i);
					}

					do
					{
						updateVisibleTile(i, currentCellY, i + 0.5 - playerX, currentCellY + 0.5 - playerY);

						if(i == currentEndCellX)
							return;
						i++;

					} while(getTileOpaque(i, currentCellY));

					startSlope = i < playerX ? (currentCellY + (directionY == 1) - playerY) / (i - playerX) : (currentCellY + (directionY == -1) - playerY) / (i - playerX);
					double directedStartSlope = startSlope * directionY;
					if(directedStartSlope < 0.0)
					{
						startEnterX = i;
						startCellX = i - 1;
					}
					else
					{
						startEnterX = i + 1.0 / directedStartSlope;
						startCellX = startEnterX;
					}
				}

				updateVisibleTile(i, currentCellY, i + 0.5 - playerX, currentCellY + 0.5 - playerY);
			}

			if(!getSlopesCorrect(startSlope * directionY, endSlope * directionY))
				return;

			currentCellY += directionY;
		}
	};
	calculateVisibilitySector(playerCellY - 1, -1, topStartSlope, startCellX + 1.0, startCellX, topEndSlope, endCellX, endCellX);
	calculateVisibilitySector(playerCellY + 1, 1, bottomStartSlope, startCellX + 1.0, startCellX, bottomEndSlope, endCellX, endCellX);
}
