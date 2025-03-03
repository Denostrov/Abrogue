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
	if(topRightCollision)
	{
		if(topRightTileX > centerTileX && topRightTileY < centerTileY)
		{
			if(!bottomRightCollision && !topLeftCollision)
			{
				if(std::abs(x - (centerTileX + 0.5f) * QuadData::tileScale.x) <= std::abs(y - (centerTileY + 0.5f) * QuadData::tileScale.y))
				{
					velocityX = 0.0;
					x = (centerTileX + 0.5f) * QuadData::tileScale.x;
				}
				else
				{
					velocityY = 0.0;
					y = (centerTileY + 0.5f) * QuadData::tileScale.y;
				}
			}
			else
			{
				if(bottomRightCollision)
				{
					velocityX = 0.0;
					x = (centerTileX + 0.5f) * QuadData::tileScale.x;
					bottomRightCollision = false;
				}
				if(topLeftCollision)
				{
					velocityY = 0.0;
					y = (centerTileY + 0.5f) * QuadData::tileScale.y;
					topLeftCollision = false;
				}
			}
		}
		else if(topRightTileX > centerTileX)
		{
			if(!bottomRightCollision && velocityY > 0.0)
			{
				velocityY = 0.0;
				y = (centerTileY + 1.0f + 0.5f) * QuadData::tileScale.y;
			}
			else
			{
				velocityX = 0.0;
				x = (centerTileX + 0.5f) * QuadData::tileScale.x;
			}
		}
		else if(topRightTileY < centerTileY)
		{
			if(!topLeftCollision && velocityX < 0.0)
			{
				velocityX = 0.0;
				x = (centerTileX - 1.0f + 0.5f) * QuadData::tileScale.x;
			}
			else
			{
				velocityY = 0.0;
				y = (centerTileY + 0.5f) * QuadData::tileScale.y;
			}
		}
	}
	if(bottomRightCollision)
	{
		if(bottomRightTileX > centerTileX && bottomRightTileY > centerTileY)
		{
			if(!topRightCollision && !bottomLeftCollision)
			{
				if(std::abs(x - (centerTileX + 0.5f) * QuadData::tileScale.x) <= std::abs(y - (centerTileY + 0.5f) * QuadData::tileScale.y))
				{
					velocityX = 0.0;
					x = (centerTileX + 0.5f) * QuadData::tileScale.x;
				}
				else
				{
					velocityY = 0.0;
					y = (centerTileY + 0.5f) * QuadData::tileScale.y;
				}
			}
			else
			{
				if(topRightCollision)
				{
					velocityX = 0.0;
					x = (centerTileX + 0.5f) * QuadData::tileScale.x;
				}
				if(bottomLeftCollision)
				{
					velocityY = 0.0;
					y = (centerTileY + 0.5f) * QuadData::tileScale.y;
					bottomLeftCollision = false;
				}
			}
		}
		else if(bottomRightTileX > centerTileX)
		{
			velocityX = 0.0;
			x = (centerTileX + 0.5f) * QuadData::tileScale.x;
		}
		else if(bottomRightTileY > centerTileY)
		{
			velocityY = 0.0;
			y = (centerTileY + 0.5f) * QuadData::tileScale.y;
		}
	}
	if(bottomLeftCollision)
	{
		if(bottomLeftTileX < centerTileX && bottomLeftTileY > centerTileY)
		{
			if(!topLeftCollision && !bottomRightCollision)
			{
				if(std::abs(x - (centerTileX + 0.5f) * QuadData::tileScale.x) <= std::abs(y - (centerTileY + 0.5f) * QuadData::tileScale.y))
				{
					velocityX = 0.0;
					x = (centerTileX + 0.5f) * QuadData::tileScale.x;
				}
				else
				{
					velocityY = 0.0;
					y = (centerTileY + 0.5f) * QuadData::tileScale.y;
				}
			}
			else
			{
				if(topLeftCollision)
				{
					velocityX = 0.0;
					x = (centerTileX + 0.5f) * QuadData::tileScale.x;
					topLeftCollision = false;
				}
				if(bottomRightCollision)
				{
					velocityY = 0.0;
					y = (centerTileY + 0.5f) * QuadData::tileScale.y;
				}
			}
		}
		else if(bottomLeftTileX < centerTileX)
		{
			velocityX = 0.0;
			x = (centerTileX + 0.5f) * QuadData::tileScale.x;
		}
		else if(bottomLeftTileY > centerTileY)
		{
			velocityY = 0.0;
			y = (centerTileY + 0.5f) * QuadData::tileScale.y;
		}
	}
	if(topLeftCollision)
	{
		if(topLeftTileX < centerTileX && topLeftTileY < centerTileY)
		{
			if(!bottomLeftCollision && !topRightCollision)
			{
				if(std::abs(x - (centerTileX + 0.5f) * QuadData::tileScale.x) <= std::abs(y - (centerTileY + 0.5f) * QuadData::tileScale.y))
				{
					velocityX = 0.0;
					x = (centerTileX + 0.5f) * QuadData::tileScale.x;
				}
				else
				{
					velocityY = 0.0;
					y = (centerTileY + 0.5f) * QuadData::tileScale.y;
				}
			}
			else
			{
				if(bottomLeftCollision)
				{
					velocityX = 0.0;
					x = (centerTileX + 0.5f) * QuadData::tileScale.x;
				}
				if(topRightCollision)
				{
					velocityY = 0.0;
					y = (centerTileY + 0.5f) * QuadData::tileScale.y;
				}
			}
		}
		else if(topLeftTileX < centerTileX)
		{
			velocityX = 0.0;
			x = (centerTileX + 0.5f) * QuadData::tileScale.x;
		}
		else if(topLeftTileY < centerTileY)
		{
			velocityY = 0.0;
			y = (centerTileY + 0.5f) * QuadData::tileScale.y;
		}
	}
}