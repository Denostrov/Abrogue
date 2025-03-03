module PhysicsComponent;

import Game;

void PhysicsComponent::update()
{
	auto calculateNextStep = [this](double coordinate, double velocity, int32_t movementDirection, int32_t otherMovementDirection)
	{
		std::pair<double, double> result{coordinate, velocity};

		result.first += velocity * Constants::tickDuration;

		double movementForce = movementDirection * walkingForce * frictionCoefficient * (otherMovementDirection != 0 ? 1.0 / std::sqrt(2.0) : 1.0);
		double frictionForce{};
		if(std::abs(velocity) > 0.0)
		{
			auto slowSpeed = std::max(0.2 * maxSpeed, std::abs(velocity));
			frictionForce = std::copysign(slowSpeed, -velocity) * frictionCoefficient * resistanceCoefficient;
		}

		auto velocitySign = std::signbit(velocity);
		result.second += (movementForce + frictionForce) / mass * Constants::tickDuration;
		if(std::signbit(result.second) != velocitySign && movementForce == 0.0)
			result.second = 0.0;

		return result;
	};

	std::tie(x, velocityX) = calculateNextStep(x, velocityX, movementDirectionX, movementDirectionY);
	std::tie(y, velocityY) = calculateNextStep(y, velocityY, movementDirectionY, movementDirectionX);

	using TileCoords = std::pair<std::uint32_t, std::uint32_t>;

	std::uint32_t centerTileX = x / QuadData::tileScale.x;
	std::uint32_t centerTileY = y / QuadData::tileScale.y;

	std::uint32_t topRightTileX = (x + QuadData::tileScale.x * 0.49f) / QuadData::tileScale.x;
	std::uint32_t topRightTileY = (y - QuadData::tileScale.y * 0.49f) / QuadData::tileScale.y;

	std::uint32_t bottomRightTileX = (x + QuadData::tileScale.x * 0.49f) / QuadData::tileScale.x;
	std::uint32_t bottomRightTileY = (y + QuadData::tileScale.y * 0.49f) / QuadData::tileScale.y;

	std::uint32_t topLeftTileX = (x - QuadData::tileScale.x * 0.49f) / QuadData::tileScale.x;
	std::uint32_t topLeftTileY = (y - QuadData::tileScale.y * 0.49f) / QuadData::tileScale.y;

	std::uint32_t bottomLeftTileX = (x - QuadData::tileScale.x * 0.49f) / QuadData::tileScale.x;
	std::uint32_t bottomLeftTileY = (y + QuadData::tileScale.y * 0.49f) / QuadData::tileScale.y;

	bool topRightCollision = Game::getTileSolid(topRightTileX, topRightTileY);
	bool bottomRightCollision = Game::getTileSolid(bottomRightTileX, bottomRightTileY);
	bool topLeftCollision = Game::getTileSolid(topLeftTileX, topLeftTileY);
	bool bottomLeftCollision = Game::getTileSolid(bottomLeftTileX, bottomLeftTileY);

	auto checkCollision = [this, centerTileX, centerTileY](bool collision, TileCoords tile, bool oppositeCollision, TileCoords oppositeTile,
												  bool& xCollision, bool& yCollision, bool movingAwayX, bool movingAwayY)
	{
		if(!collision || oppositeCollision)
			return;

		if(!xCollision && !yCollision)
		{
			if(tile.first != oppositeTile.first && tile.second != oppositeTile.second)
			{
				if(centerTileX != tile.first && movingAwayX)
				{
					velocityX = 0.0;
					x = (oppositeTile.first + 0.5f) * QuadData::tileScale.x;
				}
				else if(centerTileY != tile.second && movingAwayY)
				{
					velocityY = 0.0;
					y = (oppositeTile.second + 0.5f) * QuadData::tileScale.y;
				}
				else
				{
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
			else if(tile.first == oppositeTile.first)
			{
				velocityY = 0.0;
				y = (oppositeTile.second + 0.5f) * QuadData::tileScale.y;
			}
			else if(tile.second == oppositeTile.second)
			{
				velocityX = 0.0;
				x = (oppositeTile.first + 0.5f) * QuadData::tileScale.x;
			}
		}
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
	checkCollision(topRightCollision, {topRightTileX, topRightTileY}, bottomLeftCollision, {bottomLeftTileX, bottomLeftTileY}, bottomRightCollision, topLeftCollision, velocityX <= 0.0, velocityY >= 0.0);
	checkCollision(bottomRightCollision, {bottomRightTileX, bottomRightTileY}, topLeftCollision, {topLeftTileX, topLeftTileY}, topRightCollision, bottomLeftCollision, velocityX <= 0.0, velocityY <= 0.0);
	checkCollision(bottomLeftCollision, {bottomLeftTileX, bottomLeftTileY}, topRightCollision, {topRightTileX, topRightTileY}, topLeftCollision, bottomRightCollision, velocityX >= 0.0, velocityY <= 0.0);
	checkCollision(topLeftCollision, {topLeftTileX, topLeftTileY}, bottomRightCollision, {bottomRightTileX, bottomRightTileY}, bottomLeftCollision, topRightCollision, velocityX >= 0.0, velocityY >= 0.0);
}