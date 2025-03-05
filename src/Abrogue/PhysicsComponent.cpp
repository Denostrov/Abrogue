module PhysicsComponent;

import Game;

void PhysicsComponent::update()
{
	auto calculateNextStep = [this](double coordinate, double velocity, int32_t movementDirection, int32_t otherMovementDirection)
	{
		std::pair<double, double> result{coordinate, velocity};

		//Advance coordinate by half of time step
		result.first += velocity * Constants::tickDuration / 2.0;

		//Calculate forces
		double movementForce = movementDirection * walkingForce * frictionCoefficient * (otherMovementDirection != 0 ? 1.0 / std::sqrt(2.0) : 1.0);
		double frictionForce{};
		if(std::abs(velocity) > 0.0)
		{
			auto slowSpeed = std::max(0.2 * maxSpeed, std::abs(velocity));
			frictionForce = std::copysign(slowSpeed, -velocity) * frictionCoefficient * resistanceCoefficient;
		}

		//Apply forces and check if speed inverted due to friction
		auto velocitySign = std::signbit(velocity);
		result.second += (movementForce + frictionForce) / mass * Constants::tickDuration / 2.0;
		if(std::signbit(result.second) != velocitySign && movementForce == 0.0)
			result.second = 0.0;

		return result;
	};

	//Do forward euler integration in two steps to reflect coordinate change on same tick
	std::tie(x, velocityX) = calculateNextStep(x, velocityX, movementDirectionX, movementDirectionY);
	std::tie(y, velocityY) = calculateNextStep(y, velocityY, movementDirectionY, movementDirectionX);

	std::tie(x, velocityX) = calculateNextStep(x, velocityX, movementDirectionX, movementDirectionY);
	std::tie(y, velocityY) = calculateNextStep(y, velocityY, movementDirectionY, movementDirectionX);

	using TileCoords = std::pair<std::uint32_t, std::uint32_t>;

	//Get tile indices of center and all four corners
	TileCoords centerTile{x / QuadData::tileScale.x, y / QuadData::tileScale.y};
	TileCoords topRightTile{(x + QuadData::tileScale.x * 0.49f) / QuadData::tileScale.x, (y - QuadData::tileScale.y * 0.49f) / QuadData::tileScale.y};
	TileCoords bottomRightTile{(x + QuadData::tileScale.x * 0.49f) / QuadData::tileScale.x, (y + QuadData::tileScale.y * 0.49f) / QuadData::tileScale.y};
	TileCoords topLeftTile{(x - QuadData::tileScale.x * 0.49f) / QuadData::tileScale.x, (y - QuadData::tileScale.y * 0.49f) / QuadData::tileScale.y};
	TileCoords bottomLeftTile{(x - QuadData::tileScale.x * 0.49f) / QuadData::tileScale.x, (y + QuadData::tileScale.y * 0.49f) / QuadData::tileScale.y};

	//Check if corners overlap solid tiles
	bool topRightCollision = Game::getTileSolid(topRightTile.first, topRightTile.second);
	bool bottomRightCollision = Game::getTileSolid(bottomRightTile.first, bottomRightTile.second);
	bool topLeftCollision = Game::getTileSolid(topLeftTile.first, topLeftTile.second);
	bool bottomLeftCollision = Game::getTileSolid(bottomLeftTile.first, bottomLeftTile.second);

	auto checkCollision = [this, centerTile](bool collision, TileCoords tile, bool oppositeCollision, TileCoords oppositeTile,
											 bool& xCollision, bool& yCollision, bool movingTowards)
	{
		//No collision or wedged between two corners
		if(!collision || oppositeCollision)
			return;

		//No collisions with neighboring corners
		if(!xCollision && !yCollision)
		{
			auto horizontalTileSolid = Game::getTileSolid(tile.first - 2 * (tile.first - oppositeTile.first), tile.second);
			auto verticalTileSolid = Game::getTileSolid(tile.first, tile.second - 2 * (tile.second - oppositeTile.second));
			if(movingTowards && (horizontalTileSolid || verticalTileSolid) && !isEnteringCorridor)
			{
				if(horizontalTileSolid && verticalTileSolid)
				{
					if(std::abs(velocityX) < std::abs(velocityY) || isInVerticalCorridor)
					{
						velocityY = 0.0;
						y = (oppositeTile.second + 0.5f) * QuadData::tileScale.y;
					}
					else
					{
						velocityX = 0.0;
						x = (oppositeTile.first + 0.5f) * QuadData::tileScale.x;
					}
					isEnteringCorridor = true;
					corridorX = x;
					corridorY = y;
				}
				else if(verticalTileSolid)
				{
					velocityY = 0.0;
					y = (oppositeTile.second + 0.5f) * QuadData::tileScale.y;
					isEnteringCorridor = true;
					corridorX = x;
					corridorY = y;
				}
				//Detect vertical hallway
				else if(horizontalTileSolid)
				{
					velocityX = 0.0;
					x = (oppositeTile.first + 0.5f) * QuadData::tileScale.x;
					isEnteringCorridor = true;
					corridorX = x;
					corridorY = y;
				}
			}
			else
			{
				//Resolve corner collision towards direction of greater offset
				if(std::abs(x - (tile.first + 0.5f) * QuadData::tileScale.x) <= std::abs(y - (tile.second + 0.5f) * QuadData::tileScale.y) * 0.5f)
				{
					velocityY = 0.0;
					y = (oppositeTile.second + 0.5f) * QuadData::tileScale.y;
				}
				else
				{
					velocityX = 0.0;
					x = (oppositeTile.first + 0.5f) * QuadData::tileScale.x;
				}
			}
		}
		//Collision with neighboring corners means a wall
		else
		{
			if(xCollision)
			{
				velocityX = 0.0;
				x = (oppositeTile.first + 0.5f) * QuadData::tileScale.x;
				xCollision = false;
			}
			if(yCollision)
			{
				velocityY = 0.0;
				y = (oppositeTile.second + 0.5f) * QuadData::tileScale.y;
				yCollision = false;
			}
		}
	};
	checkCollision(topRightCollision, topRightTile, bottomLeftCollision, bottomLeftTile, bottomRightCollision, topLeftCollision, velocityX > 0.0 && velocityY < 0.0);
	checkCollision(bottomRightCollision, bottomRightTile, topLeftCollision, topLeftTile, topRightCollision, bottomLeftCollision, velocityX > 0.0 && velocityY > 0.0);
	checkCollision(bottomLeftCollision, bottomLeftTile, topRightCollision, topRightTile, topLeftCollision, bottomRightCollision, velocityX < 0.0 && velocityY > 0.0);
	checkCollision(topLeftCollision, topLeftTile, bottomRightCollision, bottomRightTile, bottomLeftCollision, topRightCollision, velocityX < 0.0 && velocityY < 0.0);

	if(isEnteringCorridor && (std::abs(x - corridorX) > QuadData::tileScale.x || std::abs(y - corridorY) > QuadData::tileScale.y))
		isEnteringCorridor = false;

	isInVerticalCorridor = (Game::getTileSolid((x + QuadData::tileScale.x * 0.55f) / QuadData::tileScale.x, (y - QuadData::tileScale.y * 0.49f) / QuadData::tileScale.y) &&
			Game::getTileSolid((x - QuadData::tileScale.x * 0.55f) / QuadData::tileScale.x, (y - QuadData::tileScale.y * 0.49f) / QuadData::tileScale.y)) 
		|| (Game::getTileSolid((x + QuadData::tileScale.x * 0.55f) / QuadData::tileScale.x, (y + QuadData::tileScale.y * 0.49f) / QuadData::tileScale.y) &&
			Game::getTileSolid((x - QuadData::tileScale.x * 0.55f) / QuadData::tileScale.x, (y + QuadData::tileScale.y * 0.49f) / QuadData::tileScale.y));
	isInHorizontalCorridor = (Game::getTileSolid((x + QuadData::tileScale.x * 0.49f) / QuadData::tileScale.x, (y - QuadData::tileScale.y * 0.55f) / QuadData::tileScale.y) &&
							Game::getTileSolid((x + QuadData::tileScale.x * 0.49f) / QuadData::tileScale.x, (y + QuadData::tileScale.y * 0.55f) / QuadData::tileScale.y))
		|| (Game::getTileSolid((x - QuadData::tileScale.x * 0.49f) / QuadData::tileScale.x, (y - QuadData::tileScale.y * 0.55f) / QuadData::tileScale.y) &&
			Game::getTileSolid((x - QuadData::tileScale.x * 0.49f) / QuadData::tileScale.x, (y + QuadData::tileScale.y * 0.55f) / QuadData::tileScale.y));
}