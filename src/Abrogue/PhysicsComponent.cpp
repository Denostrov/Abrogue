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

	//Remember values to update previous later
	bool copyTopRightCollision = topRightCollision;
	bool copyBottomRightCollision = bottomRightCollision;
	bool copyTopLeftCollision = topLeftCollision;
	bool copyBottomLeftCollision = bottomLeftCollision;

	auto checkCollision = [this, centerTile](bool collision, bool previousCollision, TileCoords tile, bool oppositeCollision, TileCoords oppositeTile,
											 bool& xCollision, bool& yCollision, bool previousXCollision, bool previousYCollision)
	{
		//No collision or wedged between two corners
		if(!collision || oppositeCollision)
			return;

		//No collisions with neighboring corners
		if(!xCollision && !yCollision)
		{
			//Detect horizontal hallway if collisions flip-flopped
			if(!previousCollision && previousXCollision)
			{
				velocityY = 0.0;
				y = (oppositeTile.second + 0.5f) * QuadData::tileScale.y;
			}
			//Detect vertical hallway
			else if(!previousCollision && previousYCollision)
			{
				velocityX = 0.0;
				x = (oppositeTile.first + 0.5f) * QuadData::tileScale.x;
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
	checkCollision(topRightCollision, previousTopRightCollision, topRightTile, bottomLeftCollision, bottomLeftTile, bottomRightCollision, topLeftCollision, previousBottomRightCollision, previousTopLeftCollision);
	checkCollision(bottomRightCollision, previousBottomRightCollision, bottomRightTile, topLeftCollision, topLeftTile, topRightCollision, bottomLeftCollision, previousTopRightCollision, previousBottomLeftCollision);
	checkCollision(bottomLeftCollision, previousBottomLeftCollision, bottomLeftTile, topRightCollision, topRightTile, topLeftCollision, bottomRightCollision, previousTopLeftCollision, previousBottomRightCollision);
	checkCollision(topLeftCollision, previousTopLeftCollision, topLeftTile, bottomRightCollision, bottomRightTile, bottomLeftCollision, topRightCollision, previousBottomLeftCollision, previousTopRightCollision);

	//Update previous values for door detection
	previousTopRightCollision = copyTopRightCollision;
	previousBottomRightCollision = copyBottomRightCollision;
	previousBottomLeftCollision = copyBottomLeftCollision;
	previousTopLeftCollision = copyTopLeftCollision;
}